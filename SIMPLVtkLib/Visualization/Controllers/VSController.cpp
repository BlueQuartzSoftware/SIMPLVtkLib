/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "VSController.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUuid>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/Dialogs/FijiListWidget.h"
#include "SIMPLVtkLib/Dialogs/Utilities/TileConfigFileGenerator.h"

#include "SIMPLVtkLib/QtWidgets/VSFilterFactory.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"

#include "SIMPLVtkLib/Dialogs/RobometListWidget.h"
#include "SIMPLVtkLib/Dialogs/Utilities/ImporterWorker.h"
#include "SIMPLVtkLib/Dialogs/ZeissListWidget.h"

#include "SIMPLVtkLib/Wizards/PerformMontage/PerformMontageConstants.h"
#include "SIMPLVtkLib/Wizards/PerformMontage/PerformMontageWizard.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::VSController(QObject* parent)
: QObject(parent)
, m_FilterModel(new VSFilterModel())
{
  m_ImportObject = new VSConcurrentImport(this);

  qRegisterMetaType<VSAbstractImporter::Pointer>();

  connect(m_FilterModel, &VSFilterModel::filterAdded, this, &VSController::filterAdded);
  connect(m_FilterModel, &VSFilterModel::filterRemoved, this, &VSController::filterRemoved);

  // VSConcurrentImport works on another thread, so use the old-style connections to forward signals on the current thread
  connect(m_ImportObject, SIGNAL(blockRender(bool)), this, SIGNAL(blockRender(bool)));
  connect(m_ImportObject, SIGNAL(applyingDataFilters(int)), this, SIGNAL(applyingDataFilters(int)));
  connect(m_ImportObject, SIGNAL(dataFilterApplied(int)), this, SIGNAL(dataFilterApplied(int)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::~VSController()
{
  delete m_FilterModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainerArray(QString filePath, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray(filePath, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::reloadDataContainerArray(VSFileNameFilter* fileFilter, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Reload);
  m_ImportObject->addDataContainerArray(fileFilter, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray(pipeline, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importPipelineOutput(std::vector<FilterPipeline::Pointer> pipelines)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  for(FilterPipeline::Pointer pipeline : pipelines)
  {
    DataContainerArray::Pointer dca = pipeline->getDataContainerArray();
    m_ImportObject->addDataContainerArray(pipeline, dca);
  }
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::reloadPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  VSPipelineFilter* parentFilter = dynamic_cast<VSPipelineFilter*>(getFilterModel()->getPipelineFilter(pipeline));
  if(parentFilter)
  {
    m_ImportObject->setLoadType(VSConcurrentImport::LoadType::SemiReload);
    m_ImportObject->addDataContainerArray(parentFilter, dca);
  }
  else
  {
    m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Geometry);
    m_ImportObject->addDataContainerArray(pipeline, dca);
  }

  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainerArray(DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray("No File", dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainer(DataContainer::Pointer dc)
{
  SIMPLVtkBridge::WrappedDataContainerPtr wrappedData = SIMPLVtkBridge::WrapDataContainerAsStruct(dc);

  // Add VSSIMPLDataContainerFilter if the DataContainer contains relevant data for rendering
  if(wrappedData)
  {
    VSSIMPLDataContainerFilter* filter = new VSSIMPLDataContainerFilter(wrappedData);
    m_FilterModel->addFilter(filter);
  }

  emit dataImported();
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::selectFilter(VSAbstractFilter* filter)
{
  // Do not change the selection when nullptr is passed in
  if(nullptr == filter || m_FilterModel->getRootFilter() == filter)
  {
    return;
  }

  QModelIndex index = m_FilterModel->getIndexFromFilter(filter);
  //m_SelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::saveSession(const QString& sessionFilePath)
{
  QFile outputFile(sessionFilePath);
  if(outputFile.open(QIODevice::WriteOnly))
  {
    QJsonObject rootObj;

    VSAbstractFilter::FilterListType baseFilters = getBaseFilters();
    for(VSAbstractFilter* filter : baseFilters)
    {
      saveFilter(filter, rootObj);
    }

    QJsonDocument doc(rootObj);

    outputFile.write(doc.toJson());
    outputFile.close();

    return true;
  }
  else
  {
    // "Failed to open output file" error
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::saveAsImage(const QString& imageFilePath, VSAbstractFilter* filter)
{
  bool imageSaved = false;
  VSSIMPLDataContainerFilter* dcFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filter);
  if(dcFilter != nullptr)
  {
    VSFilterFactory::Pointer filterFactory = VSFilterFactory::New();
    FilterPipeline::Pointer pipeline = FilterPipeline::New();
    DataContainer::Pointer dataContainer = dcFilter->getWrappedDataContainer()->m_DataContainer;
    DataContainer::Container_t attributeMatricies = dataContainer->getAttributeMatrices();
    QString dcName = dataContainer->getName();
    QString amName;
    QString dataArrayName;
    for(AttributeMatrix::Pointer am : attributeMatricies)
    {
      if(am->getType() == AttributeMatrix::Type::Cell)
      {
        amName = am->getName();
        dataArrayName = am->getAttributeArrayNames().first();
      }
    }
    DataContainerArray::Pointer dca = DataContainerArray::New();
    dca->addOrReplaceDataContainer(dataContainer);
    AbstractFilter::Pointer imageWriter = filterFactory->createImageFileWriterFilter(imageFilePath, dcName, amName, dataArrayName);
    if(imageWriter != AbstractFilter::NullPointer())
    {
      pipeline->pushBack(imageWriter);
      pipeline->execute(dca);
      if(pipeline->getErrorCode() >= 0)
      {
        imageSaved = true;
      }
    }
  }
  return imageSaved;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::saveAsDREAM3D(const QString& outputFilePath, VSAbstractFilter* filter)
{
  bool dream3dFileSaved = false;
  VSFilterFactory::Pointer filterFactory = VSFilterFactory::New();
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  DataContainerArray::Pointer dca = DataContainerArray::New();
  VSSIMPLDataContainerFilter* dcFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filter);
  VSPipelineFilter* pipelineFilter = dynamic_cast<VSPipelineFilter*>(filter);
  VSFileNameFilter* filenameFilter = dynamic_cast<VSFileNameFilter*>(filter);

  // If it is a data container filter, get the parent filter (a pipeline)
  if(dcFilter != nullptr)
  {
	pipelineFilter = dynamic_cast<VSPipelineFilter*>(dcFilter->getParentFilter());
  }
  if(pipelineFilter != nullptr)
  {
	VSAbstractFilter::FilterListType children = pipelineFilter->getChildren();
	for(VSAbstractFilter* childFilter : children)
	{
	  dcFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(childFilter);
	  if(dcFilter != nullptr)
	  {
		DataContainer::Pointer dataContainer = dcFilter->getWrappedDataContainer()->m_DataContainer;
		if(dataContainer != nullptr)
		{
		  dca->addOrReplaceDataContainer(dataContainer);
		}
	  }
	}
  }
  else if(filenameFilter != nullptr)
  {
	VSAbstractFilter::FilterListType children = filenameFilter->getChildren();
	for(VSAbstractFilter* childFilter : children)
	{
	  dcFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(childFilter);
	  if(dcFilter != nullptr)
	  {
		DataContainer::Pointer dataContainer = dcFilter->getWrappedDataContainer()->m_DataContainer;
		if(dataContainer != nullptr)
		{
		  double* pos = dcFilter->getTransform()->getLocalPosition();
		  ImageGeom::Pointer geom = dataContainer->getGeometryAs<ImageGeom>();
		  geom->setOrigin(pos[0], pos[1], pos[2]);
		  dca->addOrReplaceDataContainer(dataContainer);
		}
	  }
	}
  }
  else
  {
	return false;
  }

  AbstractFilter::Pointer dcWriter = filterFactory
	->createDataContainerWriterFilter(outputFilePath, true, false);
  if(dcWriter != AbstractFilter::NullPointer())
  {
	pipeline->pushBack(dcWriter);
	pipeline->execute(dca);
	if(pipeline->getErrorCode() >= 0)
	{
	  return true;
	}
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::saveFilter(VSAbstractFilter* filter, QJsonObject& obj)
{
  QJsonObject rootFilterObj;

  filter->writeJson(rootFilterObj);

  // Write the children
  QJsonObject childrenObj;

  VSAbstractFilter::FilterListType childFilters = filter->getChildren();
  for(VSAbstractFilter* childFilter : childFilters)
  {
    saveFilter(childFilter, childrenObj);
  }

  rootFilterObj["Child Filters"] = childrenObj;

  obj[filter->getFilterName()] = rootFilterObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::loadSession(const QString& sessionFilePath)
{
  QFile inputFile(sessionFilePath);
  if(inputFile.open(QIODevice::ReadOnly) == false)
  {
    return false;
  }

  QByteArray byteArray = inputFile.readAll();
  QJsonParseError parseError;

  QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
  if(parseError.error != QJsonParseError::NoError)
  {
    return false;
  }

  QJsonObject rootObj = doc.object();
  for(QJsonObject::iterator iter = rootObj.begin(); iter != rootObj.end(); iter++)
  {
    QJsonObject filterObj = iter.value().toObject();
    loadFilter(filterObj);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::loadFilter(QJsonObject& obj, VSAbstractFilter* parentFilter)
{
  QUuid uuid(obj["Uuid"].toString());

  VSAbstractFilter* newFilter = nullptr;
  if(uuid == VSClipFilter::GetUuid())
  {
    newFilter = VSClipFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSCropFilter::GetUuid())
  {
    newFilter = VSCropFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSDataSetFilter::GetUuid())
  {
    if(dynamic_cast<VSFileNameFilter*>(parentFilter) != nullptr)
    {
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
      QString filePath = fileNameFilter->getFilePath();

      newFilter = VSDataSetFilter::Create(filePath, obj, parentFilter);
    }
  }
  else if(uuid == VSFileNameFilter::GetUuid())
  {
    newFilter = VSFileNameFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSMaskFilter::GetUuid())
  {
    newFilter = VSMaskFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSSIMPLDataContainerFilter::GetUuid())
  {
    if(dynamic_cast<VSFileNameFilter*>(parentFilter) != nullptr)
    {
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
      QString filePath = fileNameFilter->getFilePath();

      newFilter = VSSIMPLDataContainerFilter::Create(filePath, obj, parentFilter);
    }
  }
  else if(uuid == VSSliceFilter::GetUuid())
  {
    newFilter = VSSliceFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSTextFilter::GetUuid())
  {
    newFilter = VSTextFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSThresholdFilter::GetUuid())
  {
    newFilter = VSThresholdFilter::Create(obj, parentFilter);
  }

  if(newFilter != nullptr)
  {
    QJsonObject childrenObj = obj["Child Filters"].toObject();

    m_FilterModel->addFilter(newFilter, (childrenObj.size() == 0));
    newFilter->setChecked(obj["CheckState"].toInt() == Qt::Checked);
    emit filterCheckStateChanged(newFilter);

    for(QJsonObject::iterator iter = childrenObj.begin(); iter != childrenObj.end(); iter++)
    {
      QJsonObject childObj = iter.value().toObject();
      loadFilter(childObj, newFilter);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFileNameFilter* VSController::getBaseFileNameFilter(QString text)
{
  VSAbstractFilter::FilterListType baseFilters = getBaseFilters();
  for(VSAbstractFilter* baseFilter : baseFilters)
  {
    VSFileNameFilter* filter = dynamic_cast<VSFileNameFilter*>(baseFilter);
    if(filter)
    {
      if(filter->getFilterName().compare(text) == 0)
      {
        return filter;
      }
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSController::getBaseFilters()
{
  return m_FilterModel->getBaseFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSController::getAllFilters()
{
  return m_FilterModel->getAllFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterModel* VSController::getFilterModel()
{
  return m_FilterModel;
}
