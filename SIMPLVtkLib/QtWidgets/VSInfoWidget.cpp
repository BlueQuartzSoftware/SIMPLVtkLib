/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "VSInfoWidget.h"

#include "ui_VSInfoWidget.h"

class VSInfoWidget::VSInternals : public Ui::VSInfoWidget
{
public:
  VSInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInfoWidget::VSInfoWidget(QWidget* parent)
  : QWidget(parent)
  , m_Internals(new VSInternals())
{
  m_Internals->setupUi(this);
  setupGui();

  setFilter(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setupGui()
{
  m_presetsDialog = new ColorPresetsDialog();

  connect(m_Internals->activeArrayCombo, SIGNAL(currentIndexChanged(int)),
    this, SLOT(updateActiveArrayIndex(int)));
  connect(m_Internals->activeComponentCombo, SIGNAL(currentIndexChanged(int)),
    this, SLOT(updateActiveComponentIndex(int)));
  connect(m_Internals->mapScalarsCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(setScalarsMapped(int)));
  connect(m_Internals->showScalarBarCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(setScalarBarVisible(int)));
  connect(m_Internals->invertColorScaleBtn, SIGNAL(clicked()),
    this, SLOT(invertScalarBar()));
  connect(m_Internals->alphaSlider, SIGNAL(valueChanged(int)),
    this, SLOT(alphaSliderMoved(int)));
  connect(m_Internals->selectPresetColorsBtn, SIGNAL(clicked()),
    this, SLOT(selectPresetColors()));
  connect(m_presetsDialog, SIGNAL(applyPreset(const QJsonObject&, const QPixmap&)),
    this, SLOT(loadPresetColors(const QJsonObject&, const QPixmap&)));

  connect(m_Internals->applyBtn, SIGNAL(clicked()),
    this, SLOT(applyFilter()));
  connect(m_Internals->resetBtn, SIGNAL(clicked()),
    this, SLOT(resetFilter()));
  connect(m_Internals->deleteBtn, SIGNAL(clicked()),
    this, SLOT(deleteFilter()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::applyFilter()
{
  if(nullptr == m_Filter)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::resetFilter()
{
  if(nullptr == m_Filter)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::deleteFilter()
{
  if(nullptr == m_Filter)
  {
    return;
  }

  emit filterDeleted(m_Filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setFilter(VSAbstractFilter* filter)
{
  m_Filter = filter;

  bool filterExists = (nullptr != filter);
  m_Internals->applyBtn->setEnabled(filterExists);
  m_Internals->resetBtn->setEnabled(filterExists);
  m_Internals->deleteBtn->setEnabled(filterExists);

  if(filterExists && m_ViewController)
  {
    m_ViewSettings = m_ViewController->getViewSettings(m_Filter);
  }
  else
  {
    m_ViewSettings = nullptr;
  }

  updateFilterInfo();
  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setViewController(VSViewController* viewController)
{
  m_ViewController = viewController;

  if(m_ViewController)
  {
    m_ViewSettings = m_ViewController->getViewSettings(m_Filter);
  }
  else
  {
    m_ViewSettings = nullptr;
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateFilterInfo()
{
  // TODO: display information both about the filter

  // Add and set the array / component combo box values
  m_Internals->activeArrayCombo->blockSignals(true);
  m_Internals->activeArrayCombo->clear();

  if(m_Filter)
  {
    m_Internals->activeArrayCombo->addItems(m_Filter->getArrayNames());

    if(m_ViewSettings)
    {
      int activeIndex = m_ViewSettings->getActiveArrayIndex();
      m_Internals->activeArrayCombo->setCurrentIndex(activeIndex);
    }
    else
    {
      m_Internals->activeArrayCombo->setCurrentIndex(0);
    }
  }

  m_Internals->activeArrayCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateViewSettingInfo()
{
  // Clear the visualization settings if the current VSFilterViewSettings is null
  if(nullptr == m_ViewSettings)
  {
    m_Internals->activeArrayCombo->setCurrentIndex(-1);
    m_Internals->activeComponentCombo->setCurrentIndex(-1);

    m_Internals->showScalarBarCheckBox->setChecked(Qt::Unchecked);
    m_Internals->mapScalarsCheckBox->setChecked(Qt::Unchecked);

    m_Internals->viewSettingsWidget->setEnabled(false);
    return;
  }

  // Apply the current filter view settings to the widget
  m_Internals->viewSettingsWidget->setEnabled(true);

  int activeArrayIndex = m_ViewSettings->getActiveArrayIndex();
  int activeComponentIndex = m_ViewSettings->getActiveComponentIndex() + 1;

  // Array
  m_Internals->activeArrayCombo->setCurrentIndex(activeArrayIndex);
  updateActiveArrayIndex(activeArrayIndex);

  // Components
  int numComponents = m_ViewSettings->getNumberOfComponents(activeArrayIndex);
  if(numComponents > 1)
  {
    m_Internals->activeComponentCombo->setCurrentIndex(activeComponentIndex);
  }

  m_Internals->showScalarBarCheckBox->setChecked(m_ViewSettings->isScalarBarVisible() ? Qt::Checked : Qt::Unchecked);
  m_Internals->mapScalarsCheckBox->setChecked(m_ViewSettings->getMapColors() ? Qt::Checked : Qt::Unchecked);
  m_Internals->alphaSlider->setValue(m_ViewSettings->getAlpha() * 100);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateActiveArrayIndex(int index)
{
  int componentIndex = 0;
  if(m_ViewSettings && m_ViewSettings->getActiveArrayIndex() == index)
  {
    componentIndex = m_ViewSettings->getActiveComponentIndex();
  }

  // Set the active component combo box values
  m_Internals->activeComponentCombo->clear();

  QStringList componentList = m_Filter->getComponentList(index);
  bool multiComponents = componentList.size() > 1;
  m_Internals->activeComponentCombo->setEnabled(multiComponents);
  
  if(multiComponents)
  {
    m_Internals->activeComponentCombo->setEnabled(true);
    m_Internals->activeComponentCombo->addItems(componentList);
    m_Internals->activeComponentCombo->setCurrentIndex(componentIndex);
  }
  else
  {
    m_Internals->activeComponentCombo->setEnabled(false);
  }

  if(m_ViewSettings)
  {
    m_ViewSettings->setActiveArrayIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateActiveComponentIndex(int index)
{
  if(m_ViewSettings)
  {
    if(m_Internals->activeComponentCombo->maxCount() > 1)
    {
      index--;
    }

    m_ViewSettings->setActiveComponentIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setScalarsMapped(int checkState)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  bool checked = checkState == Qt::Checked;
  m_ViewSettings->setMapColors(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setScalarBarVisible(int checkState)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  bool checked = checkState == Qt::Checked;
  m_ViewSettings->setScalarBarVisible(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::selectPresetColors()
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_presetsDialog->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::loadPresetColors(const QJsonObject& preset, const QPixmap& pixmap)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->loadPresetColors(preset);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::invertScalarBar()
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->invertScalarBar();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::alphaSliderMoved(int value)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->setAlpha(value / 100.0);
}
