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
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importData(DataContainerArray::Pointer dca)
{
  std::vector<SIMPLVtkBridge::WrappedDataContainerPtr> wrappedData = SIMPLVtkBridge::WrapDataContainerArrayAsStruct(dca);

  // TODO: Add VSDataSetFilter for each DataContainer with relevant data
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importData(DataContainer::Pointer dc)
{
  SIMPLVtkBridge::WrappedDataContainerPtr wrappedData = SIMPLVtkBridge::WrapDataContainerAsStruct(dc);

  // TODO: Add VSDataSetFilter if the DataContainer contains relevant data for rendering
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::updateData(DataContainerArray::Pointer dca)
{
  std::vector<SIMPLVtkBridge::WrappedDataContainerPtr> wrappedData = SIMPLVtkBridge::WrapDataContainerArrayAsStruct(dca);

  // TODO: Check if any of the DataContainers are already imported as VSDataSetFilters
  // TODO: Update any DataContainers already imported
  // TODO: Import any DataContainers that are not already imported
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::updateData(DataContainer::Pointer dc)
{
  SIMPLVtkBridge::WrappedDataContainerPtr wrappedData = SIMPLVtkBridge::WrapDataContainerAsStruct(dc);

  // TODO: Check if the DataContainer is already imported as a VSDataSetFilter
  // TODO: Update the DataContainer if it is already imported
  // TODO: Import the DataContainer if it is not already imported
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::removeData(DataContainerArray::Pointer dca)
{
  std::vector<SIMPLVtkBridge::WrappedDataContainerPtr> wrappedData = SIMPLVtkBridge::WrapDataContainerArrayAsStruct(dca);

  // TODO: Check if any of the DataContainers are already imported as VSDataSetFilters
  // TODO: Remove any of the DataContainers already imported
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::removeData(DataContainer::Pointer dc)
{
  SIMPLVtkBridge::WrappedDataContainerPtr wrappedData = SIMPLVtkBridge::WrapDataContainerAsStruct(dc);

  // TODO: Check if the DataContainer is already imported as a VSDataSetFilter
  // TODO: Remove the DataContainer if it is already imported
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
QVector<VSDataSetFilter*> VSController::getDataFilters()
{
  return m_DataFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractFilter*> VSController::getAllFilters()
{
  QVector<VSAbstractFilter*> filters(m_DataFilters.size());

  size_t count = m_DataFilters.size();
  for(int i = 0; i < count; i++)
  {
    filters.push_back(m_DataFilters[i]);
    filters.append(m_DataFilters[i]->getDescendants());
  }

  return filters;
}
