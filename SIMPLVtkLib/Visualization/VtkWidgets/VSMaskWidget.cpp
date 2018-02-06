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

#include "VSMaskWidget.h"

#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkImplicitFunction.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

#include <QDoubleSpinBox>
#include <QSlider>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskWidget::VSMaskWidget(QWidget* parent, QString mask, double bounds[6], vtkRenderWindowInteractor* iren)
: VSAbstractWidget(parent, bounds, iren)
{
  setupUi(this);

  // adjust the vtkWidget when values are changed
  connect(maskComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentMaskChanged(int)));

  setMaskName(mask);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskWidget::~VSMaskWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskWidget::updateMaskNames(vtkDataSet* inputData)
{
  QString selectedMaskName = maskComboBox->currentText();

  maskComboBox->clear();
  int numArrays = inputData->GetCellData()->GetNumberOfArrays();

  for(int i = 0; i < numArrays; i++)
  {
    const char* arrayName = inputData->GetCellData()->GetArray(i)->GetName();
    maskComboBox->addItem(arrayName);
  }

  maskComboBox->setCurrentText(selectedMaskName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskWidget::setMaskName(QString mask)
{
  maskComboBox->setCurrentText(mask);
  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSMaskWidget::getMaskName()
{
  return maskComboBox->currentText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSMaskWidget::getMaskId()
{
  return maskComboBox->currentIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskWidget::enable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskWidget::disable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskWidget::currentMaskChanged(int index)
{
  emit modified();
}
