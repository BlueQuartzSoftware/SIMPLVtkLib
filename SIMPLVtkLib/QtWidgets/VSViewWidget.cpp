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
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

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
VSViewWidget::VSViewWidget(QWidget* parent, Qt::WindowFlags windowFlags)
: VSAbstractViewWidget(parent, windowFlags)
, m_Internals(new VSInternals())
, m_InteractorStyle(VSInteractorStyleFilterCamera::New())
{
  m_Internals->setupUi(this);
  m_Internals->visualizationWidget->setInteractorStyle(m_InteractorStyle);
  m_InteractorStyle->setViewWidget(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewWidget::VSViewWidget(const VSViewWidget& other)
: VSAbstractViewWidget(other)
, m_Internals(new VSInternals())
, m_InteractorStyle(VSInteractorStyleFilterCamera::New())
{
  m_Internals->setupUi(this);
  m_Internals->visualizationWidget->setInteractorStyle(m_InteractorStyle);
  m_InteractorStyle->setViewWidget(this);

  copyFilters(*other.getFilterViewModel());
  getVisualizationWidget()->copy(other.getVisualizationWidget());
  copySelection(other);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::setupGui()
{
  connectSlots();
  updateClosable();

  // Disable the visualization widget's context menu
  getVisualizationWidget()->useOwnContextMenu(false);

  QString styleString;
  QTextStream ss(&styleString);

  ss << "VSViewWidget[Active=\"true\"] {";
  ss << "border: 1px solid #0500ff;";
  ss << "padding: 1px;";
  ss << "}";

  setStyleSheet(styleString);
  getVisualizationWidget()->render();
  m_InteractorStyle->SetInteractionModeToImage3D();
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

  // Control the visualization widget's context menu
  connect(getVisualizationWidget(), &VSVisualizationWidget::customContextMenuRequested, this, &VSViewWidget::showVisualizationContextMenu);

  // Set up the interactive mode button
  connect(m_Internals->interactiveModeBtn, &QPushButton::clicked, [=]()
  {
    if(m_Internals->interactiveModeBtn->text() == "3D")
    {
      m_Internals->interactiveModeBtn->setText("2D");
      m_InteractorStyle->SetInteractionModeToImage2D();
    }
    else
    {
      m_Internals->interactiveModeBtn->setText("3D");
      m_InteractorStyle->SetInteractionModeToImage3D();
    }
  });
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
QMenu* VSViewWidget::getContextMenu(VSAbstractFilter* filter)
{
  QMenu* menu = new QMenu("Visualization", this);

  VSFilterViewSettings* settings = getFilterViewSettings(filter);
  if(settings)
  {
    QString filterName = settings->getFilter()->getFilterName();

    QAction* selectFilterAction = menu->addAction("Select '" + filterName + "'");
    connect(selectFilterAction, &QAction::triggered, [=] { selectFilter(settings->getFilter()); });

    menu->addSeparator();

    // Visibility Settings
    QAction* hideFilterAction = menu->addAction("Hide '" + filterName + "'");
    connect(hideFilterAction, &QAction::triggered, [=] { settings->setVisible(false); });

    QAction* showOnlyFilterAction = menu->addAction("Show only '" + filterName + "'");
    connect(showOnlyFilterAction, &QAction::triggered, [=] { showOnlyFilter(settings); });

    QAction* showAllFiltersAction = menu->addAction("Show all");
    connect(showAllFiltersAction, &QAction::triggered, this, &VSViewWidget::showAllFilters);

    menu->addSeparator();

    // Scalar Bar Menu
    {
      QMenu* scalarBarMenu = menu->addMenu("Scalar Bars");

      QAction* hideAllScalarBarsAction = scalarBarMenu->addAction("Hide all");
      connect(hideAllScalarBarsAction, &QAction::triggered, this, &VSViewWidget::hideAllScalarBars);

      QAction* showOnlyScalarBarsAction = scalarBarMenu->addAction("Show only '" + filterName + "'");
      connect(showOnlyScalarBarsAction, &QAction::triggered, [=] { showOnlyScalarBar(settings); });

      scalarBarMenu->addAction(settings->getToggleScalarBarAction());

      menu->addMenu(settings->getMapScalarsMenu());
    }

    menu->addSeparator();

    // Rendering Settings from VSFilterViewSettings
    {
      // Set Color and Alpha
      menu->addAction(settings->getSetColorAction());
      menu->addAction(settings->getSetOpacityAction());

      menu->addSeparator();

      // Representation menu
      menu->addMenu(settings->getRepresentationMenu());

      // Color By Array menu
      menu->addMenu(settings->getColorByMenu());

      // Color by Component menu
      if(settings->hasMulipleComponents())
      {
        menu->addMenu(settings->getArrayComponentMenu());
      }
    }

    menu->addSeparator();

    QAction* deleteFilterAction = menu->addAction("Delete '" + filterName + "'");
    connect(deleteFilterAction, &QAction::triggered, [=] { settings->getFilter()->deleteFilter(); });

    menu->addSeparator();
  }

  menu->addAction(getVisualizationWidget()->getLinkCamerasAction());

  return menu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::showVisualizationContextMenu(const QPoint& point)
{
  QMenu menu("Visualization", this);

  VSFilterViewSettings* settings = getFilterViewSettingsAtMousePos(point);
  if(settings)
  {
    QString filterName = settings->getFilter()->getFilterName();

    QAction* selectFilterAction = menu.addAction("Select '" + filterName + "'");
    connect(selectFilterAction, &QAction::triggered, [=] { selectFilter(settings->getFilter()); });

    menu.addSeparator();

    // Visibility Settings
    QAction* hideFilterAction = menu.addAction("Hide '" + filterName + "'");
    connect(hideFilterAction, &QAction::triggered, [=] { settings->setVisible(false); });

    QAction* showOnlyFilterAction = menu.addAction("Show only '" + filterName + "'");
    connect(showOnlyFilterAction, &QAction::triggered, [=] { showOnlyFilter(settings); });

    QAction* showAllFiltersAction = menu.addAction("Show all");
    connect(showAllFiltersAction, &QAction::triggered, this, &VSViewWidget::showAllFilters);

    menu.addSeparator();

    // Scalar Bar Menu
    {
      QMenu* scalarBarMenu = menu.addMenu("Scalar Bars");

      QAction* hideAllScalarBarsAction = scalarBarMenu->addAction("Hide all");
      connect(hideAllScalarBarsAction, &QAction::triggered, this, &VSViewWidget::hideAllScalarBars);

      QAction* showOnlyScalarBarsAction = scalarBarMenu->addAction("Show only '" + filterName + "'");
      connect(showOnlyScalarBarsAction, &QAction::triggered, [=] { showOnlyScalarBar(settings); });

      scalarBarMenu->addAction(settings->getToggleScalarBarAction());

      menu.addMenu(settings->getMapScalarsMenu());
    }

    menu.addSeparator();

    // Rendering Settings from VSFilterViewSettings
    {
      // Set Color and Alpha
      menu.addAction(settings->getSetColorAction());
      menu.addAction(settings->getSetOpacityAction());

      menu.addSeparator();

      // Representation menu
      menu.addMenu(settings->getRepresentationMenu());

      // Color By Array menu
      menu.addMenu(settings->getColorByMenu());

      // Color by Component menu
      if(settings->hasMulipleComponents())
      {
        menu.addMenu(settings->getArrayComponentMenu());
      }
    }

    menu.addSeparator();

    QAction* deleteFilterAction = menu.addAction("Delete '" + filterName + "'");
    connect(deleteFilterAction, &QAction::triggered, [=] { settings->getFilter()->deleteFilter(); });

    menu.addSeparator();
  }

  menu.addAction(getVisualizationWidget()->getLinkCamerasAction());

  menu.exec(getVisualizationWidget()->mapToGlobal(point));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::setFilterShowScalarBar(const bool& showScalarBar)
{
  VSFilterViewSettings* viewSettings = dynamic_cast<VSFilterViewSettings*>(sender());

  if(false == (viewSettings && viewSettings->getScalarBarWidget()))
  {
    return;
  }

  VTK_PTR(vtkScalarBarWidget) scalarBarWidget = viewSettings->getScalarBarWidget();
  scalarBarWidget->SetEnabled(showScalarBar);

  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::updateClosable()
{
  m_Internals->closeBtn->setEnabled(isClosable());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSViewWidget::getFilterAtMousePos(const QPoint& point)
{
  if(nullptr == getVisualizationWidget())
  {
    return nullptr;
  }

  int pos[2];
  pos[0] = point.x();
  pos[1] = getVisualizationWidget()->height() - point.y();

  return getVisualizationWidget()->getFilterFromScreenCoords(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSViewWidget::getFilterViewSettingsAtMousePos(const QPoint& point)
{
  return getFilterViewSettings(getFilterAtMousePos(point));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::showOnlyFilter(VSFilterViewSettings* settings)
{
  VSFilterViewSettings::Map settingsMap = getAllFilterViewSettings();
  for(auto iter = settingsMap.begin(); iter != settingsMap.end(); iter++)
  {
    VSFilterViewSettings* iterSettings = (*iter).second;
    (*iter).second->setVisible(iterSettings == settings);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::showAllFilters()
{
  VSFilterViewSettings::Map settingsMap = getAllFilterViewSettings();
  for(auto iter = settingsMap.begin(); iter != settingsMap.end(); iter++)
  {
    (*iter).second->setScalarBarSetting(VSFilterViewSettings::ScalarBarSetting::Always);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::showOnlyScalarBar(VSFilterViewSettings* settings)
{
  VSFilterViewSettings::Map settingsMap = getAllFilterViewSettings();
  for(auto iter = settingsMap.begin(); iter != settingsMap.end(); iter++)
  {
    VSFilterViewSettings* iterSettings = (*iter).second;
    VSFilterViewSettings::ScalarBarSetting scalarBarSetting;
    if(iterSettings == settings)
    {
      scalarBarSetting = VSFilterViewSettings::ScalarBarSetting::Always;
    }
    else
    {
      scalarBarSetting = VSFilterViewSettings::ScalarBarSetting::Never;
    }

    (*iter).second->setScalarBarSetting(scalarBarSetting);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::hideAllScalarBars()
{
  VSFilterViewSettings::Map settingsMap = getAllFilterViewSettings();
  for(auto iter = settingsMap.begin(); iter != settingsMap.end(); iter++)
  {
    (*iter).second->setScalarBarSetting(VSFilterViewSettings::ScalarBarSetting::Never);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::updateTransformText(QString transformText)
{
  m_Internals->transformTextLabel->setText(transformText);
}
