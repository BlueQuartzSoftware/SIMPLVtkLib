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

#include <QtConcurrent>

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QThread>
#include <QtCore/QUuid>
#include <QtGui/QKeySequence>
#include <QtWidgets/QShortcut>
#include <QInputDialog>

#include <QtWidgets/QMessageBox>

#include <QVTKInteractor.h>

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/Dialogs/LoadHDF5FileDialog.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSPipelineFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"
#include "SIMPLVtkLib/Wizards/ExecutePipeline/ExecutePipelineConstants.h"
#include "SIMPLVtkLib/Wizards/PerformMontage/PerformMontageConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMainWidgetBase::VSMainWidgetBase(QWidget* parent)
: QWidget(parent)
, m_Controller(new VSController())
{
  connectSlots();
  setupShortcuts();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::connectSlots()
{
  // Signals from VSController should be on the main thread, so Qt5 connections should be safe
  connect(m_Controller, &VSController::filterAdded, this, &VSMainWidgetBase::filterAdded);
  connect(m_Controller, &VSController::filterRemoved, this, &VSMainWidgetBase::filterRemoved);
  connect(m_Controller, &VSController::blockRender, this, &VSMainWidgetBase::setBlockRender);
  connect(m_Controller, &VSController::importDataQueueStarted, this, &VSMainWidgetBase::importDataQueueStarted);
  connect(m_Controller, &VSController::importDataQueueFinished, this, &VSMainWidgetBase::importDataQueueFinished);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setupShortcuts()
{
  QShortcut* filterUpShortcut = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Up), this);
  QShortcut* filterDownShortcut = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Down), this);
  QShortcut* filterLeftShortcut = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Left), this);
  QShortcut* filterRightShortcut = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Right), this);
  filterUpShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  filterDownShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  filterLeftShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  filterRightShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

  connect(filterUpShortcut, &QShortcut::activated, [=] { changeFilterSelected(VSAbstractViewWidget::FilterStepChange::Parent); });
  connect(filterDownShortcut, &QShortcut::activated, [=] { changeFilterSelected(VSAbstractViewWidget::FilterStepChange::Child); });
  connect(filterLeftShortcut, &QShortcut::activated, [=] { changeFilterSelected(VSAbstractViewWidget::FilterStepChange::PrevSibling); });
  connect(filterRightShortcut, &QShortcut::activated, [=] { changeFilterSelected(VSAbstractViewWidget::FilterStepChange::NextSibling); });

  QShortcut* addFilterUpShortcut = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Up), this);
  QShortcut* addFilterDownShortcut = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Down), this);
  QShortcut* addFilterLeftShortcut = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Left), this);
  QShortcut* addFilterRightShortcut = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Right), this);
  addFilterUpShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  addFilterDownShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  addFilterLeftShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  addFilterRightShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

  connect(addFilterUpShortcut, &QShortcut::activated, [=] { changeFilterSelected(VSAbstractViewWidget::FilterStepChange::Parent, true); });
  connect(addFilterDownShortcut, &QShortcut::activated, [=] { changeFilterSelected(VSAbstractViewWidget::FilterStepChange::Child, true); });
  connect(addFilterLeftShortcut, &QShortcut::activated, [=] { changeFilterSelected(VSAbstractViewWidget::FilterStepChange::PrevSibling, true); });
  connect(addFilterRightShortcut, &QShortcut::activated, [=] { changeFilterSelected(VSAbstractViewWidget::FilterStepChange::NextSibling, true); });

  QShortcut* applyFilterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
  QShortcut* resetFilterShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  QShortcut* deleteFilterShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
  applyFilterShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  resetFilterShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  deleteFilterShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

  connect(applyFilterShortcut, &QShortcut::activated, [=] { applyCurrentFilter(); });
  connect(resetFilterShortcut, &QShortcut::activated, [=] { resetCurrentFilter(); });
  connect(deleteFilterShortcut, &QShortcut::activated, [=] { deleteCurrentFilter(); });

  QShortcut* toggleVisibleShortcut = new QShortcut(QKeySequence(Qt::Key_Tab), this);
  toggleVisibleShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
  connect(toggleVisibleShortcut, &QShortcut::activated, [=] { toggleCurrentFilterVisibility(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::connectViewWidget(VSAbstractViewWidget* viewWidget)
{
  connect(viewWidget, SIGNAL(viewWidgetCreated(VSAbstractViewWidget*)), this, SLOT(connectViewWidget(VSAbstractViewWidget*)));

  connect(viewWidget, SIGNAL(markActive(VSAbstractViewWidget*)), this, SLOT(setActiveView(VSAbstractViewWidget*)));
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
    disconnect(m_FilterView, &VSFilterView::deleteFilterRequested, this, &VSMainWidgetBase::deleteFilter);
    disconnect(m_FilterView, &VSFilterView::reloadFilterRequested, this, &VSMainWidgetBase::reloadDataFilter);
    disconnect(m_FilterView, &VSFilterView::reloadFileFilterRequested, this, &VSMainWidgetBase::reloadFileFilter);
	disconnect(view, &VSFilterView::renameFilterRequested, this, &VSMainWidgetBase::renameDataFilter);
    disconnect(m_FilterView, &VSFilterView::filterClicked, this, &VSMainWidgetBase::setCurrentFilter);
    disconnect(this, &VSMainWidgetBase::changedActiveView, m_FilterView, &VSFilterView::setViewWidget);
  }

  m_FilterView = view;
  connect(view, &VSFilterView::deleteFilterRequested, this, &VSMainWidgetBase::deleteFilter);
  connect(view, &VSFilterView::reloadFilterRequested, this, &VSMainWidgetBase::reloadDataFilter);
  connect(view, &VSFilterView::reloadFileFilterRequested, this, &VSMainWidgetBase::reloadFileFilter);
  connect(view, &VSFilterView::renameFilterRequested, this, &VSMainWidgetBase::renameDataFilter);
  connect(view, &VSFilterView::filterClicked, this, &VSMainWidgetBase::setCurrentFilter);
  connect(this, &VSMainWidgetBase::changedActiveView, view, &VSFilterView::setViewWidget);

  view->setViewWidget(m_ActiveViewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterSettingsWidget* VSMainWidgetBase::getFilterSettingsWidget() const
{
  return m_FilterSettingsWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisibilitySettingsWidget* VSMainWidgetBase::getVisibilitySettingsWidget() const
{
  return m_VisibilitySettingsWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSColorMappingWidget* VSMainWidgetBase::getColorMappingWidget() const
{
  return m_ColorMappingWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAdvancedVisibilitySettingsWidget* VSMainWidgetBase::getAdvancedVisibilitySettingsWidget() const
{
  return m_AdvancedVisibilityWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransformWidget* VSMainWidgetBase::getTransformWidget() const
{
  return m_TransformWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setFilterSettingsWidget(VSFilterSettingsWidget* widget)
{
  if(m_FilterSettingsWidget)
  {
    disconnect(this, &VSMainWidgetBase::selectedFiltersChanged, m_FilterSettingsWidget, &VSFilterSettingsWidget::setFilters);
    disconnect(m_FilterSettingsWidget, &VSFilterSettingsWidget::filterDeleted, this, &VSMainWidgetBase::deleteFilter);
  }

  m_FilterSettingsWidget = widget;

  if(m_FilterSettingsWidget)
  {
    connect(this, &VSMainWidgetBase::selectedFiltersChanged, m_FilterSettingsWidget, &VSFilterSettingsWidget::setFilters);
    connect(m_FilterSettingsWidget, &VSFilterSettingsWidget::filterDeleted, this, &VSMainWidgetBase::deleteFilter);

    m_FilterSettingsWidget->setFilters(getCurrentSelection());
    m_FilterSettingsWidget->setViewWidget(getActiveViewWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setVisibilitySettingsWidget(VSVisibilitySettingsWidget* widget)
{
  if(m_VisibilitySettingsWidget)
  {
    disconnect(this, &VSMainWidgetBase::selectedFiltersChanged, m_VisibilitySettingsWidget, &VSVisibilitySettingsWidget::setFilters);
    disconnect(this, &VSMainWidgetBase::changedActiveView, m_VisibilitySettingsWidget, &VSVisibilitySettingsWidget::setViewWidget);
  }

  m_VisibilitySettingsWidget = widget;

  if(m_VisibilitySettingsWidget)
  {
    connect(this, &VSMainWidgetBase::selectedFiltersChanged, m_VisibilitySettingsWidget, &VSVisibilitySettingsWidget::setFilters);
    connect(this, &VSMainWidgetBase::changedActiveView, m_VisibilitySettingsWidget, &VSVisibilitySettingsWidget::setViewWidget);

    m_VisibilitySettingsWidget->setFilters(getCurrentSelection());
    m_VisibilitySettingsWidget->setViewWidget(getActiveViewWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setColorMappingWidget(VSColorMappingWidget* widget)
{
  if(m_ColorMappingWidget)
  {
    disconnect(this, &VSMainWidgetBase::selectedFiltersChanged, m_ColorMappingWidget, &VSColorMappingWidget::setFilters);
    disconnect(this, &VSMainWidgetBase::changedActiveView, m_ColorMappingWidget, &VSColorMappingWidget::setViewWidget);
  }

  m_ColorMappingWidget = widget;

  if(m_ColorMappingWidget)
  {
    connect(this, &VSMainWidgetBase::selectedFiltersChanged, m_ColorMappingWidget, &VSColorMappingWidget::setFilters);
    connect(this, &VSMainWidgetBase::changedActiveView, m_ColorMappingWidget, &VSColorMappingWidget::setViewWidget);

    m_ColorMappingWidget->setFilters(getCurrentSelection());
    m_ColorMappingWidget->setViewWidget(getActiveViewWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setAdvancedVisibilityWidget(VSAdvancedVisibilitySettingsWidget* widget)
{
  if(m_AdvancedVisibilityWidget)
  {
    disconnect(this, &VSMainWidgetBase::selectedFiltersChanged, m_AdvancedVisibilityWidget, &VSAdvancedVisibilitySettingsWidget::setFilters);
    disconnect(this, &VSMainWidgetBase::changedActiveView, m_AdvancedVisibilityWidget, &VSAdvancedVisibilitySettingsWidget::setViewWidget);
  }

  m_AdvancedVisibilityWidget = widget;

  if(m_ColorMappingWidget)
  {
    connect(this, &VSMainWidgetBase::selectedFiltersChanged, m_AdvancedVisibilityWidget, &VSAdvancedVisibilitySettingsWidget::setFilters);
    connect(this, &VSMainWidgetBase::changedActiveView, m_AdvancedVisibilityWidget, &VSAdvancedVisibilitySettingsWidget::setViewWidget);

    m_AdvancedVisibilityWidget->setFilters(getCurrentSelection());
    m_AdvancedVisibilityWidget->setViewWidget(getActiveViewWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setTransformWidget(VSTransformWidget* widget)
{
  if(m_TransformWidget)
  {
    disconnect(this, &VSMainWidgetBase::selectedFiltersChanged, m_TransformWidget, &VSTransformWidget::setFilters);
  }

  m_TransformWidget = widget;

  if(m_TransformWidget)
  {
    connect(this, &VSMainWidgetBase::selectedFiltersChanged, m_TransformWidget, &VSTransformWidget::setFilters);

    if(getCurrentFilter())
    {
      m_TransformWidget->setTransform(getCurrentFilter()->getTransform());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setInfoWidget(VSInfoWidget* infoWidget)
{
  if(infoWidget)
  {
    setFilterSettingsWidget(infoWidget->getFilterSettingsWidget());
    setVisibilitySettingsWidget(infoWidget->getVisibilitySettingsWidget());
    setColorMappingWidget(infoWidget->getColorMappingWidget());
    setTransformWidget(infoWidget->getTransformWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSMainWidgetBase::getCurrentFilter() const
{
  return m_CurrentFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSMainWidgetBase::getCurrentSelection() const
{
  if(m_ActiveViewWidget)
  {
    return m_ActiveViewWidget->getSelectedFilters();
  }

  return VSAbstractFilter::FilterListType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::selectFilter(VSAbstractFilter* filter)
{
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->selectFilter(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::selectFilters(VSAbstractFilter::FilterListType filters)
{
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->selectFilters(filters);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::filterAdded(VSAbstractFilter* filter, bool currentFilter)
{
  vtkRenderWindowInteractor* interactor = nullptr;
  VSAbstractViewWidget* activeViewWidget = getActiveViewWidget();
  if(activeViewWidget != nullptr)
  {
    VSVisualizationWidget* vizWidget = activeViewWidget->getVisualizationWidget();
    if(vizWidget != nullptr)
    {
      interactor = vizWidget->GetInteractor();
    }
  }

  if(currentFilter == true)
  {
    selectFilter(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::filterRemoved(VSAbstractFilter* filter)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::launchHDF5SelectionDialog(const QString& filePath)
{
  QSharedPointer<LoadHDF5FileDialog> dialog = QSharedPointer<LoadHDF5FileDialog>(new LoadHDF5FileDialog());
  dialog->setHDF5FilePath(filePath);
  int ret = dialog->exec();

  if(ret == QDialog::Accepted)
  {
    SIMPLH5DataReader reader;

    bool success = reader.openFile(filePath);
    if(success)
    {
      connect(&reader, SIGNAL(errorGenerated(const QString&, const QString&, const int&)), this, SLOT(generateError(const QString&, const QString&, const int&)));

      DataContainerArrayProxy dcaProxy = dialog->getDataStructureProxy();
      DataContainerArray::Pointer dca = reader.readSIMPLDataUsingProxy(dcaProxy, false);
      if(dca.get() == nullptr)
      {
        return;
      }
      m_Controller->importDataContainerArray(filePath, dca);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSMainWidgetBase::importDataContainerArray(const QString &filePath, DataContainerArray::Pointer dca)
{
  m_Controller->importDataContainerArray(filePath, dca);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSMainWidgetBase::importPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  m_Controller->importPipelineOutput(pipeline, dca);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSMainWidgetBase::importPipelineOutput(std::vector<FilterPipeline::Pointer> pipelines)
{
	m_Controller->importPipelineOutput(pipelines);
	return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::importFilterPipeline(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  m_Controller->importPipelineOutput(pipeline, dca);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::generateError(const QString& title, const QString& msg, const int& code)
{
  QMessageBox::critical(this, title, msg, QMessageBox::StandardButton::Ok);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setActiveView(VSAbstractViewWidget* viewWidget)
{
  // Do nothing if the active view is not changed
  if(viewWidget == m_ActiveViewWidget)
  {
    return;
  }

  // Disconnect the old active view widget
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->setActive(false);

    disconnect(m_ActiveViewWidget, SIGNAL(viewWidgetClosed()), this, SLOT(activeViewClosed()));
    disconnect(m_ActiveViewWidget, SIGNAL(applyCurrentFilter()), this, SLOT(applyCurrentFilter()));
    disconnect(m_ActiveViewWidget, SIGNAL(resetCurrentFilter()), this, SLOT(resetCurrentFilter()));
  }

  m_ActiveViewWidget = viewWidget;

  // Connect the new active view widget
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->setActive(true);

    connect(m_ActiveViewWidget, SIGNAL(viewWidgetClosed()), this, SLOT(activeViewClosed()));
    connect(m_ActiveViewWidget, SIGNAL(applyCurrentFilter()), this, SLOT(applyCurrentFilter()));
    connect(m_ActiveViewWidget, SIGNAL(resetCurrentFilter()), this, SLOT(resetCurrentFilter()));
    connect(m_ActiveViewWidget, &VSAbstractViewWidget::currentFilterChanged, this, &VSMainWidgetBase::listenCurrentFilterChanged);
    connect(m_ActiveViewWidget, &VSAbstractViewWidget::selectionChanged, this, &VSMainWidgetBase::listenSelectionChanged);

    listenCurrentFilterChanged(m_ActiveViewWidget->getCurrentFilter());
    emit selectedFiltersChanged(m_ActiveViewWidget->getSelectedFilters());
  }
  else
  {
    listenCurrentFilterChanged(nullptr);
    emit selectedFiltersChanged(VSAbstractFilter::FilterListType());
  }

  emit changedActiveView(viewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::activeViewClosed()
{
  setActiveView(nullptr);
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilterWidget::ListType VSMainWidgetBase::getFilterWidgets(VSAbstractFilter::FilterListType filters)
{
  VSAbstractFilterWidget::ListType filterWidgets;
  for(VSAbstractFilter* filter : filters)
  {
    VSAbstractFilterWidget* widget = m_FilterToFilterWidgetMap.value(filter);
    if(nullptr != widget)
    {
      filterWidgets.push_back(widget);
    }
  }

  return filterWidgets;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setCurrentFilter(VSAbstractFilter* filter)
{
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->selectFilter(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::changeFilterSelected(VSAbstractViewWidget::FilterStepChange stepDirection, bool addSelection)
{
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->changeFilterSelected(stepDirection, addSelection);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::listenSelectionChanged(QItemSelection selection)
{
  if(nullptr == m_ActiveViewWidget)
  {
    return;
  }

  VSAbstractFilter::FilterListType filters = m_ActiveViewWidget->getSelectedFilters();
  if(getCurrentFilter())
  {
    getCurrentFilter()->getValues()->setSelection(filters);
  }

  emit selectedFiltersChanged(filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::listenCurrentFilterChanged(VSAbstractFilter* filter)
{
  if(m_Controller->getFilterModel()->getRootFilter() == filter)
  {
    filter = nullptr;
  }

  if(m_CurrentFilter)
  {
    m_CurrentFilter->getValues()->setRenderingEnabled(false);
  }

  m_CurrentFilter = filter;

  if(m_CurrentFilter)
  {
    m_CurrentFilter->getValues()->setSelection(getCurrentSelection());
    m_CurrentFilter->getValues()->setInteractor(getActiveViewWidget()->getVisualizationWidget()->GetInteractor());
    m_CurrentFilter->getValues()->setRenderingEnabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::applyCurrentFilter()
{
  if(getCurrentFilter())
  {
    getCurrentFilter()->getValues()->applyValues();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::resetCurrentFilter()
{
  if(getCurrentFilter())
  {
    getCurrentFilter()->getValues()->resetValues();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::deleteCurrentFilter()
{
  if(getCurrentFilter())
  {
    getCurrentFilter()->deleteFilter();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::toggleCurrentFilterVisibility()
{
  if(getActiveViewWidget() && getActiveViewWidget()->getFilterViewSettings(getCurrentFilter()))
  {
    VSFilterViewSettings* viewSettings = getActiveViewWidget()->getFilterViewSettings(getCurrentFilter());
    viewSettings->setVisible(!viewSettings->isVisible());
  }
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
    bool visible = viewSettings->isVisible();
    viewSettings->setVisible(false);

    VSFilterViewSettings* parentSettings = widget->getFilterViewSettings(filter->getParentFilter());
    if(parentSettings)
    {
      bool parentVisible = parentSettings->isVisible();
      parentSettings->setVisible(visible || parentVisible);
    }
  }

  VSAbstractFilter::FilterListType childFilters = filter->getChildren();
  for(VSAbstractFilter* child : childFilters)
  {
    deleteFilter(child);
  }

  if(getCurrentFilter() == filter)
  {
    selectFilter(filter->getParentFilter());
  }

  m_Controller->getFilterModel()->removeFilter(filter);
  emit selectedFiltersChanged(getActiveViewWidget()->getSelectedFilters());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::clearFilters()
{
  VSAbstractFilter::FilterListType filters = getController()->getFilterModel()->getBaseFilters();
  for(VSAbstractFilter* filter : filters)
  {
    deleteFilter(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::reloadDataFilter(VSAbstractDataFilter* filter)
{
  std::vector<VSAbstractDataFilter*> filters;
  filters.push_back(filter);

  QtConcurrent::run(this, &VSMainWidgetBase::reloadFilters, filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::renameDataFilter(VSAbstractDataFilter* filter)
{
  bool ok;
  VSSIMPLDataContainerFilter* dcFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filter);
  if(dcFilter != nullptr)
  {
	DataContainer::Pointer dataContainer = dcFilter->getWrappedDataContainer()->m_DataContainer;
	if(dataContainer != nullptr)
	{
	  VSSIMPLDataContainerValues* dcValues = dynamic_cast<VSSIMPLDataContainerValues*>(dcFilter->getValues());
	  QString dcName = QInputDialog::getText(this, tr("Rename Filter"),
		tr("New Data Container Name:"), QLineEdit::Normal,
		dcFilter->getFilterName(), &ok);
	  if(ok && !dcName.isEmpty())
	  {
		dcValues->getWrappedDataContainer()->m_Name = dcName;
		dataContainer->setName(dcName);
		dcFilter->setText(dcName);
	  }
	}
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::reloadFileFilter(VSFileNameFilter* filter)
{
  VSAbstractFilter::FilterListType childFilters = filter->getChildren();
  std::vector<VSAbstractDataFilter*> filters;
  for(VSAbstractFilter* childFilter : childFilters)
  {
    VSAbstractDataFilter* dataFilter = dynamic_cast<VSAbstractDataFilter*>(childFilter);
    if(dataFilter)
    {
      filters.push_back(dataFilter);
    }
  }

  QtConcurrent::run(this, &VSMainWidgetBase::reloadFilters, filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::reloadFilters(std::vector<VSAbstractDataFilter*> filters)
{
  if(filters.size() == 1)
  {
    // This is a single filter, so do a simple reload
    VSAbstractDataFilter* filter = filters[0];
    filter->reloadData();
  }
  else if(filters.size() > 1 && dynamic_cast<VSSIMPLDataContainerFilter*>(filters[0]) != nullptr)
  {
    // This is from a file containing multiple SIMPL Data Containers, so we will use this block of code to optimize the file reading process
    QSharedPointer<SIMPLH5DataReader> reader = QSharedPointer<SIMPLH5DataReader>(new SIMPLH5DataReader());
    connect(reader.data(), SIGNAL(errorGenerated(const QString&, const QString&, const int&)), this, SLOT(generateError(const QString&, const QString&, const int&)));

    VSSIMPLDataContainerFilter* simplFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filters[0]);
    VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(simplFilter->getParentFilter());
    if(fileNameFilter == nullptr)
    {
      QString ss = QObject::tr("Data Container filters could not be reloaded because they do not have a file filter parent.");
      emit generateError("Data Reload Error", ss, -3002);
    }

    bool success = reader->openFile(fileNameFilter->getFilePath());
    if(success)
    {
      int err = 0;
      DataContainerArrayProxy dcaProxy = reader->readDataContainerArrayStructure(nullptr, err);

      for(size_t i = 0; i < filters.size(); i++)
      {
        VSSIMPLDataContainerFilter* validFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filters[i]);
		QMap<QString, DataContainerProxy>& dataContainers = dcaProxy.getDataContainers();
        DataContainerProxy dcProxy = dataContainers.value(validFilter->getFilterName());

        AttributeMatrixProxy::AMTypeFlags amFlags(AttributeMatrixProxy::AMTypeFlag::Cell_AMType);
        DataArrayProxy::PrimitiveTypeFlags pFlags(DataArrayProxy::PrimitiveTypeFlag::Any_PType);
        DataArrayProxy::CompDimsVector compDimsVector;

        dcProxy.setFlags(Qt::Checked, amFlags, pFlags, compDimsVector);
        dataContainers[dcProxy.getName()] = dcProxy;
      }

      DataContainerArray::Pointer dca = reader->readSIMPLDataUsingProxy(dcaProxy, false);
      m_Controller->reloadDataContainerArray(fileNameFilter, dca);
    }
  }
  else
  {
    // This should not happen, so throw an error and bail!
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createClipFilter(VSAbstractFilter::FilterListType parents)
{
  if(parents.size() == 0)
  {
    parents = getCurrentSelection();
  }

  VSAbstractFilter::FilterListType createdFilters;
  for(VSAbstractFilter* parent : parents)
  {
    if(parent && VSClipFilter::CompatibleWithParent(parent))
    {
      VSClipFilter* filter = new VSClipFilter(parent);
      finishAddingFilter(filter, parent);
      createdFilters.push_back(filter);
    }
  }

  selectFilters(createdFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createCropFilter(VSAbstractFilter::FilterListType parents)
{
  if(parents.size() == 0)
  {
    parents = getCurrentSelection();
  }

  VSAbstractFilter::FilterListType createdFilters;
  for(VSAbstractFilter* parent : parents)
  {
    if(parent && VSCropFilter::CompatibleWithParent(parent))
    {
      VSCropFilter* filter = new VSCropFilter(parent);
      finishAddingFilter(filter, parent);
      createdFilters.push_back(filter);
    }
  }

  selectFilters(createdFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createSliceFilter(VSAbstractFilter::FilterListType parents)
{
  if(parents.size() == 0)
  {
    parents = getCurrentSelection();
  }

  VSAbstractFilter::FilterListType createdFilters;
  for(VSAbstractFilter* parent : parents)
  {
    if(parent && VSSliceFilter::CompatibleWithParent(parent))
    {
      VSSliceFilter* filter = new VSSliceFilter(parent);
      finishAddingFilter(filter, parent);
      createdFilters.push_back(filter);
    }
  }

  selectFilters(createdFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createMaskFilter(VSAbstractFilter::FilterListType parents)
{
  if(parents.size() == 0)
  {
    parents = getCurrentSelection();
  }

  VSAbstractFilter::FilterListType createdFilters;
  for(VSAbstractFilter* parent : parents)
  {
    if(parent && VSMaskFilter::CompatibleWithParent(parent))
    {
      VSMaskFilter* filter = new VSMaskFilter(parent);
      finishAddingFilter(filter, parent);
      createdFilters.push_back(filter);
    }
  }

  selectFilters(createdFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createThresholdFilter(VSAbstractFilter::FilterListType parents)
{
  if(parents.size() == 0)
  {
    parents = getCurrentSelection();
  }

  VSAbstractFilter::FilterListType createdFilters;
  for(VSAbstractFilter* parent : parents)
  {
    if(parent && VSThresholdFilter::CompatibleWithParent(parent))
    {
      VSThresholdFilter* filter = new VSThresholdFilter(parent);
      finishAddingFilter(filter, parent);
      createdFilters.push_back(filter);
    }
  }

  selectFilters(createdFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createTextFilter(VSAbstractFilter::FilterListType parents)
{
  if(parents.size() == 0)
  {
    parents = getCurrentSelection();
  }

  VSAbstractFilter::FilterListType createdFilters;
  for(VSAbstractFilter* parent : parents)
  {
    if(parent && VSTextFilter::CompatibleWithParent(parent))
    {
      VSTextFilter* filter = new VSTextFilter(parent, "Text", "Tool Tip");
      finishAddingFilter(filter, parent);
      createdFilters.push_back(filter);
    }
  }

  selectFilters(createdFilters);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setBlockRender(bool block)
{
  for(VSAbstractViewWidget* viewWidget : getAllViewWidgets())
  {
    viewWidget->setBlockRender(block);
  }
}
