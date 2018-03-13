/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VSThresholdFilterWidget.h"

#include <QtCore/QString>

#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>

#include "ui_VSThresholdFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VSThresholdFilterWidget::vsInternals : public Ui::VSThresholdFilterWidget
{
public:
  vsInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdFilterWidget::VSThresholdFilterWidget(VSThresholdFilter* filter, vtkRenderWindowInteractor *interactor, QWidget* parent)
: VSAbstractFilterWidget(parent)
, m_Internals(new vsInternals())
, m_ThresholdFilter(filter)
{
  m_Internals->setupUi(this);

  connect(m_Internals->scalarsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(scalarIndexChanged(int)));
  connect(m_Internals->minSpinBox, SIGNAL(editingFinished()), this, SLOT(minSpinBoxValueChanged()));
  connect(m_Internals->maxSpinBox, SIGNAL(editingFinished()), this, SLOT(maxSpinBoxValueChanged()));
  connect(m_Internals->minSlider, SIGNAL(valueChanged(int)), this, SLOT(minSliderValueChanged()));
  connect(m_Internals->maxSlider, SIGNAL(valueChanged(int)), this, SLOT(maxSliderValueChanged()));

  setupScalarsComboBox();

  if (m_ThresholdFilter->isInitialized() == true)
  {
    m_ThresholdFilter->setInitialized(false);
    reset();
    apply();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::apply()
{
  VSAbstractFilterWidget::apply();

  m_ThresholdFilter->apply(getScalarName(), getLowerBound(), getUpperBound());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::reset()
{
  QString lastArrayName = m_ThresholdFilter->getLastArrayName();
  double lastLowerValue = m_ThresholdFilter->getLastMinValue();
  double lastUpperValue = m_ThresholdFilter->getLastMaxValue();

  setScalarName(lastArrayName);
  setLowerThreshold(lastLowerValue);
  setUpperThreshold(lastUpperValue);

  cancelChanges();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::setupScalarsComboBox()
{
  vtkDataSet* inputData = m_ThresholdFilter->getOutput();
  m_Internals->scalarsComboBox->clear();

  // Return if there is no vtkDataSet or vtkCellData
  if(false == (inputData && inputData->GetCellData()))
  {
    return;
  }

  // Add scalar arrays to the combo box
  int numArrays = inputData->GetCellData()->GetNumberOfArrays();
  for(int i = 0; i < numArrays; i++)
  {
    vtkDataArray* dataArray = inputData->GetCellData()->GetArray(i);
    if(dataArray->GetNumberOfComponents() == 1)
    {
      const char* arrayName = dataArray->GetName();
      m_Internals->scalarsComboBox->addItem(arrayName);
    }
  }

  // Initialize values based on the first scalar array
  if(m_Internals->scalarsComboBox->count() > 0)
  {
    m_Internals->scalarsComboBox->setCurrentIndex(0);
    initRange();
  }
  else
  {
    adjustSize();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::initRange()
{
  vtkDataArray* dataArray = getDataArray(getScalarName());

  double range[2];
  if(dataArray)
  {
    range[0] = dataArray->GetRange()[0];
    range[1] = dataArray->GetRange()[1];
  }
  else
  {
    range[0] = 0.0;
    range[1] = 0.0;
  }

  setScalarRange(range[0], range[1]);
  setLowerThreshold(range[0]);
  setUpperThreshold(range[1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::updateRange(QString arrayName)
{
  if(arrayName.isEmpty())
  {
    arrayName = getScalarName();
  }

  vtkDataArray* dataArray = getDataArray(arrayName);

  if(dataArray)
  {
    double min = dataArray->GetRange()[0];
    double max = dataArray->GetRange()[1];

    setScalarRange(min, max);
  }
  else
  {
    setScalarRange(0.0, 0.0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkDataArray* VSThresholdFilterWidget::getDataArray(QString name)
{
  vtkDataSet* dataSet = m_ThresholdFilter->getOutput();
  if(dataSet && dataSet->GetCellData())
  {
    return dataSet->GetCellData()->GetArray(qPrintable(name));
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::scalarIndexChanged(int index)
{
  updateRange();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSThresholdFilterWidget::getScalarName()
{
  return m_Internals->scalarsComboBox->currentText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::setScalarName(QString arrayName)
{
  m_Internals->scalarsComboBox->setCurrentText(arrayName);

  updateRange(arrayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdFilterWidget::getLowerBound()
{
  return m_Internals->minSpinBox->value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::setLowerThreshold(double min)
{
  m_Internals->minSpinBox->setValue(min);
  minSpinBoxValueChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::setUpperThreshold(double max)
{
  m_Internals->maxSpinBox->setValue(max);
  maxSpinBoxValueChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdFilterWidget::getUpperBound()
{
  return m_Internals->maxSpinBox->value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::setScalarRange(double min, double max)
{
  m_Internals->minSpinBox->setMinimum(min);
  m_Internals->maxSpinBox->setMinimum(min);

  m_Internals->minSpinBox->setMaximum(max);
  m_Internals->maxSpinBox->setMaximum(max);

  // adjust sliders accordingly
  minSpinBoxValueChanged();
  maxSpinBoxValueChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::minSpinBoxValueChanged()
{
  spinBoxToSlider(m_Internals->minSpinBox, m_Internals->minSlider);

  if(checkMinSpinBox())
  {
    spinBoxToSlider(m_Internals->maxSpinBox, m_Internals->maxSlider);
  }

  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::maxSpinBoxValueChanged()
{
  spinBoxToSlider(m_Internals->maxSpinBox, m_Internals->maxSlider);

  if(checkMaxSpinBox())
  {
    spinBoxToSlider(m_Internals->minSpinBox, m_Internals->minSlider);
  }

  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::minSliderValueChanged()
{
  sliderToSpinBox(m_Internals->minSlider, m_Internals->minSpinBox);

  if(checkMinSpinBox())
  {
    spinBoxToSlider(m_Internals->maxSpinBox, m_Internals->maxSlider);
  }

  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::maxSliderValueChanged()
{
  sliderToSpinBox(m_Internals->maxSlider, m_Internals->maxSpinBox);

  if(checkMaxSpinBox())
  {
    spinBoxToSlider(m_Internals->minSpinBox, m_Internals->minSlider);
  }

  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::sliderToSpinBox(QSlider* slider, QDoubleSpinBox* spinBox)
{
  int sliderRange = slider->maximum() - slider->minimum();
  double spinBoxRange = spinBox->maximum() - spinBox->minimum();

  int sliderValue = slider->value() + slider->minimum();

  double percentage = sliderValue / (double)sliderRange;
  spinBox->setValue(percentage * spinBoxRange + spinBox->minimum());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilterWidget::spinBoxToSlider(QDoubleSpinBox* spinBox, QSlider* slider)
{
  int sliderRange = slider->maximum() - slider->minimum();
  double spinBoxRange = spinBox->maximum() - spinBox->minimum();

  double spinBoxValue = spinBox->value() - spinBox->minimum();

  double percentage = spinBoxValue / spinBoxRange;
  slider->setValue(percentage * sliderRange + slider->minimum());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSThresholdFilterWidget::checkMinSpinBox()
{
  if(m_Internals->minSpinBox->value() > m_Internals->maxSpinBox->value())
  {
    m_Internals->maxSpinBox->setValue(m_Internals->minSpinBox->value());
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSThresholdFilterWidget::checkMaxSpinBox()
{
  if(m_Internals->minSpinBox->value() > m_Internals->maxSpinBox->value())
  {
    m_Internals->minSpinBox->setValue(m_Internals->maxSpinBox->value());
    return true;
  }

  return false;
}
