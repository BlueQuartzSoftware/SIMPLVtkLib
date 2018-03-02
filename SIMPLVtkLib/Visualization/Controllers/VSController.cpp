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

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QFile>
#include <QtCore/QUuid>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::VSController(QObject* parent)
  : QObject(parent)
  , m_FilterModel(new VSFilterModel())
{
  connect(m_FilterModel, SIGNAL(filterAdded(VSAbstractFilter*, bool)), this, SIGNAL(filterAdded(VSAbstractFilter*, bool)));
  connect(m_FilterModel, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SIGNAL(filterRemoved(VSAbstractFilter*)));

  m_ImportObject = new VSConcurrentImport(this);
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
  m_ImportObject->addDataContainerArray(filePath, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainerArray(DataContainerArray::Pointer dca)
{
  m_ImportObject->addDataContainerArray("No File", dca);
  m_ImportObject->run();

  //emit dataImported();
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importData(const QString &filePath)
{
  VSDataSetFilter* filter = new VSDataSetFilter(filePath);
  // Check if any data was imported
  if(filter->getOutput())
  {
    VSFileNameFilter* textFilter = new VSFileNameFilter(filePath);
    m_FilterModel->addFilter(textFilter, false);

    filter->setParentFilter(textFilter);
    m_FilterModel->addFilter(filter);

    emit dataImported();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::saveSession(const QString &sessionFilePath)
{
  QFile outputFile(sessionFilePath);
  if(outputFile.open(QIODevice::WriteOnly))
  {
    QJsonObject rootObj;

    QVector<VSAbstractFilter*> baseFilters = getBaseFilters();
    for (int i = 0; i < baseFilters.size(); i++)
    {
      VSAbstractFilter* filter = baseFilters[i];

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
void VSController::saveFilter(VSAbstractFilter* filter, QJsonObject &obj)
{
  QJsonObject rootFilterObj;

  filter->writeJson(rootFilterObj);

  // Write the children
  QJsonObject childrenObj;

  QVector<VSAbstractFilter*> childFilters = filter->getChildren();
  for (int i = 0; i < childFilters.size(); i++)
  {
    VSAbstractFilter* childFilter = childFilters[i];
    saveFilter(childFilter, childrenObj);
  }

  rootFilterObj["Child Filters"] = childrenObj;

  obj[filter->getFilterName()] = rootFilterObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::loadSession(const QString &sessionFilePath)
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
  for (QJsonObject::iterator iter = rootObj.begin(); iter != rootObj.end(); iter++)
  {
    QJsonObject filterObj = iter.value().toObject();
    loadFilter(filterObj);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::loadFilter(QJsonObject &obj, VSAbstractFilter* parentFilter)
{
  QUuid uuid(obj["Uuid"].toString());

  VSAbstractFilter* newFilter = nullptr;
  if (uuid == VSClipFilter::GetUuid())
  {
    newFilter = VSClipFilter::Create(obj, parentFilter);
  }
  else if (uuid == VSCropFilter::GetUuid())
  {
    newFilter = VSCropFilter::Create(obj, parentFilter);
  }
  else if (uuid == VSDataSetFilter::GetUuid())
  {
    if (dynamic_cast<VSFileNameFilter*>(parentFilter) != nullptr)
    {
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
      QString filePath = fileNameFilter->getFilePath();

      newFilter = VSDataSetFilter::Create(filePath, obj, parentFilter);
    }
  }
  else if (uuid == VSFileNameFilter::GetUuid())
  {
    newFilter = VSFileNameFilter::Create(obj, parentFilter);
  }
  else if (uuid == VSMaskFilter::GetUuid())
  {
    newFilter = VSMaskFilter::Create(obj, parentFilter);
  }
  else if (uuid == VSSIMPLDataContainerFilter::GetUuid())
  {
    if (dynamic_cast<VSFileNameFilter*>(parentFilter) != nullptr)
    {
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
      QString filePath = fileNameFilter->getFilePath();

      newFilter = VSSIMPLDataContainerFilter::Create(filePath, obj, parentFilter);
    }
  }
  else if (uuid == VSSliceFilter::GetUuid())
  {
    newFilter = VSSliceFilter::Create(obj, parentFilter);
  }
  else if (uuid == VSTextFilter::GetUuid())
  {
    newFilter = VSTextFilter::Create(obj, parentFilter);
  }
  else if (uuid == VSThresholdFilter::GetUuid())
  {
    newFilter = VSThresholdFilter::Create(obj, parentFilter);
  }

  if (newFilter != nullptr)
  {
    QJsonObject childrenObj = obj["Child Filters"].toObject();

    m_FilterModel->addFilter(newFilter, (childrenObj.size() == 0));
    newFilter->setCheckState(static_cast<Qt::CheckState>(obj["CheckState"].toInt()));
    emit filterCheckStateChanged(newFilter);

    for (QJsonObject::iterator iter = childrenObj.begin(); iter != childrenObj.end(); iter++)
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
  QVector<VSAbstractFilter*> baseFilters = getBaseFilters();
  int count = baseFilters.size();

  for(int i = 0; i < count; i++)
  {
    VSFileNameFilter* filter = dynamic_cast<VSFileNameFilter*>(baseFilters[i]);
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
QVector<VSAbstractFilter*> VSController::getBaseFilters()
{
  return m_FilterModel->getBaseFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractFilter*> VSController::getAllFilters()
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
