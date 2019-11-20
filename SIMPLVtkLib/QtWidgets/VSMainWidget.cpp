/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include "VSMainWidget.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"

#include "ui_VSMainWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VSMainWidget::vsInternals : public Ui::VSMainWidget
{
public:
  vsInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMainWidget::VSMainWidget(QWidget* parent)
: VSMainWidgetBase(parent)
, m_Internals(new vsInternals())
{
  m_Internals->setupUi(this);

  VSAbstractViewWidget* viewWidget = m_Internals->viewWidget;
  viewWidget->setController(getController());
  connectViewWidget(viewWidget);
  setActiveView(viewWidget);

  createFilterMenu();
  connectSlots();
  setCurrentFilter(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::connectSlots()
{
  // Filter Slots
  connect(m_Internals->clipBtn, SIGNAL(clicked()), m_ActionAddClip, SLOT(trigger()));
  connect(m_Internals->sliceBtn, SIGNAL(clicked()), m_ActionAddSlice, SLOT(trigger()));
  connect(m_Internals->thresholdBtn, SIGNAL(clicked()), m_ActionAddThreshold, SLOT(trigger()));

  // Camera Slots
  connect(m_Internals->cameraXpBtn, SIGNAL(clicked()), this, SLOT(activeCameraXPlus()));
  connect(m_Internals->cameraYpBtn, SIGNAL(clicked()), this, SLOT(activeCameraYPlus()));
  connect(m_Internals->cameraZpBtn, SIGNAL(clicked()), this, SLOT(activeCameraZPlus()));
  connect(m_Internals->cameraXmBtn, SIGNAL(clicked()), this, SLOT(activeCameraXMinus()));
  connect(m_Internals->cameraYmBtn, SIGNAL(clicked()), this, SLOT(activeCameraYMinus()));
  connect(m_Internals->cameraZmBtn, SIGNAL(clicked()), this, SLOT(activeCameraZMinus()));

  connect(getController(), &VSController::filterAdded, this, [=] { renderAll(); });
  connect(getController(), &VSController::dataImported, this, [=] { resetCamera(); });
  connect(getController(), SIGNAL(applyingDataFilters(int)), this, SLOT(importNumFilters(int)));
  connect(getController(), SIGNAL(dataFilterApplied(int)), this, SLOT(importedFilterNum(int)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::renderAll()
{
  QVector<VSAbstractViewWidget*> viewWidgets = getAllViewWidgets();
  for(auto iter = viewWidgets.begin(); iter != viewWidgets.end(); iter++)
  {
    VSVisualizationWidget* visualizationWidget = (*iter)->getVisualizationWidget();
    if(visualizationWidget)
    {
      // Render the VTK widget
      visualizationWidget->render();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::resetCamera()
{
  QVector<VSAbstractViewWidget*> viewWidgets = getAllViewWidgets();
  for(VSAbstractViewWidget* viewWidget : viewWidgets)
  {
    viewWidget->resetCamera();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::setActiveView(VSAbstractViewWidget* viewWidget)
{
  VSMainWidgetBase::setActiveView(viewWidget);

  if(getActiveViewWidget() != nullptr)
  {
    m_Internals->cameraXmBtn->setEnabled(true);
    m_Internals->cameraXpBtn->setEnabled(true);
    m_Internals->cameraYmBtn->setEnabled(true);
    m_Internals->cameraYpBtn->setEnabled(true);
    m_Internals->cameraZmBtn->setEnabled(true);
    m_Internals->cameraZpBtn->setEnabled(true);
  }
  else
  {
    m_Internals->cameraXmBtn->setDisabled(true);
    m_Internals->cameraXpBtn->setDisabled(true);
    m_Internals->cameraYmBtn->setDisabled(true);
    m_Internals->cameraYpBtn->setDisabled(true);
    m_Internals->cameraZmBtn->setDisabled(true);
    m_Internals->cameraZpBtn->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::setCurrentFilter(VSAbstractFilter* filter)
{
  VSMainWidgetBase::setCurrentFilter(filter);

  // Check if each Filter Type can be added
  // Clip Filter
  bool enableClip = VSClipFilter::CompatibleWithParent(filter);
  m_Internals->clipBtn->setEnabled(enableClip);
  m_ActionAddClip->setEnabled(enableClip);

  // Slice
  bool enableSlice = VSSliceFilter::CompatibleWithParent(filter);
  m_Internals->sliceBtn->setEnabled(enableSlice);
  m_ActionAddSlice->setEnabled(enableSlice);

  // Crop Filter
  bool enableCrop = VSCropFilter::CompatibleWithParent(filter);
  m_ActionAddCrop->setEnabled(enableCrop);

  // Mask
  bool enableMask = VSMaskFilter::CompatibleWithParent(filter);
  m_ActionAddMask->setEnabled(enableMask);

  // Threshold
  bool enableThreshold = VSThresholdFilter::CompatibleWithParent(filter);
  m_Internals->thresholdBtn->setEnabled(enableThreshold);
  m_ActionAddThreshold->setEnabled(enableThreshold);

  // Text
  bool enableText = VSTextFilter::CompatibleWithParent(filter);
  m_ActionAddText->setEnabled(enableText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::updateCurrentFilter(VSAbstractFilter* filter)
{
  setCurrentFilter(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::activeCameraXPlus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camXPlus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::activeCameraYPlus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camYPlus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::activeCameraZPlus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camZPlus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::activeCameraXMinus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camXMinus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::activeCameraYMinus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camYMinus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::activeCameraZMinus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camZMinus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* VSMainWidget::getFilterMenu()
{
  return m_FilterMenu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::createFilterMenu()
{
  // Create Actions
  m_ActionAddClip = new QAction("Clip Filter");
  connect(m_ActionAddClip, SIGNAL(triggered()), this, SLOT(createClipFilter()));

  m_ActionAddCrop = new QAction("Crop Filter");
  connect(m_ActionAddCrop, SIGNAL(triggered()), this, SLOT(createCropFilter()));

  m_ActionAddSlice = new QAction("Slice Filter");
  connect(m_ActionAddSlice, SIGNAL(triggered()), this, SLOT(createSliceFilter()));

  m_ActionAddThreshold = new QAction("Threshold Filter");
  connect(m_ActionAddThreshold, SIGNAL(triggered()), this, SLOT(createThresholdFilter()));

  m_ActionAddMask = new QAction("Mask Filter");
  connect(m_ActionAddMask, SIGNAL(triggered()), this, SLOT(createMaskFilter()));

  m_ActionAddText = new QAction("Text Filter");
  connect(m_ActionAddText, SIGNAL(triggered()), this, SLOT(createTextFilter()));

  // Create Menu
  m_FilterMenu = new QMenu("Filters", this);
  m_FilterMenu->addAction(m_ActionAddClip);
  m_FilterMenu->addAction(m_ActionAddSlice);
  m_FilterMenu->addAction(m_ActionAddCrop);
  m_FilterMenu->addAction(m_ActionAddThreshold);
  m_FilterMenu->addAction(m_ActionAddMask);

  m_FilterMenu->addSeparator();

  m_FilterMenu->addAction(m_ActionAddText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::importNumFilters(int max)
{
  if(max == 0)
  {
    m_Internals->progressBar->setMaximum(1);
  }
  else
  {
    m_Internals->progressBar->setMaximum(max);
  }

  m_Internals->progressBar->setValue(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::importedFilterNum(int value)
{
  int maxValue = m_Internals->progressBar->maximum();
  if(value == maxValue)
  {
    m_Internals->progressBar->setValue(0);
  }
  else
  {
    m_Internals->progressBar->setValue(value);
  }
}
