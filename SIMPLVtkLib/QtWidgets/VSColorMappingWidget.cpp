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

#include "VSColorMappingWidget.h"

namespace
{
const QString MultiOption = "---";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSColorMappingWidget::VSColorMappingWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSColorMappingWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilters(VSAbstractFilter::FilterListType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setupGui()
{
  m_presetsDialog = new ColorPresetsDialog();

  connect(m_Ui->mapScalarsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setScalarsMapped(int)));
  connect(m_Ui->showScalarBarComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setScalarBarSetting(int)));
  connect(m_Ui->invertColorScaleBtn, &QPushButton::clicked, this, &VSColorMappingWidget::invertScalarBar);
  connect(m_Ui->alphaSlider, &QSlider::sliderReleased, [=]() {
    int value = m_Ui->alphaSlider->value();
    alphaSliderMoved(value);
  });
  connect(m_Ui->selectPresetColorsBtn, &QPushButton::clicked, this, &VSColorMappingWidget::selectPresetColors);
  connect(m_presetsDialog, &ColorPresetsDialog::applyPreset, this, &VSColorMappingWidget::loadPresetColors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::updateViewSettingInfo()
{
  // Clear the visualization settings if the current VSFilterViewSettings is null
  if(m_ViewSettings.size() == 0)
  {
    removeMultiValueOption(m_Ui->showScalarBarComboBox);
    removeMultiValueOption(m_Ui->mapScalarsComboBox);

    m_Ui->showScalarBarComboBox->setCurrentIndex(static_cast<int>(VSFilterViewSettings::ScalarBarSetting::Never));
    m_Ui->mapScalarsComboBox->setCurrentIndex(static_cast<int>(VSFilterViewSettings::ColorMapping::None));

    this->setEnabled(false);
    return;
  }

  this->setEnabled(true);

  // Block Signals
  m_Ui->showScalarBarComboBox->blockSignals(true);
  m_Ui->mapScalarsComboBox->blockSignals(true);
  m_Ui->alphaSlider->blockSignals(true);

  // Get Values
  VSFilterViewSettings::ScalarBarSetting scalarBar = VSFilterViewSettings::GetScalarBarSettings(m_ViewSettings);
  VSFilterViewSettings::ColorMapping colorMapping = VSFilterViewSettings::GetColorMapping(m_ViewSettings);
  double alpha = VSFilterViewSettings::GetAlpha(m_ViewSettings);

  // Set UI Values
  if(VSFilterViewSettings::ScalarBarSetting::MultiValues == scalarBar)
  {
    addMultiValueOption(m_Ui->showScalarBarComboBox);
  }
  else
  {
    removeMultiValueOption(m_Ui->showScalarBarComboBox);
    m_Ui->showScalarBarComboBox->setCurrentIndex(static_cast<int>(scalarBar));
  }

  if(VSFilterViewSettings::ColorMapping::MultiValues == colorMapping)
  {
    addMultiValueOption(m_Ui->mapScalarsComboBox);
  }
  else
  {
    removeMultiValueOption(m_Ui->mapScalarsComboBox);
    m_Ui->mapScalarsComboBox->setCurrentIndex(static_cast<int>(colorMapping));
  }

  m_Ui->alphaSlider->setValue(alpha * 100);

  // Unblock Signals
  m_Ui->showScalarBarComboBox->blockSignals(false);
  m_Ui->mapScalarsComboBox->blockSignals(false);
  m_Ui->alphaSlider->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setFilters(VSAbstractFilter::FilterListType filters)
{
  m_Filters = filters;

  bool filterExists = (filters.size() > 0);
  if(filterExists && m_ViewWidget)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filters));
  }
  else
  {
    connectFilterViewSettings(VSFilterViewSettings::Collection());
  }

  // Check if VSFilterSettings exist and are valid
  VSFilterViewSettings::ActorType actorType = VSFilterViewSettings::GetActorType(m_ViewSettings);
  switch(actorType)
  {
  case VSFilterViewSettings::ActorType::DataSet:
    m_Ui->contentContainer->setVisible(true);
    m_Ui->colorMappingContainer->setVisible(true);
    break;
  case VSFilterViewSettings::ActorType::Image2D:
    m_Ui->contentContainer->setVisible(true);
    m_Ui->colorMappingContainer->setVisible(false);
    break;
  case VSFilterViewSettings::ActorType::Invalid:
  default:
    m_Ui->contentContainer->setVisible(false);
    m_Ui->colorMappingContainer->setVisible(false);
    break;
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  m_ViewWidget = viewWidget;

  if(m_ViewWidget && m_Filters.size() > 0)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filters));
  }
  else
  {
    connectFilterViewSettings(VSFilterViewSettings::Collection());
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::connectFilterViewSettings(VSFilterViewSettings::Collection filterSettings)
{
  for(VSFilterViewSettings* settings : m_ViewSettings)
  {
    disconnect(settings, &VSFilterViewSettings::mapColorsChanged, this, &VSColorMappingWidget::listenMapColors);
    disconnect(settings, &VSFilterViewSettings::alphaChanged, this, &VSColorMappingWidget::listenAlpha);
    disconnect(settings, &VSFilterViewSettings::scalarBarSettingChanged, this, &VSColorMappingWidget::listenScalarBar);
  }

  m_ViewSettings = filterSettings;

  for(VSFilterViewSettings* settings : m_ViewSettings)
  {
    connect(settings, &VSFilterViewSettings::mapColorsChanged, this, &VSColorMappingWidget::listenMapColors);
    connect(settings, &VSFilterViewSettings::alphaChanged, this, &VSColorMappingWidget::listenAlpha);
    connect(settings, &VSFilterViewSettings::scalarBarSettingChanged, this, &VSColorMappingWidget::listenScalarBar);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setScalarsMapped(int colorMappingIndex)
{
  // Do not change values when the multi-value option is selected
  if(hasMultiValueOption(m_Ui->mapScalarsComboBox))
  {
    if(colorMappingIndex == 0)
    {
      return;
    }

    colorMappingIndex--;
  }

  VSFilterViewSettings::ColorMapping colorMapping = static_cast<VSFilterViewSettings::ColorMapping>(colorMappingIndex);
  for(VSFilterViewSettings* settings : m_ViewSettings)
  {
    settings->setMapColors(colorMapping);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setScalarBarSetting(int index)
{
  // Do not change values when the multi-value option is selected
  if(hasMultiValueOption(m_Ui->showScalarBarComboBox))
  {
    if(index == 0)
    {
      return;
    }

    index--;
  }

  VSFilterViewSettings::ScalarBarSetting setting = static_cast<VSFilterViewSettings::ScalarBarSetting>(index);
  for(VSFilterViewSettings* settings : m_ViewSettings)
  {
    settings->setScalarBarSetting(setting);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::selectPresetColors()
{
  if(m_ViewSettings.size() == 0)
  {
    return;
  }

  m_presetsDialog->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::loadPresetColors(const QJsonObject& preset, const QPixmap& pixmap)
{
  for(VSFilterViewSettings* settings : m_ViewSettings)
  {
    settings->loadPresetColors(preset);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::invertScalarBar()
{
  for(VSFilterViewSettings* settings : m_ViewSettings)
  {
    settings->invertScalarBar();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::alphaSliderMoved(int value)
{
  for(VSFilterViewSettings* settings : m_ViewSettings)
  {
    settings->setAlpha(value / 100.0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::listenMapColors(VSFilterViewSettings::ColorMapping colorMapping)
{
  colorMapping = VSFilterViewSettings::GetColorMapping(m_ViewSettings);

  m_Ui->mapScalarsComboBox->blockSignals(true);
  if(VSFilterViewSettings::ColorMapping::MultiValues == colorMapping)
  {
    addMultiValueOption(m_Ui->mapScalarsComboBox);
  }
  else
  {
    removeMultiValueOption(m_Ui->mapScalarsComboBox);
    m_Ui->mapScalarsComboBox->setCurrentIndex(static_cast<int>(colorMapping));
  }
  m_Ui->mapScalarsComboBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::listenAlpha(double alpha)
{
  m_Ui->alphaSlider->blockSignals(true);
  m_Ui->alphaSlider->setValue(alpha * 100);
  m_Ui->alphaSlider->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::listenScalarBar(const VSFilterViewSettings::ScalarBarSetting&)
{
  VSFilterViewSettings::ScalarBarSetting scalarBar = VSFilterViewSettings::GetScalarBarSettings(m_ViewSettings);

  m_Ui->showScalarBarComboBox->blockSignals(true);
  if(VSFilterViewSettings::ScalarBarSetting::MultiValues == scalarBar)
  {
    addMultiValueOption(m_Ui->showScalarBarComboBox);
  }
  else
  {
    removeMultiValueOption(m_Ui->showScalarBarComboBox);
    m_Ui->showScalarBarComboBox->setCurrentIndex(static_cast<int>(scalarBar));
  }
  m_Ui->showScalarBarComboBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSColorMappingWidget::hasMultiValueOption(QComboBox* comboBox) const
{
  return comboBox->findText(::MultiOption) != -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::addMultiValueOption(QComboBox* comboBox)
{
  if(hasMultiValueOption(comboBox))
  {
    // Select the Multi-Value option
    comboBox->setCurrentIndex(comboBox->findText(::MultiOption));
    return;
  }

  comboBox->insertItem(0, ::MultiOption);
  comboBox->setCurrentIndex(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::removeMultiValueOption(QComboBox* comboBox)
{
  if(!hasMultiValueOption(comboBox))
  {
    return;
  }

  int index = comboBox->findText(::MultiOption);
  comboBox->removeItem(index);
}
