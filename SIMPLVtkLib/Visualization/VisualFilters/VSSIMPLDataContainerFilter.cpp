/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "VSSIMPLDataContainerFilter.h"

#include <QtConcurrent>
#include <QtCore/QUuid>

#include <vtkAlgorithmOutput.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkTransformFilter.h>
#include <vtkPointSet.h>
#include <vtkExtractVOI.h>

#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSPipelineFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSIMPLDataContainerFilter::VSSIMPLDataContainerFilter(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDataContainer, VSAbstractFilter* parent)
: VSAbstractDataFilter()
, m_WrappingWatcher(this)
, m_ApplyLock(1)
{
  m_DCValues = new VSSIMPLDataContainerValues(this);
  m_DCValues->setWrappedDataContainer(wrappedDataContainer);

  createFilter();
  setParentFilter(parent);

  connect(this, SIGNAL(finishedWrapping()), this, SLOT(apply()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSIMPLDataContainerFilter::~VSSIMPLDataContainerFilter()
{
  if(m_DCValues->getWrappedDataContainer())
  {
    size_t count = m_DCValues->getWrappedDataContainer()->m_CellData.size();
    for(size_t i = 0; i < count; i++)
    {
      m_DCValues->getWrappedDataContainer()->m_CellData[i]->m_SIMPLArray = nullptr;
      m_DCValues->getWrappedDataContainer()->m_CellData[i]->m_VtkArray = nullptr;
    }
    m_DCValues->getWrappedDataContainer()->m_CellData.clear();
    m_DCValues->getWrappedDataContainer() = nullptr;
  }

  delete m_DCValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSSIMPLDataContainerFilter::getBounds() const
{
  return m_DCValues->getWrappedDataContainer()->m_DataSet->GetBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSSIMPLDataContainerFilter::getOutputPort()
{
  if(m_TrivialProducer)
  {
    return m_TrivialProducer->GetOutputPort();
  }
  else
  {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) VSSIMPLDataContainerFilter::getOutput() const
{
  if(nullptr == m_DCValues->getWrappedDataContainer())
  {
    return nullptr;
  }

  return m_DCValues->getWrappedDataContainer()->m_DataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSIMPLDataContainerFilter* VSSIMPLDataContainerFilter::Create(const QString& filePath, QJsonObject& json, VSAbstractFilter* parent)
{
  QString dcName = json["Data Container Name"].toString();

  // Read in the data from the file and initialize the filter
  SIMPLH5DataReader reader;
  bool success = reader.openFile(filePath);
  if(success)
  {
    int err = 0;
    DataContainerArrayProxy proxy = reader.readDataContainerArrayStructure(nullptr, err);
	QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();
    for(QMap<QString, DataContainerProxy>::iterator dcIter = dataContainers.begin(); dcIter != dataContainers.end(); dcIter++)
    {
      if(dcIter.key() == dcName)
      {
        DataContainerProxy dcProxy = dcIter.value();
		QMap<QString, AttributeMatrixProxy>& attributeMatricies = dcProxy.getAttributeMatricies();
        dcProxy.setFlag(Qt::Checked);

        for(QMap<QString, AttributeMatrixProxy>::iterator amIter = attributeMatricies.begin(); amIter != attributeMatricies.end(); amIter++)
        {
          AttributeMatrixProxy amProxy = amIter.value();

          if(amProxy.getAMType() == AttributeMatrix::Type::Cell)
          {
            amProxy.setFlag(Qt::Checked);
          }
		  QMap<QString, DataArrayProxy> dataArrays = amProxy.getDataArrays();
          for(QMap<QString, DataArrayProxy>::iterator daIter = dataArrays.begin(); daIter != dataArrays.end(); daIter++)
          {
            DataArrayProxy daProxy = daIter.value();
            daProxy.setFlag(Qt::Checked);

            dataArrays[daProxy.getName()] = daProxy;
          }

          attributeMatricies[amProxy.getName()] = amProxy;
        }

        dataContainers[dcProxy.getName()] = dcProxy;

        DataContainerArray::Pointer dca = reader.readSIMPLDataUsingProxy(proxy, false);
        DataContainerShPtr dc = dca->getDataContainer(dcName);
        if(dc)
        {
          SIMPLVtkBridge::WrappedDataContainerPtr wrappedDC = SIMPLVtkBridge::WrapDataContainerAsStruct(dc);

          VSSIMPLDataContainerFilter* newFilter = new VSSIMPLDataContainerFilter(wrappedDC, parent);
          newFilter->setToolTip(json["Tooltip"].toString());
          newFilter->setInitialized(true);
          newFilter->readTransformJson(json);
          return newFilter;
        }
      }
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerFilter::writeJson(QJsonObject& json)
{
  VSAbstractFilter::writeJson(json);

  json["Data Container Name"] = getText();
  json["Tooltip"] = getToolTip();
  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSSIMPLDataContainerFilter::GetUuid()
{
  return QUuid("{d1c86a47-85b5-55bd-bcdb-0bdb6c5cf020}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerFilter::createFilter()
{
  connect(&m_WrappingWatcher, SIGNAL(finished()), this, SLOT(reloadWrappingFinished()));

  getTransform()->setLocalPosition(m_DCValues->getWrappedDataContainer()->m_Origin);
  getTransform()->setOriginPosition(m_DCValues->getWrappedDataContainer()->m_Origin);
  updateTransformFilter();

  VTK_PTR(vtkDataSet) dataSet = m_DCValues->getWrappedDataContainer()->m_DataSet;
  dataSet->ComputeBounds();

  vtkCellData* cellData = dataSet->GetCellData();
  if(cellData)
  {
    vtkDataArray* dataArray = cellData->GetArray(0);
    if(dataArray)
    {
      char* name = dataArray->GetName();
      cellData->SetActiveScalars(name);
    }
  }

  m_TrivialProducer = VTK_PTR(vtkTrivialProducer)::New();
  m_TrivialProducer->SetOutput(dataSet);

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerFilter::reloadData()
{
  QString dcName = m_DCValues->getWrappedDataContainer()->m_Name;

  VSAbstractFilter* parentFilter = getParentFilter();
  VSFileNameFilter* fileFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
  VSPipelineFilter* pipelineFilter = dynamic_cast<VSPipelineFilter*>(parentFilter);
  // Reload file data
  if(fileFilter != nullptr)
  {
    QString filePath = fileFilter->getFilePath();

    QSharedPointer<SIMPLH5DataReader> reader = QSharedPointer<SIMPLH5DataReader>(new SIMPLH5DataReader());
    connect(reader.data(), SIGNAL(errorGenerated(const QString&, const QString&, const int&)), this, SIGNAL(errorGenerated(const QString&, const QString&, const int&)));

    bool success = reader->openFile(filePath);
    if(success)
    {
      int err = 0;
      DataContainerArrayProxy dcaProxy = reader->readDataContainerArrayStructure(nullptr, err);
	  QMap<QString, DataContainerProxy>& dataContainers = dcaProxy.getDataContainers();
      QStringList dcNames = dataContainers.keys();
      if(dcNames.contains(dcName))
      {
        DataContainerProxy dcProxy = dataContainers.value(dcName);
        if(dcProxy.getDCType() != static_cast<unsigned int>(IGeometry::Type::Unknown))
        {
          QString dcName = m_DCValues->getWrappedDataContainer()->m_Name;
          DataContainerProxy dcProxy = dataContainers.value(dcName);

          AttributeMatrixProxy::AMTypeFlags amFlags(AttributeMatrixProxy::AMTypeFlag::Cell_AMType);
          DataArrayProxy::PrimitiveTypeFlags pFlags(DataArrayProxy::PrimitiveTypeFlag::Any_PType);
          DataArrayProxy::CompDimsVector compDimsVector;

          dcProxy.setFlags(Qt::Checked, amFlags, pFlags, compDimsVector);
          dataContainers[dcProxy.getName()] = dcProxy;

          DataContainerArray::Pointer dca = reader->readSIMPLDataUsingProxy(dcaProxy, false);
          DataContainer::Pointer dc = dca->getDataContainer(m_DCValues->getWrappedDataContainer()->m_Name);

          m_WrappingWatcher.setFuture(QtConcurrent::run(this, &VSSIMPLDataContainerFilter::reloadData, dc));
        }
        else
        {
          QString ss = QObject::tr("Data Container '%1' could not be reloaded because it has an unknown data container geometry.").arg(dcName).arg(filePath);
          emit errorGenerated("Data Reload Error", ss, -3003);
          return;
        }
      }
      else
      {
        QString ss = QObject::tr("Data Container '%1' could not be reloaded because it no longer exists in the underlying file '%2'.").arg(dcName).arg(filePath);
        emit errorGenerated("Data Reload Error", ss, -3001);
        return;
      }
    }
  }
  // Reload pipeline data
  else if(pipelineFilter != nullptr)
  {
    FilterPipeline::Pointer pipeline = pipelineFilter->getFilterPipeline();
    DataContainerArray::Pointer dca = pipeline->getDataContainerArray();

    if(nullptr == dca)
    {
      QString ss = QObject::tr("The DataContainerArray '%1' could not be accessed because it no longer exists in the underlying pipeline '%1'.").arg(pipelineFilter->getPipelineName());
      emit errorGenerated("Data Reload Error", ss, -3000);
      return;
    }

    DataContainer::Pointer dc = dca->getDataContainer(m_DCValues->getWrappedDataContainer()->m_Name);
    if(nullptr == dc)
    {
      QString ss = QObject::tr("Data Container '%1' could not be reloaded because it no longer exists in the underlying pipeline '%2'.").arg(dcName).arg(pipelineFilter->getPipelineName());
      emit errorGenerated("Data Reload Error", ss, -3001);
      return;
    }

    IGeometry::Pointer geom = dc->getGeometry();
    if(nullptr == geom)
    {
      QString ss = QObject::tr("Data Container '%1' could not be reloaded because it does not have a data container geometry.").arg(dcName).arg(pipelineFilter->getPipelineName());
      emit errorGenerated("Data Reload Error", ss, -3002);
      return;
    }
    else if(geom->getGeometryType() == IGeometry::Type::Unknown)
    {
      QString ss = QObject::tr("Data Container '%1' could not be reloaded because it has an unknown data container geometry.").arg(dcName).arg(pipelineFilter->getPipelineName());
      emit errorGenerated("Data Reload Error", ss, -3003);
      return;
    }

    m_WrappingWatcher.setFuture(QtConcurrent::run(this, &VSSIMPLDataContainerFilter::reloadData, dc));
  }
  // No known reload method
  else
  {
    QString ss = QObject::tr("Data Container '%1' could not be reloaded because it does not have a file filter parent.").arg(dcName);
    emit errorGenerated("Data Reload Error", ss, -3002);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerFilter::reloadData(DataContainer::Pointer dc)
{
  m_DCValues->setWrappedDataContainer(SIMPLVtkBridge::WrapDataContainerAsStruct(dc));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerFilter::reloadWrappingFinished()
{
  VTK_PTR(vtkDataSet) dataSet = m_DCValues->getWrappedDataContainer()->m_DataSet;
  dataSet->ComputeBounds();

  vtkCellData* cellData = dataSet->GetCellData();
  if(cellData)
  {
    vtkDataArray* dataArray = cellData->GetArray(0);
    if(dataArray)
    {
      char* name = dataArray->GetName();
      cellData->SetActiveScalars(name);
    }
  }

  m_TrivialProducer->SetOutput(dataSet);

  emit updatedOutputPort(this);
  emit dataReloaded();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSSIMPLDataContainerFilter::getFilterName() const
{
  return m_DCValues->getWrappedDataContainer()->m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSSIMPLDataContainerFilter::getToolTip() const
{
  return m_DCValues->getWrappedDataContainer()->m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerFilter::apply()
{
  // finishWrapping aquires the apply lock and emits a signal that calls apply().
  // finishWrapping will not execute a second time until reaching the end of apply()
  // where the apply lock is released.
  if(finishWrapping())
  {
    emit dataImported();
  }
  m_ApplyLock.release();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSIMPLDataContainerFilter::finishWrapping()
{
  // Do not lock the main thread trying to apply a filter that is already being applied.
  if(m_ApplyLock.tryAcquire())
  {
    SIMPLVtkBridge::FinishWrappingDataContainerStruct(m_DCValues->getWrappedDataContainer());
    m_DCValues->setFullyWrapped(true);
    VTK_PTR(vtkDataSet) dataSet = m_DCValues->getWrappedDataContainer()->m_DataSet;
    m_TrivialProducer->SetOutput(dataSet);
    emit finishedWrapping();
    emit arrayNamesChanged();
    emit updatedOutputPort(this);
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSIMPLDataContainerFilter::dataFullyLoaded()
{
  return m_DCValues->isFullyWrapped();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilterValues* VSSIMPLDataContainerFilter::getValues()
{
  return m_DCValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr VSSIMPLDataContainerFilter::getWrappedDataContainer() const
{
  return m_DCValues->getWrappedDataContainer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerFilter::setWrappedDataContainer(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc)
{
  m_DCValues->setWrappedDataContainer(wrappedDc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSIMPLDataContainerFilter::CompatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  if(dynamic_cast<VSFileNameFilter*>(filter))
  {
    return true;
  }
  if(dynamic_cast<VSPipelineFilter*>(filter))
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSSIMPLDataContainerFilter::getTransformBounds()
{
	if(nullptr == getTransformFilter())
	{
		return getBounds();
	}
	VTK_PTR(vtkDataSet) outputData = getOutput();
	VTK_PTR(vtkImageData) imageData = dynamic_cast<vtkImageData*>(outputData.Get());

	// Subsample the image to reduce amount of data stored in transform filter
	VTK_PTR(vtkExtractVOI) subsample = VTK_PTR(vtkExtractVOI)::New();
	int* inputDims = imageData->GetDimensions();
	subsample->SetInputData(imageData);
	subsample->SetSampleRate(20, 20, 20);
	subsample->ReleaseDataFlagOn();
	subsample->Update();

	VTK_PTR(vtkTransformFilter) trans = getTransformFilter();
	trans->SetInputConnection(subsample->GetOutputPort());
	trans->SetTransform(getTransform()->getGlobalTransform());
	trans->ReleaseDataFlagOn();
	trans->Update();
	return trans->GetOutput()->GetBounds();
}
