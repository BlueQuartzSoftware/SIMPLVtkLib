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

#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSClipFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSCropFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSDataSetFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSMaskFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSSliceFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSThresholdFilterWidget.h"

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
  connect(m_Internals->activeArrayCombo, SIGNAL(currentIndexChanged(int)),
    this, SLOT(updateActiveArrayIndex(int)));
  connect(m_Internals->activeComponentCombo, SIGNAL(currentIndexChanged(int)),
    this, SLOT(updateActiveComponentIndex(int)));
  connect(m_Internals->mapScalarsCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(setScalarsMapped(int)));
  connect(m_Internals->showScalarBarCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(setScalarBarVisible(int)));
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

  if (static_cast<VSClipFilter*>(filter) != nullptr)
  {
    VSClipFilter* clipFilter = static_cast<VSClipFilter*>(filter);
    VSClipFilterWidget* fw = new VSClipFilterWidget(clipFilter, nullptr, this);
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
  m_Internals->activeArrayCombo->clear();
  if(m_Filter)
  {
    m_Internals->activeArrayCombo->addItems(m_Filter->getArrayNames());

    if(m_ViewSettings)
    {
      m_Internals->activeArrayCombo->setCurrentIndex(m_ViewSettings->getActiveArrayIndex());
    }
    else
    {
      m_Internals->activeArrayCombo->setCurrentIndex(0);
    }
  }
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

  m_Internals->activeArrayCombo->setCurrentIndex(m_ViewSettings->getActiveArrayIndex());
  m_Internals->activeComponentCombo->setCurrentIndex(m_ViewSettings->getActiveComponentIndex());

  m_Internals->showScalarBarCheckBox->setChecked(m_ViewSettings->isScalarBarVisible() ? Qt::Checked : Qt::Unchecked);
  m_Internals->mapScalarsCheckBox->setChecked(m_ViewSettings->getMapColors() ? Qt::Checked : Qt::Unchecked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateActiveArrayIndex(int index)
{
  // Set the active component combo box values
  m_Internals->activeComponentCombo->clear();

  QStringList componentList = m_Filter->getComponentList(index);
  bool multiComponents = componentList.size() > 1;
  m_Internals->activeComponentCombo->setEnabled(multiComponents);
  
  if(multiComponents)
  {
    m_Internals->activeComponentCombo->addItems(componentList);
    m_Internals->activeComponentCombo->setCurrentIndex(0);
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
