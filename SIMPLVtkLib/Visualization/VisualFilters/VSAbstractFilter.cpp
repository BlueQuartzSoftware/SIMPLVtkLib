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

#include "VSAbstractFilter.h"

#include <QString>

#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include <vtkGenericDataObjectWriter.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSAbstractWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::VSAbstractFilter()
: QObject()
, QStandardItem()
, m_InputPort(nullptr)
{
  setCheckable(true);
  setCheckState(Qt::Checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::~VSAbstractFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::deleteFilter()
{
  if(getParentFilter())
  {
    getParentFilter()->removeChild(this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getParentFilter() const
{
  VSAbstractFilter* parentFilter = dynamic_cast<VSAbstractFilter*>(QStandardItem::parent());
  return parentFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setParentFilter(VSAbstractFilter* parent)
{
  setParent(parent);
  if(parent)
  {
    parent->addChild(this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::addChild(VSAbstractFilter* child)
{
  connect(this, SIGNAL(updatedOutputPort(VSAbstractFilter*)), 
    child, SLOT(connectToOutuput(VSAbstractFilter*)), Qt::UniqueConnection);

  appendRow(child);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::removeChild(VSAbstractFilter* child)
{
  int row = getIndexOfChild(child);

  disconnect(this, SIGNAL(updatedOutputPort(VSAbstractFilter*)), child, SLOT(connectToOutuput(VSAbstractFilter*)));
  removeRow(row);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getAncestor()
{
  if(nullptr == getParentFilter())
  {
    return this;
  }

  return getParentFilter()->getAncestor();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractFilter*> VSAbstractFilter::getChildren() const
{
  int numRows = rowCount();
  QVector<VSAbstractFilter*> children(numRows);

  for(int i = 0; i < numRows; i++)
  {
    children[i] = dynamic_cast<VSAbstractFilter*>(child(i));
  }

  return children;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractFilter::getIndexOfChild(VSAbstractFilter* childFilter) const
{
  if(nullptr == childFilter)
  {
    return -1;
  }

  int numRows = rowCount();
  for(int i = 0; i < numRows; i++)
  {
    if(childFilter == dynamic_cast<VSAbstractFilter*>(child(i)))
    {
      return i;
    }
  }

  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractFilter*> VSAbstractFilter::getDescendants() const
{
  QVector<VSAbstractFilter*> descendants;
  QVector<VSAbstractFilter*> children = getChildren();

  for(VSAbstractFilter* filter : children)
  {
    descendants.push_back(filter);
    descendants.append(filter->getDescendants());
  }

  return descendants;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getChild(int index) const
{
  if(index < 0 || index >= rowCount())
  {
    return nullptr;
  }

  return getChildren()[index];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr VSAbstractFilter::getWrappedDataContainer()
{
  VSSIMPLDataContainerFilter* dataSetFilter = getDataSetFilter();

  if(nullptr == dataSetFilter)
  {
    return nullptr;
  }

  return dataSetFilter->getWrappedDataContainer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSAbstractFilter::getArrayNames()
{
  QStringList arrayNames;

  VTK_PTR(vtkDataSet) dataSet = getOutput();
  if(dataSet)
  {
    int numArrays = dataSet->GetCellData()->GetNumberOfArrays();
    for(int i = 0; i < numArrays; i++)
    {
      arrayNames.push_back(dataSet->GetCellData()->GetAbstractArray(i)->GetName());
    }
  }

  return arrayNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSAbstractFilter::getComponentList(QString arrayName)
{
  QStringList componentNames;

  VTK_PTR(vtkDataSet) dataSet = getOutput();
  if(dataSet)
  {
    const char* charName = arrayName.toLatin1();
    VTK_PTR(vtkAbstractArray) array = dataSet->GetCellData()->GetAbstractArray(charName);
    componentNames = getComponentList(array);
  }

  return componentNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSAbstractFilter::getComponentList(int arrayIndex)
{
  QStringList componentNames;

  VTK_PTR(vtkDataSet) dataSet = getOutput();
  if(dataSet)
  {
    VTK_PTR(vtkAbstractArray) array = dataSet->GetCellData()->GetAbstractArray(arrayIndex);
    componentNames = getComponentList(array);
  }

  return componentNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSAbstractFilter::getComponentList(vtkAbstractArray* array)
{
  QStringList componentNames;

  if(array)
  {
    int numComponent = array->GetNumberOfComponents();
    bool isCharArray = array->IsA("vtkCharArray");
    if(isCharArray && numComponent == 3)
    {
      componentNames.push_back("R");
      componentNames.push_back("G");
      componentNames.push_back("B");
    }
    else if(isCharArray && numComponent == 4)
    {
      componentNames.push_back("R");
      componentNames.push_back("G");
      componentNames.push_back("B");
      componentNames.push_back("A");
    }
    else
    {
      for(int i = 0; i < numComponent; i++)
      {
        componentNames.push_back("Comp_" + QString::number(i + 1));
      }
    }

    if(numComponent > 1)
    {
      componentNames.push_front("Magnitude");
    }
  }

  return componentNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractFilter::getBounds() const
{
  if(nullptr == getParentFilter())
  {
    return nullptr;
  }

  return getParentFilter()->getBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSIMPLDataContainerFilter* VSAbstractFilter::getDataSetFilter()
{
  VSSIMPLDataContainerFilter* cast = dynamic_cast<VSSIMPLDataContainerFilter*>(this);

  if(cast != nullptr)
  {
    return cast;
  }

  if(nullptr == getParentFilter())
  {
    return nullptr;
  }

  return getParentFilter()->getDataSetFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::compatibleInput(VSAbstractFilter::dataType_t inputType, VSAbstractFilter::dataType_t requiredType)
{
  if(requiredType == ANY_DATA_SET)
  {
    return true;
  }

  return requiredType == inputType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::saveFile(QString fileName)
{
  vtkDataSet* output = getOutput();

  VTK_NEW(vtkGenericDataObjectWriter, writer);

  writer->SetFileName(fileName.toStdString().c_str());
  writer->SetInputData(output);
  writer->Write();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::connectToOutuput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  m_InputPort = filter->getOutputPort();

  if(m_ConnectedInput)
  {
    // Connect algorithm input and filter output
    updateAlgorithmInput(filter);
  }
  else
  {
    // Emit only when the filter is not connected to its algorithm
    emit updatedOutputPort(filter);
  }
}
