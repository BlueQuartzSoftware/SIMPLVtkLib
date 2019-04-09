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

#include "ImporterWorker.h"

#include <QtCore/QCoreApplication>

#include <iostream>

#include "QtWidgets/VSQueueItem.h"
#include "QtWidgets/VSQueueModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImporterWorker::ImporterWorker()
: m_QueueModel(nullptr)
, m_ImportSem(1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImporterWorker::~ImporterWorker()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::setQueueModel(VSQueueModel* queueModel)
{
  m_QueueModel = queueModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::cancelWorker()
{
  m_Cancelled = true;
  m_CurrentImporter->cancel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::process()
{
  m_QueueModel->startQueue();

  m_Cancelled = false;

  m_CurrentIndex = m_QueueModel->index(0, VSQueueItem::ItemData::Contents);

  m_ImportSem.acquire();
  while(m_CurrentIndex.isValid() && m_CurrentIndex.row() < m_QueueModel->rowCount())
  {
    m_ImportSem.release();

    QCoreApplication::processEvents();

    if(m_Cancelled)
    {
      emit finished();
      return;
    }

    m_CurrentImporter = m_QueueModel->data(m_CurrentIndex, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();

    if(m_CurrentImporter && m_CurrentImporter->getState() == VSAbstractImporter::State::Ready)
    {
      m_CurrentImporter->execute();
    }

    m_ImportSem.acquire();
    m_CurrentIndex = m_QueueModel->index(m_CurrentIndex.row() + 1, VSQueueItem::ItemData::Contents);
  }
  m_ImportSem.release();

  emit finished();
}
