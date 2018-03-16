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

#include <QtCore/QUuid>

#include <vtkAlgorithmOutput.h>

#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSIMPLDataContainerFilter::VSSIMPLDataContainerFilter(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDataContainer, VSAbstractFilter *parent)
: VSAbstractDataFilter()
, m_WrappedDataContainer(wrappedDataContainer)
, m_ApplyLock(1)
{
  createFilter();

  setText(wrappedDataContainer->m_Name);
  setToolTip(getToolTip());
  setParentFilter(parent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSSIMPLDataContainerFilter::getBounds() const
{
  return m_WrappedDataContainer->m_DataSet->GetBounds();
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
VTK_PTR(vtkDataSet) VSSIMPLDataContainerFilter::getOutput()
{
  if(nullptr == m_WrappedDataContainer)
  {
    return nullptr;
  }

  return m_WrappedDataContainer->m_DataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSIMPLDataContainerFilter* VSSIMPLDataContainerFilter::Create(const QString &filePath, QJsonObject &json, VSAbstractFilter* parent)
{
  QString dcName = json["Data Container Name"].toString();

  // Read in the data from the file and initialize the filter
  SIMPLH5DataReader reader;
  bool success = reader.openFile(filePath);
  if (success)
  {
    int err = 0;
    DataContainerArrayProxy proxy = reader.readDataContainerArrayStructure(nullptr, err);
    for (QMap<QString,DataContainerProxy>::iterator dcIter = proxy.dataContainers.begin(); dcIter != proxy.dataContainers.end(); dcIter++)
    {
      if (dcIter.key() == dcName)
      {
        DataContainerProxy dcProxy = dcIter.value();
        dcProxy.flag = Qt::Checked;

        for (QMap<QString,AttributeMatrixProxy>::iterator amIter = dcProxy.attributeMatricies.begin(); amIter != dcProxy.attributeMatricies.end(); amIter++)
        {
          AttributeMatrixProxy amProxy = amIter.value();

          if (amProxy.amType == AttributeMatrix::Type::Cell)
          {
            amProxy.flag = Qt::Checked;
          }

          for (QMap<QString,DataArrayProxy>::iterator daIter = amProxy.dataArrays.begin(); daIter != amProxy.dataArrays.end(); daIter++)
          {
            DataArrayProxy daProxy = daIter.value();
            daProxy.flag = Qt::Checked;

            amProxy.dataArrays[daProxy.name] = daProxy;
          }

          dcProxy.attributeMatricies[amProxy.name] = amProxy;
        }

        proxy.dataContainers[dcProxy.name] = dcProxy;

        DataContainerArray::Pointer dca = reader.readSIMPLDataUsingProxy(proxy, false);
        DataContainerShPtr dc = dca->getDataContainer(dcName);
        if (dc)
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
void VSSIMPLDataContainerFilter::writeJson(QJsonObject &json)
{
  VSAbstractFilter::writeJson(json);

  json["Data Container Name"] = text();
  json["Tooltip"] = toolTip();
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
  VTK_PTR(vtkDataSet) dataSet = m_WrappedDataContainer->m_DataSet;
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
const QString VSSIMPLDataContainerFilter::getFilterName()
{
  return m_WrappedDataContainer->m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSSIMPLDataContainerFilter::getToolTip() const
{
  return m_WrappedDataContainer->m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerFilter::apply()
{
  // Do not lock the main thread trying to apply a filter that is already being applied.
  if(m_ApplyLock.tryAcquire())
  {
    SIMPLVtkBridge::FinishWrappingDataContainerStruct(m_WrappedDataContainer);
    m_TrivialProducer->SetOutput(m_WrappedDataContainer->m_DataSet);
    m_ApplyLock.release();

    emit dataImported();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr VSSIMPLDataContainerFilter::getWrappedDataContainer()
{
  return m_WrappedDataContainer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSIMPLDataContainerFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  if(dynamic_cast<VSFileNameFilter*>(filter))
  {
    return true;
  }

  return false;
}
