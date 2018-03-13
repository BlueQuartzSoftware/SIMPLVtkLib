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

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QFile>
#include <QtCore/QThread>
#include <QtCore/QUuid>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/Dialogs/LoadHDF5FileDialog.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSClipFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSCropFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSDataSetFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSSIMPLDataContainerFilterWidget.h"
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
  connect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*, bool)),
    this, SLOT(filterAdded(VSAbstractFilter*, bool)));
  connect(m_Controller, SIGNAL(filterRemoved(VSAbstractFilter*)), 
    this, SLOT(filterRemoved(VSAbstractFilter*)));
  connect(m_Controller, SIGNAL(blockRender(bool)),
    this, SLOT(setBlockRender(bool)));
  connect(m_Controller, SIGNAL(filterSelected(VSAbstractFilter*)),
    this, SLOT(setCurrentFilter(VSAbstractFilter*)));

  connect(this, SIGNAL(proxyFromFilePathGenerated(DataContainerArrayProxy, const QString &)),
          this, SLOT(launchSIMPLSelectionDialog(DataContainerArrayProxy, const QString &)));
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
    disconnect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)), view, SLOT(setViewWidget(VSAbstractViewWidget*)));
    disconnect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)),
      view, SLOT(setActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));
  }

  view->setController(m_Controller);

  m_FilterView = view;
  connect(view, SIGNAL(filterClicked(VSAbstractFilter*)), this, SLOT(setCurrentFilter(VSAbstractFilter*)));
  connect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)), view, SLOT(setViewWidget(VSAbstractViewWidget*)));
  connect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)),
    view, SLOT(setActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));
  
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
void VSMainWidgetBase::filterAdded(VSAbstractFilter* filter, bool currentFilter)
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
  else if (dynamic_cast<VSSIMPLDataContainerFilter*>(filter) != nullptr)
  {
    VSSIMPLDataContainerFilter* vsFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filter);
    fw = new VSSIMPLDataContainerFilterWidget(vsFilter, this);
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

  if (currentFilter == true)
  {
    setCurrentFilter(filter);
  }
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
void VSMainWidgetBase::importFiles(QStringList filePaths)
{
  for (int i = 0; i < filePaths.size(); i++)
  {
    QString filePath = filePaths[i];

    QMimeDatabase db;

    QMimeType mimeType = db.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
    QString mimeName = mimeType.name();

    QFileInfo fi(filePath);
    QString ext = fi.completeSuffix().toLower();
    if (ext == "dream3d")
    {
      openDREAM3DFile(filePath);
    }
    else if (mimeType.inherits("image/png") || mimeType.inherits("image/tiff") || mimeType.inherits("image/jpeg") || mimeType.inherits("image/bmp"))
    {
      m_Controller->importData(filePath);
    }
    else if (ext == "vtk" || ext == "vti" || ext == "vtp" || ext == "vtr"
             || ext == "vts" || ext == "vtu")
    {
      m_Controller->importData(filePath);
    }
    else if (ext == "stl")
    {
      m_Controller->importData(filePath);
    }
    else
    {
      QMessageBox::critical(this, "Invalid File Type",
                            tr("IMF Viewer failed to open the file because the file extension, '.%1', is not supported by the "
                               "application.").arg(ext), QMessageBox::StandardButton::Ok);
      continue;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::openDREAM3DFile(const QString &filePath)
{
  QFileInfo fi(filePath);

  SIMPLH5DataReader reader;
  connect(&reader, SIGNAL(errorGenerated(const QString &, const QString &, const int &)),
          this, SLOT(generateError(const QString &, const QString &, const int &)));

  bool success = reader.openFile(filePath);
  if (success)
  {
    int err = 0;
    SIMPLH5DataReaderRequirements req(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    DataContainerArrayProxy proxy = reader.readDataContainerArrayStructure(&req, err);
    if (proxy.dataContainers.isEmpty())
    {
      return;
    }

    bool containsCellAttributeMatrices = false;
    bool containsValidArrays = false;

    QStringList dcNames = proxy.dataContainers.keys();
    for (int i = 0; i < dcNames.size(); i++)
    {
      QString dcName = dcNames[i];
      DataContainerProxy dcProxy = proxy.dataContainers[dcName];

      // We want only data containers with geometries displayed
      if (dcProxy.dcType == static_cast<unsigned int>(DataContainer::Type::Unknown))
      {
        proxy.dataContainers.remove(dcName);
      }
      else
      {
        QStringList amNames = dcProxy.attributeMatricies.keys();
        for (int j = 0; j < amNames.size(); j++)
        {
          QString amName = amNames[j];
          AttributeMatrixProxy amProxy = dcProxy.attributeMatricies[amName];

          // We want only cell attribute matrices displayed
          if (amProxy.amType != AttributeMatrix::Type::Cell)
          {
            dcProxy.attributeMatricies.remove(amName);
            proxy.dataContainers[dcName] = dcProxy;
          }
          else
          {
            containsCellAttributeMatrices = true;

            if (amProxy.dataArrays.size() > 0)
            {
              containsValidArrays = true;
            }
          }
        }
      }
    }

    if (proxy.dataContainers.size() <= 0)
    {
      QMessageBox::critical(this, "Invalid Data",
                            tr("IMF Viewer failed to open file '%1' because the file does not "
                               "contain any data containers with a supported geometry.").arg(fi.fileName()), QMessageBox::StandardButton::Ok);
      return;
    }

    emit proxyFromFilePathGenerated(proxy, filePath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::launchSIMPLSelectionDialog(DataContainerArrayProxy proxy, const QString &filePath)
{
  QSharedPointer<LoadHDF5FileDialog> dialog = QSharedPointer<LoadHDF5FileDialog>(new LoadHDF5FileDialog());
  dialog->setProxy(proxy);
  int ret = dialog->exec();

  if (ret == QDialog::Accepted)
  {
    SIMPLH5DataReader reader;

    bool success = reader.openFile(filePath);
    if (success)
    {
      connect(&reader, SIGNAL(errorGenerated(const QString &, const QString &, const int &)),
              this, SLOT(generateError(const QString &, const QString &, const int &)));

      DataContainerArrayProxy dcaProxy = dialog->getDataStructureProxy();
      DataContainerArray::Pointer dca = reader.readSIMPLDataUsingProxy(dcaProxy, false);
      if (dca.get() == nullptr)
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
void VSMainWidgetBase::generateError(const QString &title, const QString &msg, const int &code)
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
  if(false == (viewSettings && viewSettings->getFilter()))
  {
    return;
  }
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
  VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(m_CurrentFilter);
  if(filterWidget)
  {
    filterWidget->setRenderingEnabled(false);
  }

  m_CurrentFilter = filter;
  filterWidget = m_FilterToFilterWidgetMap.value(filter);

  if(filterWidget)
  {
    filterWidget->setRenderingEnabled(true);
  }

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
    bool visible = viewSettings->isVisible();
    viewSettings->setVisible(false);
    viewSettings->setScalarBarVisible(false);

    VSFilterViewSettings* parentSettings = widget->getFilterViewSettings(filter->getParentFilter());
    if(parentSettings)
    {
      bool parentVisible = parentSettings->isVisible();
      parentSettings->setVisible(visible || parentVisible);
    }
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

  if(parent && VSClipFilter::compatibleWithParent(parent))
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

  if(parent && VSCropFilter::compatibleWithParent(parent))
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

  if(parent && VSSliceFilter::compatibleWithParent(parent))
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

  if(parent && VSMaskFilter::compatibleWithParent(parent))
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

  if(parent && VSThresholdFilter::compatibleWithParent(parent))
  {
    VSThresholdFilter* filter = new VSThresholdFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createTextFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent && VSTextFilter::compatibleWithParent(parent))
  {
    VSTextFilter* filter = new VSTextFilter(parent, "Text", "Tool Tip");
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

  //QThread::currentThread()->wait(1);
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
