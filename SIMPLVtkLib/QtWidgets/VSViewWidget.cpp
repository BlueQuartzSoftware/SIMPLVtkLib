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

#include <QtGui/QPainter>

#include "ui_VSViewWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VSViewWidget::VSInternals : public Ui::VSViewWidget
{
public:
  VSInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewWidget::VSViewWidget(QWidget* parent, Qt::WindowFlags f)
  : VSAbstractViewWidget(parent, f)
  , m_Internals(new VSInternals())
{
  m_Internals->setupUi(this);

  connectSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewWidget::VSViewWidget(const VSViewWidget& other)
  : VSAbstractViewWidget(nullptr)
  , m_Internals(new VSInternals())
{
  m_Internals->setupUi(this);

  setController(other.getController());
  getVisualizationWidget()->copy(other.getVisualizationWidget());
  copyFilters(other.getAllFilterViewSettings());

  connectSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::connectSlots()
{
  connect(m_Internals->splitHorizontalBtn, SIGNAL(clicked()), this, SLOT(splitHorizontally()));
  connect(m_Internals->splitVerticalBtn, SIGNAL(clicked()), this, SLOT(splitVertically()));
  connect(m_Internals->closeBtn, SIGNAL(clicked()), this, SLOT(closeView()));

  // Clicking any button should set the active VSViewController
  connect(m_Internals->splitHorizontalBtn, SIGNAL(clicked()), this, SLOT(mousePressed()));
  connect(m_Internals->splitVerticalBtn, SIGNAL(clicked()), this, SLOT(mousePressed()));
  connect(getVisualizationWidget(), SIGNAL(mousePressed()), this, SLOT(mousePressed()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget* VSViewWidget::clone()
{
  VSViewWidget* viewWidget = new VSViewWidget(*(this));
  return viewWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisualizationWidget* VSViewWidget::getVisualizationWidget() const
{
  if(nullptr == m_Internals)
  {
    return nullptr;
  }

  return m_Internals->visualizationWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::setActive(bool active)
{
  (active) ? toActiveState() : toInactiveState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::toActiveState()
{
  QString styleString;
  QTextStream ss(&styleString);

  ss << "VSViewWidget {";

  ss << "border: 1px solid #0500ff;";
  ss << "padding: 1px;";

  ss << "}";

  setStyleSheet(styleString);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::toInactiveState()
{
  setStyleSheet("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::setFilterShowScalarBar(VSFilterViewSettings* viewSettings, bool showScalarBar)
{
  if(false == (viewSettings && viewSettings->getScalarBarWidget()))
  {
    return;
  }

  VTK_PTR(vtkScalarBarWidget) scalarBarWidget = viewSettings->getScalarBarWidget();
  scalarBarWidget->SetEnabled(showScalarBar);

  renderView();
}
