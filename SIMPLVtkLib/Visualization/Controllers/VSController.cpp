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

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::VSController(QObject* parent)
  : QObject(parent)
  , m_FilterModel(new VSFilterModel())
{
  connect(m_FilterModel, SIGNAL(filterAdded(VSAbstractFilter*)), this, SIGNAL(filterAdded(VSAbstractFilter*)));
  connect(m_FilterModel, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SIGNAL(filterRemoved(VSAbstractFilter*)));
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
void VSController::importData(DataContainerArray::Pointer dca)
{
  std::vector<SIMPLVtkBridge::WrappedDataContainerPtr> wrappedData = SIMPLVtkBridge::WrapDataContainerArrayAsStruct(dca);
  
  // Add VSDataSetFilter for each DataContainer with relevant data
  size_t count = wrappedData.size();
  for(size_t i = 0; i < count; i++)
  {
    VSDataSetFilter* filter = new VSDataSetFilter(wrappedData[i]);
    m_FilterModel->addFilter(filter);
  }

  emit dataImported();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importData(DataContainer::Pointer dc)
{
  SIMPLVtkBridge::WrappedDataContainerPtr wrappedData = SIMPLVtkBridge::WrapDataContainerAsStruct(dc);

  // Add VSDataSetFilter if the DataContainer contains relevant data for rendering
  if(wrappedData)
  {
    VSDataSetFilter* filter = new VSDataSetFilter(wrappedData);
    m_FilterModel->addFilter(filter);
  }

  emit dataImported();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewController* VSController::getActiveViewController()
{
  return m_ActiveViewController;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::setActiveViewController(VSViewController* activeView)
{
  if(m_ActiveViewController != activeView)
  {
    m_ActiveViewController = activeView;

    emit activeViewChanged(m_ActiveViewController);
  }
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
