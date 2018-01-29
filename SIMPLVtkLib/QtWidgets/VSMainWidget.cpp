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

  VSViewController* viewController = new VSViewController(getController());
  m_Internals->viewWidget->setViewController(viewController);
  getController()->setActiveViewController(viewController);

  connectSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget::connectSlots()
{
  connect(m_Internals->cameraXpBtn, SIGNAL(clicked()), this, SLOT(activeCameraXPlus()));
  connect(m_Internals->cameraYpBtn, SIGNAL(clicked()), this, SLOT(activeCameraYPlus()));
  connect(m_Internals->cameraZpBtn, SIGNAL(clicked()), this, SLOT(activeCameraZPlus()));
  connect(m_Internals->cameraXmBtn, SIGNAL(clicked()), this, SLOT(activeCameraXMinus()));
  connect(m_Internals->cameraYmBtn, SIGNAL(clicked()), this, SLOT(activeCameraYMinus()));
  connect(m_Internals->cameraZmBtn, SIGNAL(clicked()), this, SLOT(activeCameraZMinus()));
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
