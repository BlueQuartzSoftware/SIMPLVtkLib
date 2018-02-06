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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget::VSAbstractViewWidget(QWidget* parent)
  : QWidget(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewController* VSAbstractViewWidget::getViewController()
{
  return m_ViewController;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setViewController(VSViewController* controller)
{
  if(m_ViewController)
  {
    disconnect(m_ViewController, SIGNAL(filterAdded(VSAbstractFilter*)), this, SLOT(filterAdded(VSAbstractFilter*)));

    disconnect(m_ViewController, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)), this, SLOT(filterVisibilityChanged(VSFilterViewSettings*, bool)));
    disconnect(m_ViewController, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)), this, SLOT(filterArrayIndexChanged(VSFilterViewSettings*, int)));
    disconnect(m_ViewController, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)), this, SLOT(filterComponentIndexChanged(VSFilterViewSettings*, int)));
    disconnect(m_ViewController, SIGNAL(mapColorsChanged(VSFilterViewSettings*, bool)), this, SLOT(filterMapColorsChanged(VSFilterViewSettings*, bool)));
    disconnect(m_ViewController, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)), this, SLOT(filterShowScalarBarChanged(VSFilterViewSettings*, bool)));
    disconnect(m_ViewController, SIGNAL(requiresRender()), this, SLOT(renderView()));
  }

  m_ViewController = controller;

  connect(controller, SIGNAL(filterAdded(VSAbstractFilter*)), this, SLOT(filterAdded(VSAbstractFilter*)));

  connect(m_ViewController, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)), this, SLOT(filterVisibilityChanged(VSFilterViewSettings*, bool)));
  connect(m_ViewController, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)), this, SLOT(filterArrayIndexChanged(VSFilterViewSettings*, int)));
  connect(m_ViewController, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)), this, SLOT(filterComponentIndexChanged(VSFilterViewSettings*, int)));
  connect(m_ViewController, SIGNAL(mapColorsChanged(VSFilterViewSettings*, bool)), this, SLOT(filterMapColorsChanged(VSFilterViewSettings*, bool)));
  connect(m_ViewController, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)), this, SLOT(filterShowScalarBarChanged(VSFilterViewSettings*, bool)));
  connect(m_ViewController, SIGNAL(requiresRender()), this, SLOT(renderView()));

  // check filter and scalar bar visibility
  std::vector<VSFilterViewSettings*> viewSettings = controller->getAllViewSettings();
  for(auto iter = viewSettings.begin(); iter != viewSettings.end(); iter++)
  {
    checkFilterViewSetting(*iter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterAdded(VSAbstractFilter* filter)
{
  VSFilterViewSettings* filterViewSettings = m_ViewController->getViewSettings(filter);

  if(nullptr == filterViewSettings)
  {
    return;
  }

  checkFilterViewSetting(filterViewSettings);
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
  setting->getScalarBarWidget()->SetInteractor(getVisualizationWidget()->GetInteractor());

  filterVisibilityChanged(setting, setting->getVisible());
  filterShowScalarBarChanged(setting, setting->isScalarBarVisible());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSAbstractViewWidget::getActiveFilterSettings()
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
void VSAbstractViewWidget::filterVisibilityChanged(VSFilterViewSettings* viewSettings, bool filterVisible)
{
  if(nullptr == viewSettings)
  {
    return;
  }

  if(nullptr == getVisualizationWidget())
  {
    return;
  }

  if(filterVisible)
  {
    getVisualizationWidget()->getRenderer()->AddActor(viewSettings->getActor());

    if(viewSettings->isScalarBarVisible())
    {
      viewSettings->getScalarBarWidget()->SetEnabled(1);
    }
  }
  else
  {
    getVisualizationWidget()->getRenderer()->RemoveActor(viewSettings->getActor());

    if(viewSettings->isScalarBarVisible())
    {
      viewSettings->getScalarBarWidget()->SetEnabled(0);
    }
  }

  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterArrayIndexChanged(VSFilterViewSettings* viewSettings, int index)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterComponentIndexChanged(VSFilterViewSettings* viewSettings, int index)
{
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterMapColorsChanged(VSFilterViewSettings* viewSettings, bool mapColors)
{
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterShowScalarBarChanged(VSFilterViewSettings* viewSettings, bool showScalarBar)
{
  if(viewSettings->getVisible())
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

  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterVisibility(VSFilterViewSettings* viewSettings, bool filterVisible)
{
  viewSettings->setVisible(filterVisible);
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
void VSAbstractViewWidget::changeFilterMapColors(bool mapColors)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setMapColors(mapColors);
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
  if(m_ViewController)
  {
    m_ViewController->markActive();
  }
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
