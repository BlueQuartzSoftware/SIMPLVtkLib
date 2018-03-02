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

#include "VSViewWidget.h"

#include <QtWidgets/QLayout>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractDataFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget::VSAbstractViewWidget(QWidget* parent, Qt::WindowFlags f)
  : QFrame(parent, f)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget::VSAbstractViewWidget(const VSAbstractViewWidget& other)
  : QFrame(nullptr)
{
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::copyFilters(VSFilterViewSettings::Map filters)
{
  clearFilters();

  for(auto iter = filters.begin(); iter != filters.end(); iter++)
  {
    VSAbstractFilter* filter = iter->first;
    VSFilterViewSettings* viewSettings = new VSFilterViewSettings(*(iter->second));
    m_FilterViewSettings[filter] = viewSettings;

    connect(viewSettings, SIGNAL(filterAdded(VSAbstractFilter*)), 
      this, SLOT(filterAdded(VSAbstractFilter*)));
    connect(viewSettings, SIGNAL(filterRemoved(VSAbstractFilter*)), 
      this, SLOT(filterRemoved(VSAbstractFilter*)));
    
    connect(viewSettings, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)), 
      this, SLOT(setFilterVisibility(VSFilterViewSettings*, bool)));
    connect(viewSettings, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)), 
      this, SLOT(setFilterArrayIndex(VSFilterViewSettings*, int)));
    connect(viewSettings, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)), 
      this, SLOT(setFilterComponentIndex(VSFilterViewSettings*, int)));
    connect(viewSettings, SIGNAL(mapColorsChanged(VSFilterViewSettings*, Qt::CheckState)), 
      this, SLOT(setFilterMapColors(VSFilterViewSettings*, Qt::CheckState)));
    connect(viewSettings, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)), 
      this, SLOT(setFilterShowScalarBar(VSFilterViewSettings*, bool)));
    connect(viewSettings, SIGNAL(requiresRender()), this, SLOT(renderView()));

    checkFilterViewSetting(viewSettings);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::clearFilters()
{
  for(auto iter = m_FilterViewSettings.begin(); iter != m_FilterViewSettings.end(); iter++)
  {
    VSFilterViewSettings* viewSettings = iter->second;
    viewSettings->setVisible(false);
    viewSettings->setScalarBarVisible(false);
    checkFilterViewSetting(viewSettings);
    
    viewSettings->deleteLater();
  }

  m_FilterViewSettings.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterAdded(VSAbstractFilter* filter, bool currentFilter)
{
  if(nullptr == filter)
  {
    return;
  }

  createFilterViewSettings(filter);

  if(dynamic_cast<VSAbstractDataFilter*>(filter))
  {
    getVisualizationWidget()->resetCamera();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterRemoved(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  VSFilterViewSettings* viewSettings = getFilterViewSettings(filter);
  if(viewSettings)
  {
    setFilterVisibility(viewSettings, false);
    viewSettings->deleteLater();
  }

  m_FilterViewSettings.erase(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::checkFilterViewSetting(VSFilterViewSettings* setting)
{
  if(nullptr == setting)
  {
    return;
  }

  if(setting->getScalarBarWidget())
  {
    setting->getScalarBarWidget()->SetInteractor(getVisualizationWidget()->GetInteractor());
  }

  setFilterVisibility(setting, setting->isVisible());
  setFilterShowScalarBar(setting, setting->isScalarBarVisible());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSAbstractViewWidget::getActiveFilterSettings() const
{
  return m_ActiveFilterSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setActiveFilterSettings(VSFilterViewSettings* settings)
{
  m_ActiveFilterSettings = settings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSAbstractViewWidget::getFilterViewSettings(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return nullptr;
  }

  VSFilterViewSettings* settings = m_FilterViewSettings[filter];
  if(nullptr == settings)
  {
    settings = createFilterViewSettings(filter);
  }

  return settings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Map VSAbstractViewWidget::getAllFilterViewSettings() const
{
  return m_FilterViewSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractViewWidget::getViewSettingsIndex(VSFilterViewSettings* settings)
{
  auto iter = m_FilterViewSettings.find(settings->getFilter());
  if(iter == m_FilterViewSettings.end())
  {
    return -1;
  }

  return std::distance(m_FilterViewSettings.begin(), iter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSAbstractViewWidget::createFilterViewSettings(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return nullptr;
  }

  // Do not overwrite filter view settings that already exist
  if(nullptr != m_FilterViewSettings[filter])
  {
    return m_FilterViewSettings[filter];
  }

  VSFilterViewSettings* viewSettings = new VSFilterViewSettings(filter);

  connect(viewSettings, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)),
    this, SLOT(setFilterVisibility(VSFilterViewSettings*, bool)));
  connect(viewSettings, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)),
    this, SLOT(setFilterArrayIndex(VSFilterViewSettings*, int)));
  connect(viewSettings, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)),
    this, SLOT(setFilterComponentIndex(VSFilterViewSettings*, int)));
  connect(viewSettings, SIGNAL(mapColorsChanged(VSFilterViewSettings*, Qt::CheckState)),
    this, SLOT(setFilterMapColors(VSFilterViewSettings*, Qt::CheckState)));
  connect(viewSettings, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)),
    this, SLOT(setFilterShowScalarBar(VSFilterViewSettings*, bool)));
  connect(viewSettings, SIGNAL(requiresRender()), this, SLOT(renderView()));

  m_FilterViewSettings[filter] = viewSettings;

  if(filter->getParentFilter() && filter->getParentFilter()->getOutput())
  {
    VSFilterViewSettings* parentSettings = getFilterViewSettings(filter->getParentFilter());
    if(parentSettings)
    {
      viewSettings->copySettings(parentSettings);
    }
  }

  checkFilterViewSetting(viewSettings);

  return viewSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterVisibility(VSFilterViewSettings* viewSettings, bool filterVisible)
{
  if(nullptr == viewSettings)
  {
    return;
  }
  if(false == viewSettings->isValid())
  {
    return;
  }

  if(nullptr == getVisualizationWidget())
  {
    return;
  }

  if(nullptr == getVisualizationWidget()->getRenderer())
  {
    return;
  }

  if(filterVisible)
  {
    getVisualizationWidget()->getRenderer()->AddViewProp(viewSettings->getActor());

    if(viewSettings->isScalarBarVisible())
    {
      viewSettings->getScalarBarWidget()->SetEnabled(1);
    }
  }
  else
  {
    getVisualizationWidget()->getRenderer()->RemoveViewProp(viewSettings->getActor());

    if(viewSettings->isScalarBarVisible())
    {
      viewSettings->getScalarBarWidget()->SetEnabled(0);
    }
  }

  emit visibilityChanged(viewSettings, filterVisible);
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterArrayIndex(VSFilterViewSettings* viewSettings, int index)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterComponentIndex(VSFilterViewSettings* viewSettings, int index)
{
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterMapColors(VSFilterViewSettings* viewSettings, int mapColorState)
{
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterShowScalarBar(VSFilterViewSettings* viewSettings, bool showScalarBar)
{
  if(nullptr == viewSettings)
  {
    return;
  }

  if(nullptr == viewSettings->getScalarBarWidget())
  {
    return;
  }

  if(viewSettings->getScalarBarWidget())
  {
    if(viewSettings->isVisible())
    {
      if(viewSettings->isScalarBarVisible())
      {
        viewSettings->getScalarBarWidget()->SetEnabled(1);
      }
      else
      {
        viewSettings->getScalarBarWidget()->SetEnabled(0);
      }
    }
    else
    {
      viewSettings->getScalarBarWidget()->SetEnabled(0);
    }
  }

  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterArrayIndex(int index)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setActiveArrayIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterComponentIndex(int index)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setActiveComponentIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterMapColors(Qt::CheckState mapColorState)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setMapColors(mapColorState);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterShowScalarBar(bool showScalarBar)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setScalarBarVisible(showScalarBar);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::mousePressed()
{
  emit markActive(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::mousePressEvent(QMouseEvent* event)
{
  mousePressed();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::splitVertically()
{
  splitWidget(Qt::Vertical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::splitHorizontally()
{
  splitWidget(Qt::Horizontal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSplitter* VSAbstractViewWidget::splitWidget(Qt::Orientation orientation)
{
  QSplitter* splitter = new QSplitter();
  splitter->setOrientation(orientation);

  const int currentWidth = width();
  const int currentHeight = height();

  // Replace this with the splitter in the parent layout if possible
  // Otherwise, set the splitter's parent to this widget's parent
  QWidget* parent = parentWidget();
  QSplitter* parentSplitter = dynamic_cast<QSplitter*>(parent);
  
  if(parent)
  {
    // If already part of a QSplitter
    if(parentSplitter)
    {
      int index = parentSplitter->indexOf(this);
      parentSplitter->replaceWidget(index, splitter);
    }
    // If not already part of a QSplitter
    else
    {
      if(parent->layout())
      {
        parent->layout()->replaceWidget(this, splitter);
      }
      else
      {
        splitter->setParent(parent);
      }
    }
  }

  VSAbstractViewWidget* cloneWidget = clone();
  cloneWidget->getVisualizationWidget()->render();

  // Fill the splitter with both the view widget and a clone of it
  splitter->addWidget(this);
  splitter->addWidget(cloneWidget);

  if(Qt::Horizontal == orientation)
  {
    int newWidth = currentWidth / 2;
    QList<int> newSizes;
    newSizes.append(newWidth);
    newSizes.append(newWidth);
    splitter->setSizes(newSizes);
  }
  else
  {
    int newHeight = currentHeight / 2;
    QList<int> newSizes;
    newSizes.append(newHeight);
    newSizes.append(newHeight);
    splitter->setSizes(newSizes);
  }

  // Set the size policy to match the current widget's
  splitter->setSizePolicy(this->sizePolicy());

  // Emit signal notifying the main widget of a new view widget
  emit viewWidgetCreated(cloneWidget);

  return splitter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::closeView()
{
  emit viewWidgetClosed();

  QSplitter* splitter = dynamic_cast<QSplitter*>(parentWidget());

  // If parent is a QSplitter...
  if(splitter)
  {
    int index = splitter->indexOf(this);
    QWidget* other = nullptr;

    if(1 == index)
    {
      other = splitter->widget(0);
    }
    else
    {
      other = splitter->widget(1);
    }

    QWidget* parent = splitter->parentWidget();
    QSplitter* parentSplitter = dynamic_cast<QSplitter*>(parent);
    // If another QSplitter...
    if(parentSplitter)
    {
      int parentIndex = parentSplitter->indexOf(splitter);
      parentSplitter->replaceWidget(parentIndex, other);
    }
    // If not another QSplitter..
    else
    {
      if(parent->layout())
      {
        parent->layout()->replaceWidget(splitter, other);
      }
      else
      {
        other->setParent(parent);
      }
    }

    splitter->deleteLater();
  }
  else
  {
    deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::renderView()
{
  VSVisualizationWidget* visualizationWidget = getVisualizationWidget();
  if(visualizationWidget)
  {
    visualizationWidget->render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::resetCamera()
{
  VSVisualizationWidget* visualizationWidget = getVisualizationWidget();
  if(visualizationWidget && visualizationWidget->getRenderer())
  {
    visualizationWidget->getRenderer()->ResetCamera();
    visualizationWidget->render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController* VSAbstractViewWidget::getController() const
{
  return m_Controller;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setActive(bool active)
{
  Q_UNUSED(active)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setController(VSController* controller)
{
  if(m_Controller)
  {
    disconnect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*)), this, SLOT(filterAdded(VSAbstractFilter*)));
    disconnect(m_Controller, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(filterRemoved(VSAbstractFilter*)));
  }

  m_Controller = controller;
  connect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*, bool)), this, SLOT(filterAdded(VSAbstractFilter*, bool)));
  connect(m_Controller, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(filterRemoved(VSAbstractFilter*)));
  
  // Clear old filter view settings and create new ones
  clearFilters();

  QVector<VSAbstractFilter*> filters = controller->getAllFilters();
  for(VSAbstractFilter* filter : filters)
  {
    VSFilterViewSettings* viewSettings = new VSFilterViewSettings(filter);
    m_FilterViewSettings[filter] = viewSettings;

    connect(viewSettings, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)), 
      this, SLOT(setFilterVisibility(VSFilterViewSettings*, bool)));
    connect(viewSettings, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)), 
      this, SLOT(setFilterArrayIndex(VSFilterViewSettings*, int)));
    connect(viewSettings, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)), 
      this, SLOT(setFilterComponentIndex(VSFilterViewSettings*, int)));
    connect(viewSettings, SIGNAL(mapColorsChanged(VSFilterViewSettings*, Qt::CheckState)),
      this, SLOT(setFilterMapColors(VSFilterViewSettings*, Qt::CheckState)));
    connect(viewSettings, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)), 
      this, SLOT(setFilterShowScalarBar(VSFilterViewSettings*, bool)));
    connect(viewSettings, SIGNAL(requiresRender()), this, SLOT(renderView()));

    // Check filter and scalar bar visibility
    checkFilterViewSetting(viewSettings);
  }
}
