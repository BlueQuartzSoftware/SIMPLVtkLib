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
#include <QtCore/QFutureWatcher>

#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSConcurrentImport::VSConcurrentImport(VSController* controller)
: QObject(controller)
, m_Controller(controller)
, m_ImportDataContainerOrderLock(1)
, m_UnappliedDataFilterLock(1)
, m_FilterLock(1)
, m_WrappedDcLock(1)
{
  m_UnappliedDataFilters.clear();
  connect(this, SIGNAL(importedFilter(VSAbstractFilter*, bool)),
    controller->getFilterModel(), SLOT(addFilter(VSAbstractFilter*, bool)));

  int threadsUsed = 2;
  m_ThreadCount = QThreadPool::globalInstance()->maxThreadCount();
  if(m_ThreadCount > threadsUsed)
  {
    m_ThreadCount -= threadsUsed;
  }
  else
  {
    m_ThreadCount = 1;
  }
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
  m_FileNameFilter = filePair.first;
  DataContainerArray::Pointer dca = filePair.second;
  m_Controller->getFilterModel()->addFilter(m_FileNameFilter);

  m_ImportDataContainerOrder = dca->getDataContainers();

  emit blockRender(true);
  m_FilterLock.tryAcquire();
  m_ThreadsRemaining = m_ThreadCount;
  for(int i = 0; i < m_ThreadCount; i++)
  {
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    connect(watcher, SIGNAL(finished()), this, SLOT(partialWrappingThreadFinished()));
    watcher->setFuture(QtConcurrent::run(this, &VSConcurrentImport::importDataContainer, m_FileNameFilter));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::partialWrappingThreadFinished()
{
  m_ThreadsRemaining--;
  if(m_ThreadsRemaining <= 0)
  {
    for(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc : m_WrappedDataContainers)
    {
      VSSIMPLDataContainerFilter* filter = new VSSIMPLDataContainerFilter(wrappedDc, m_FileNameFilter);
      m_Controller->getFilterModel()->addFilter(filter, false);
      m_UnappliedDataFilters.push_back(filter);
    }
    emit blockRender(false);
    m_WrappedDataContainers.clear();
    m_FilterLock.release();

    // Apply filters on multiple threads
    for(int i = 0; i < m_ThreadCount; i++)
    {
      QFuture<void> future = QtConcurrent::run(this, &VSConcurrentImport::applyDataFilters);
    }
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
        while(m_WrappedDcLock.tryAcquire() == false)
        {
          QThread::currentThread()->wait(1);
        }
        m_WrappedDataContainers.push_back(wrappedDc);

        m_WrappedDcLock.release();
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::applyDataFilters()
{
  while(m_UnappliedDataFilters.size() > 0)
  {
    if(m_UnappliedDataFilterLock.tryAcquire() == true)
    {
      VSSIMPLDataContainerFilter* filter = m_UnappliedDataFilters.front();
      m_UnappliedDataFilters.pop_front();
      m_UnappliedDataFilterLock.release();

      filter->apply();
    }
  }
}
