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
  connect(m_Controller, SIGNAL(activeViewChanged(VSViewController*)), 
    this, SLOT(activeViewChanged(VSViewController*)));

  connect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*)), this, SLOT(filterAdded(VSAbstractFilter*)));

  connect(m_Controller, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(filterRemoved(VSAbstractFilter*)));
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
VSAbstractViewWidget* VSMainWidgetBase::getViewWidget(VSViewController* viewController)
{
  if(viewController)
  {
    QVector<VSAbstractViewWidget*> viewWidgets = getAllViewWidgets();
    for(auto iter = viewWidgets.begin(); iter != viewWidgets.end(); iter++)
    {
      if((*iter)->getViewController() == viewController)
      {
        return (*iter);
      }
    }
  }

  return nullptr;
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
    disconnect(this, SIGNAL(changedActiveView(VSViewController*)), view, SLOT(changeViewController(VSViewController*)));
  }

  view->setController(m_Controller);

  m_FilterView = view;
  connect(view, SIGNAL(filterClicked(VSAbstractFilter*)), this, SLOT(changeCurrentFilter(VSAbstractFilter*)));
  connect(this, SIGNAL(changedActiveView(VSViewController*)), view, SLOT(changeViewController(VSViewController*)));
  
  view->changeViewController(m_ActiveViewWidget->getViewController());
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
    disconnect(this, SIGNAL(changedActiveView(VSViewController*)),
      m_InfoWidget, SLOT(setViewController(VSViewController*)));
  }

  m_InfoWidget = infoWidget;

  if(m_InfoWidget)
  {    
    connect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)),
      infoWidget, SLOT(setFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));
    connect(this, SIGNAL(changedActiveView(VSViewController*)),
      infoWidget, SLOT(setViewController(VSViewController*)));

    VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(m_CurrentFilter);
    m_InfoWidget->setFilter(m_CurrentFilter, filterWidget);
    m_InfoWidget->setViewController(m_Controller->getActiveViewController());
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
  VSAbstractFilterWidget* fw = nullptr;
  if (dynamic_cast<VSClipFilter*>(filter) != nullptr)
  {
    VSClipFilter* vsFilter = dynamic_cast<VSClipFilter*>(filter);
    fw = new VSClipFilterWidget(vsFilter, getActiveViewWidget()->getVisualizationWidget()->GetInteractor(), this);
  }
  else if (dynamic_cast<VSCropFilter*>(filter) != nullptr)
  {
    VSCropFilter* vsFilter = dynamic_cast<VSCropFilter*>(filter);
    fw = new VSCropFilterWidget(vsFilter, getActiveViewWidget()->getVisualizationWidget()->GetInteractor(), this);
  }
  else if (dynamic_cast<VSMaskFilter*>(filter) != nullptr)
  {
    VSMaskFilter* vsFilter = dynamic_cast<VSMaskFilter*>(filter);
    fw = new VSMaskFilterWidget(vsFilter, getActiveViewWidget()->getVisualizationWidget()->GetInteractor(), this);
  }
  else if (dynamic_cast<VSDataSetFilter*>(filter) != nullptr)
  {
    VSDataSetFilter* vsFilter = dynamic_cast<VSDataSetFilter*>(filter);
    fw = new VSDataSetFilterWidget(vsFilter, this);
  }
  else if (dynamic_cast<VSSliceFilter*>(filter) != nullptr)
  {
    VSSliceFilter* vsFilter = dynamic_cast<VSSliceFilter*>(filter);
    fw = new VSSliceFilterWidget(vsFilter, getActiveViewWidget()->getVisualizationWidget()->GetInteractor(), this);
  }
  else if (dynamic_cast<VSThresholdFilter*>(filter) != nullptr)
  {
    VSThresholdFilter* vsFilter = dynamic_cast<VSThresholdFilter*>(filter);
    fw = new VSThresholdFilterWidget(vsFilter, getActiveViewWidget()->getVisualizationWidget()->GetInteractor(), this);
  }

  if (fw != nullptr)
  {
    m_FilterToFilterWidgetMap.insert(filter, fw);
  }

  changeCurrentFilter(filter);
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
void VSMainWidgetBase::activeViewChanged(VSViewController* controller)
{
  disconnect(m_ActiveViewWidget, SIGNAL(viewWidgetClosed()), this, SLOT(activeViewClosed()));
  m_ActiveViewWidget = getViewWidget(controller);
  if(m_ActiveViewWidget)
  {
    connect(m_ActiveViewWidget, SIGNAL(viewWidgetClosed()), this, SLOT(activeViewClosed()));
  }

  emit changedActiveView(controller);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::activeViewClosed()
{
  m_Controller->setActiveViewController(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::changeCurrentFilter(VSAbstractFilter* filter)
{
  m_CurrentFilter = filter;
  VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(filter);

  emit changedActiveFilter(m_CurrentFilter, filterWidget);
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
    addFilter(filter, parent);
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
    addFilter(filter, parent);
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
    addFilter(filter, parent);
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
    addFilter(filter, parent);
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
    addFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::addFilter(VSAbstractFilter* filter, VSAbstractFilter* parent)
{
  if(nullptr == filter)
  {
    return;
  }

  m_Controller->getFilterModel()->addFilter(filter);

  // Set parent filter to invisible for the active view
  if(getActiveViewWidget() && parent)
  {
    VSViewController* viewController = getActiveViewWidget()->getViewController();
    if(viewController)
    {
      VSFilterViewSettings* parentSettings = viewController->getViewSettings(parent);
      if(parentSettings)
      {
        parentSettings->setVisible(false);
        renderAllViews();
      }
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
