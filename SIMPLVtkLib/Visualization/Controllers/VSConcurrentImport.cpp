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

#include "VSConcurrentImport.h"

#include <QtConcurrent>

#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSConcurrentImport::VSConcurrentImport(VSController* controller)
: QObject()
, m_Controller(controller)
, m_ImportDataContainerOrderLock(1)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::addDataContainerArray(QString filePath, DataContainerArray::Pointer dca)
{
  VSFileNameFilter* fileFilter = new VSFileNameFilter(filePath);
  addDataContainerArray(std::make_pair(fileFilter, dca));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::addDataContainerArray(DcaFilePair fileDcPair)
{
  m_WrappedList.push_back(fileDcPair);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::run()
{
  while(m_WrappedList.size() > 0)
  {
    DcaFilePair filePair = m_WrappedList.front();
    importDataContainerArray(filePair);

    m_WrappedList.pop_front();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::importDataContainerArray(DcaFilePair filePair)
{
  VSFileNameFilter* fileFilter = filePair.first;
  DataContainerArray::Pointer dca = filePair.second;
  m_Controller->getFilterModel()->addFilter(fileFilter);

  m_ImportDataContainerOrder = dca->getDataContainers();

  size_t threadCount = QThreadPool::globalInstance()->maxThreadCount();
  if(threadCount > 1)
  {
    threadCount--;
  }
  for (int i = 0; i < threadCount; i++)
  {
    QFuture<void> future = QtConcurrent::run(this, &VSConcurrentImport::importDataContainer, fileFilter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::importDataContainer(VSFileNameFilter* fileFilter)
{
  while (m_ImportDataContainerOrder.size() > 0)
  {
    if (m_ImportDataContainerOrderLock.tryAcquire() == true)
    {
      DataContainer::Pointer dc = m_ImportDataContainerOrder.front();
      m_ImportDataContainerOrder.pop_front();

      m_ImportDataContainerOrderLock.release();

      SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc = SIMPLVtkBridge::WrapGeometryPtr(dc);
      if(wrappedDc)
      {
        importWrappedDataContainer(fileFilter, wrappedDc);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::importWrappedDataContainer(VSFileNameFilter* fileFilter, SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc)
{
  VSSIMPLDataContainerFilter* filter = new VSSIMPLDataContainerFilter(wrappedDc, fileFilter);
  m_Controller->getFilterModel()->addFilter(filter);
}
