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

#include "VSMainWidgetBase.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSClipFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSCropFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSDataSetFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSSliceFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSMaskFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSThresholdFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMainWidgetBase::VSMainWidgetBase(QWidget* parent)
  : QWidget(parent)
  , m_Controller(new VSController())
{
  connectSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::connectSlots()
{
  connect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*)),
    this, SLOT(setCurrentFilter(VSAbstractFilter*)));
  connect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*)), 
    this, SLOT(filterAdded(VSAbstractFilter*)));
  connect(m_Controller, SIGNAL(filterRemoved(VSAbstractFilter*)), 
    this, SLOT(filterRemoved(VSAbstractFilter*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::connectViewWidget(VSAbstractViewWidget* viewWidget)
{
  connect(viewWidget, SIGNAL(viewWidgetCreated(VSAbstractViewWidget*)),
    this, SLOT(connectViewWidget(VSAbstractViewWidget*)));

  connect(viewWidget, SIGNAL(markActive(VSAbstractViewWidget*)),
    this, SLOT(setActiveView(VSAbstractViewWidget*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController* VSMainWidgetBase::getController()
{
  return m_Controller;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget* VSMainWidgetBase::getActiveViewWidget()
{
  return m_ActiveViewWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractViewWidget*> VSMainWidgetBase::getAllViewWidgets()
{
  return this->findChildren<VSAbstractViewWidget*>().toVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterView* VSMainWidgetBase::getFilterView()
{
  return m_FilterView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setFilterView(VSFilterView* view)
{
  if(m_FilterView)
  {
    disconnect(m_FilterView, SIGNAL(filterClicked(VSAbstractFilter*)));
    disconnect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)), view, SLOT(changeViewWidget(VSAbstractViewWidget*)));
  }

  view->setController(m_Controller);

  m_FilterView = view;
  connect(view, SIGNAL(filterClicked(VSAbstractFilter*)), this, SLOT(setCurrentFilter(VSAbstractFilter*)));
  connect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)), view, SLOT(setViewWidget(VSAbstractViewWidget*)));
  
  view->setViewWidget(m_ActiveViewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInfoWidget* VSMainWidgetBase::getInfoWidget()
{
  return m_InfoWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setInfoWidget(VSInfoWidget* infoWidget)
{
  if(m_InfoWidget)
  {
    disconnect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)),
      m_InfoWidget, SLOT(setFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));
    disconnect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)),
      m_InfoWidget, SLOT(setViewWidget(VSAbstractViewWidget*)));
    disconnect(infoWidget, SIGNAL(filterDeleted(VSAbstractFilter*)),
      this, SLOT(deleteFilter(VSAbstractFilter*)));
  }

  m_InfoWidget = infoWidget;

  if(m_InfoWidget)
  {
    connect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)),
      infoWidget, SLOT(setFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));
    connect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)),
      infoWidget, SLOT(setViewWidget(VSAbstractViewWidget*)));
    connect(infoWidget, SIGNAL(filterDeleted(VSAbstractFilter*)),
      this, SLOT(deleteFilter(VSAbstractFilter*)));

    VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(m_CurrentFilter);
    m_InfoWidget->setFilter(m_CurrentFilter, filterWidget);
    m_InfoWidget->setViewWidget(getActiveViewWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSMainWidgetBase::getCurrentFilter()
{
  return m_CurrentFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::filterAdded(VSAbstractFilter* filter)
{
  QVTKInteractor* interactor = nullptr;
  VSAbstractViewWidget* activeViewWidget = getActiveViewWidget();
  if (activeViewWidget != nullptr)
  {
    VSVisualizationWidget* vizWidget = activeViewWidget->getVisualizationWidget();
    if (vizWidget != nullptr)
    {
      interactor = vizWidget->GetInteractor();
    }
  }


  VSAbstractFilterWidget* fw = nullptr;
  if (dynamic_cast<VSClipFilter*>(filter) != nullptr)
  {
    VSClipFilter* vsFilter = dynamic_cast<VSClipFilter*>(filter);
    fw = new VSClipFilterWidget(vsFilter, interactor, this);
  }
  else if (dynamic_cast<VSCropFilter*>(filter) != nullptr)
  {
    VSCropFilter* vsFilter = dynamic_cast<VSCropFilter*>(filter);
    fw = new VSCropFilterWidget(vsFilter, interactor, this);
  }
  else if (dynamic_cast<VSMaskFilter*>(filter) != nullptr)
  {
    VSMaskFilter* vsFilter = dynamic_cast<VSMaskFilter*>(filter);
    fw = new VSMaskFilterWidget(vsFilter, interactor, this);
  }
  else if (dynamic_cast<VSDataSetFilter*>(filter) != nullptr)
  {
    VSDataSetFilter* vsFilter = dynamic_cast<VSDataSetFilter*>(filter);
    fw = new VSDataSetFilterWidget(vsFilter, this);
  }
  else if (dynamic_cast<VSSliceFilter*>(filter) != nullptr)
  {
    VSSliceFilter* vsFilter = dynamic_cast<VSSliceFilter*>(filter);
    fw = new VSSliceFilterWidget(vsFilter, interactor, this);
  }
  else if (dynamic_cast<VSThresholdFilter*>(filter) != nullptr)
  {
    VSThresholdFilter* vsFilter = dynamic_cast<VSThresholdFilter*>(filter);
    fw = new VSThresholdFilterWidget(vsFilter, interactor, this);
  }

  if (fw != nullptr)
  {
    m_FilterToFilterWidgetMap.insert(filter, fw);
  }

  setCurrentFilter(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::filterRemoved(VSAbstractFilter* filter)
{
  VSAbstractFilterWidget* fw = m_FilterToFilterWidgetMap.value(filter);
  delete fw;

  m_FilterToFilterWidgetMap.remove(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setActiveView(VSAbstractViewWidget* viewWidget)
{
  // Disconnect the old active view widget
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->setActive(false);

    disconnect(m_ActiveViewWidget, SIGNAL(viewWidgetClosed()), this, SLOT(activeViewClosed()));
    disconnect(m_ActiveViewWidget, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)),
      this, SLOT(setFilterVisibility(VSFilterViewSettings*, bool)));
  }

  m_ActiveViewWidget = viewWidget;

  // Connect the new active view widget
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->setActive(true);

    connect(m_ActiveViewWidget, SIGNAL(viewWidgetClosed()), this, SLOT(activeViewClosed()));
    connect(m_ActiveViewWidget, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)),
      this, SLOT(setFilterVisibility(VSFilterViewSettings*, bool)));

    // Update filter check states to match the current view widget
    getController()->getFilterModel()->updateModelForView(viewWidget->getAllFilterViewSettings());

    VSAbstractFilterWidget* fw;
    foreach(fw, m_FilterToFilterWidgetMap.values())
    {
      fw->setDrawingEnabled(false);
      fw->setInteractor(getActiveViewWidget()->getVisualizationWidget()->GetInteractor());
    }
  }

  emit changedActiveView(viewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setFilterVisibility(VSFilterViewSettings* viewSettings, bool visible)
{
  if(nullptr == m_ActiveViewWidget)
  {
    return;
  }

  viewSettings->getFilter()->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::activeViewClosed()
{
  setActiveView(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setCurrentFilter(VSAbstractFilter* filter)
{
  m_CurrentFilter = filter;
  VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(filter);

  emit changedActiveFilter(m_CurrentFilter, filterWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::deleteFilter(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  QVector<VSAbstractViewWidget*> viewWidgets = getAllViewWidgets();
  for(VSAbstractViewWidget* widget : viewWidgets)
  {
    VSFilterViewSettings* viewSettings = widget->getFilterViewSettings(filter);
    viewSettings->setVisible(false);
    viewSettings->setScalarBarVisible(false);
  }

  QVector<VSAbstractFilter*> childFilters = filter->getChildren();
  for(VSAbstractFilter* child : childFilters)
  {
    deleteFilter(child);
  }

  if(m_CurrentFilter == filter)
  {
    setCurrentFilter(filter->getParentFilter());
  }

  m_Controller->getFilterModel()->removeFilter(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createClipFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent)
  {
    VSClipFilter* filter = new VSClipFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createCropFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent)
  {
    VSCropFilter* filter = new VSCropFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createSliceFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent)
  {
    VSSliceFilter* filter = new VSSliceFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createMaskFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent)
  {
    VSMaskFilter* filter = new VSMaskFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createThresholdFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent)
  {
    VSThresholdFilter* filter = new VSThresholdFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::finishAddingFilter(VSAbstractFilter* filter, VSAbstractFilter* parent)
{
  if(nullptr == filter)
  {
    return;
  }

  m_Controller->getFilterModel()->addFilter(filter);

  // Set parent filter to invisible for the active view
  if(getActiveViewWidget() && parent)
  {
    VSFilterViewSettings* parentSettings = getActiveViewWidget()->getFilterViewSettings(parent);
    if(parentSettings)
    {
      parentSettings->setVisible(false);
      renderAllViews();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::renderActiveView()
{
  if(getActiveViewWidget())
  {
    getActiveViewWidget()->renderView();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::renderAllViews()
{
  for(VSAbstractViewWidget* viewWidget : getAllViewWidgets())
  {
    viewWidget->renderView();
  }
}
