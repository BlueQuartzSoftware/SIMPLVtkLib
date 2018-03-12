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

#include "SIMPLVtkBridge.h"

#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
#include <vtkCharArray.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkLine.h>
#include <vtkLongLongArray.h>
#include <vtkLookupTable.h>
#include <vtkMappedUnstructuredGrid.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkRectilinearGrid.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkShortArray.h>
#include <vtkStructuredPoints.h>
#include <vtkTextProperty.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkUnsignedLongLongArray.h>
#include <vtkUnsignedShortArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>

#include "SIMPLVtkLib/SIMPLBridge/VSTetrahedralGeom.h"
#include "SIMPLVtkLib/SIMPLBridge/VSTriangleGeom.h"
#include "SIMPLVtkLib/SIMPLBridge/VSEdgeGeom.h"
#include "SIMPLVtkLib/SIMPLBridge/VSVertexGeom.h"
#include "SIMPLVtkLib/SIMPLBridge/VSQuadGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::SIMPLVtkBridge()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtrCollection SIMPLVtkBridge::WrapDataContainerArrayAsStruct(DataContainerArray::Pointer dca, AttributeMatrix::Types types)
{
  SIMPLVtkBridge::WrappedDataContainerPtrCollection wrappedDataContainers;

  if(!dca)
  {
    return wrappedDataContainers;
  }

  QList<DataContainer::Pointer> dcs = dca->getDataContainers();

  for(QList<DataContainer::Pointer>::Iterator dc = dcs.begin(); dc != dcs.end(); ++dc)
  {
    WrappedDataContainerPtr wrappedDataContainer = WrapDataContainerAsStruct((*dc), types);

    if(wrappedDataContainer)
    {
      wrappedDataContainers.push_back(wrappedDataContainer);
    }
  }

  return wrappedDataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr SIMPLVtkBridge::WrapDataContainerAsStruct(DataContainer::Pointer dc, AttributeMatrix::Types types)
{
  if(!dc)
  {
    return nullptr;
  }
  if(!dc->getGeometry())
  {
    return nullptr;
  }
  VTK_PTR(vtkDataSet) dataSet = WrapGeometry(dc->getGeometry());
  if(!dataSet)
  {
    return nullptr;
  }
  else
  {
    WrappedDataContainerPtr wrappedDcStruct(new WrappedDataContainer());
    wrappedDcStruct->m_DataSet = dataSet;
    wrappedDcStruct->m_Name = dc->getName();

    DataContainer::AttributeMatrixMap_t attrMats = dc->getAttributeMatrices();

    for(DataContainer::AttributeMatrixMap_t::Iterator attrMat = attrMats.begin(); attrMat != attrMats.end(); ++attrMat)
    {
      if(!(*attrMat))
      {
        continue;
      }
      // For now, only support the requested AttributeMatrix types
      if(types.contains((*attrMat)->getType()))
      {
        wrappedDcStruct->m_CellData = WrapAttributeMatrixAsStructs((*attrMat));

        // Add vtkDataArrays to the vtkDataSet
        vtkCellData* cellData = dataSet->GetCellData();
        for(auto cellDataIter = wrappedDcStruct->m_CellData.begin(); cellDataIter != wrappedDcStruct->m_CellData.end(); cellDataIter++)
        {
          cellData->AddArray((*cellDataIter)->m_VtkArray);
        }

        // Create point data from cell data
        VTK_NEW(vtkCellDataToPointData, cell2Point);
        cell2Point->SetInputData(dataSet);
        cell2Point->PassCellDataOn();
        cell2Point->Update();
        
        vtkDataSet* pointDataSet = nullptr;
        switch(dataSet->GetDataObjectType())
        {
        case VTK_IMAGE_DATA:
          pointDataSet = cell2Point->GetImageDataOutput();
          break;
        case VTK_STRUCTURED_GRID:
          pointDataSet = cell2Point->GetUnstructuredGridOutput();
          break;
        case VTK_RECTILINEAR_GRID:
          pointDataSet = cell2Point->GetRectilinearGridOutput();
          break;
        case VTK_STRUCTURED_POINTS:
          pointDataSet = cell2Point->GetStructuredPointsOutput();
          break;
        case VTK_UNSTRUCTURED_GRID:
          pointDataSet = cell2Point->GetUnstructuredGridOutput();
          break;
        case VTK_POLY_DATA:
          pointDataSet = cell2Point->GetPolyDataOutput();
          break;
        default:
          pointDataSet = cell2Point->GetOutput();
          break;
        }
        dataSet->DeepCopy(pointDataSet);

        // Set the active cell / point data scalars
        if(cellData->GetNumberOfArrays() > 0)
        {
          cellData->SetActiveScalars(cellData->GetArray(0)->GetName());
        }
        vtkPointData* pointData = dataSet->GetPointData();
        if(pointData->GetNumberOfArrays() > 0)
        {
          pointData->SetActiveScalars(pointData->GetArray(0)->GetName());
        }
      }
    }

    return wrappedDcStruct;
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataArrayPtrCollection SIMPLVtkBridge::WrapAttributeMatrixAsStructs(AttributeMatrix::Pointer attrMat)
{
  WrappedDataArrayPtrCollection wrappedDataArrays;

  if(!attrMat)
  {
    return wrappedDataArrays;
  }

  QStringList arrayNames = attrMat->getAttributeArrayNames();

  for(QStringList::Iterator arrayName = arrayNames.begin(); arrayName != arrayNames.end(); ++arrayName)
  {
    IDataArray::Pointer array = attrMat->getAttributeArray((*arrayName));
    if(!array)
    {
      continue;
    }

    WrappedDataArrayPtr wrappedDataArray = WrapIDataArrayAsStruct(array);
    if(wrappedDataArray)
    {
      wrappedDataArrays.push_back(wrappedDataArray);
    }
  }

  return wrappedDataArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataArrayPtr SIMPLVtkBridge::WrapIDataArrayAsStruct(IDataArray::Pointer dataArray)
{
  if(!dataArray)
  {
    return nullptr;
  }

  VTK_PTR(vtkDataArray) vtkArray = WrapIDataArray(dataArray);
  if(!vtkArray)
  {
    return nullptr;
  }
  else
  {
    WrappedDataArrayPtr wrappedDataArray(new WrappedDataArray());

    QString arrayName = dataArray->getName();
    vtkArray->SetName(arrayName.toStdString().c_str());

    wrappedDataArray->m_ArrayName = arrayName;
    wrappedDataArray->m_SIMPLArray = dataArray;
    wrappedDataArray->m_VtkArray = vtkArray;

    return wrappedDataArray;
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataArray) SIMPLVtkBridge::WrapVertices(SharedVertexList::Pointer vertexArray)
{
  VTK_NEW(vtkFloatArray, vtkArray);
  vtkArray->SetNumberOfComponents(vertexArray->getNumberOfComponents());
  vtkArray->SetNumberOfTuples(vertexArray->getNumberOfTuples());

  vtkArray->SetVoidArray(vertexArray->getVoidPointer(0), vertexArray->getSize(), 1);

  return vtkArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(EdgeGeom::Pointer geom)
{
  VTK_NEW(VSEdgeGrid, dataSet);
  VSEdgeGeom* edgeGeom = dataSet->GetImplementation();
  edgeGeom->SetGeometry(geom);

  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(QuadGeom::Pointer geom)
{
  VTK_NEW(VSQuadGrid, dataSet);
  VSQuadGeom* quadGeom = dataSet->GetImplementation();
  quadGeom->SetGeometry(geom);
  
  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(RectGridGeom::Pointer geom)
{
  // vtkRectilinearGrid requires 3 arrays with the x, y, and z coordinates of points
  VTK_PTR(vtkDataArray) xCoords = WrapIDataArray(geom->getXBounds());
  VTK_PTR(vtkDataArray) yCoords = WrapIDataArray(geom->getYBounds());
  VTK_PTR(vtkDataArray) zCoords = WrapIDataArray(geom->getZBounds());

  int x = xCoords->GetNumberOfTuples();
  int y = yCoords->GetNumberOfTuples();
  int z = zCoords->GetNumberOfTuples();

  // Create the vtkRectilinearGrid and apply values
  VTK_NEW(vtkRectilinearGrid, vtkRectGrid);
  vtkRectGrid->SetXCoordinates(xCoords);
  vtkRectGrid->SetYCoordinates(yCoords);
  vtkRectGrid->SetZCoordinates(zCoords);
  vtkRectGrid->SetExtent(0, x, 0, y, 0, z);
  vtkRectGrid->SetDimensions(x, y, z);

  return vtkRectGrid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(TetrahedralGeom::Pointer geom)
{
  VTK_NEW(VSTetrahedralGrid, dataSet);
  VSTetrahedralGeom* tetGeom = dataSet->GetImplementation();
  tetGeom->SetGeometry(geom);
  
  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(TriangleGeom::Pointer geom)
{
  VTK_NEW(VtkTriangleGrid, dataSet);
  VSTriangleGeom* triGeom = dataSet->GetImplementation();
  triGeom->SetGeometry(geom);
  
  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(VertexGeom::Pointer geom)
{
  VTK_NEW(VSVertexGrid, dataSet);
  VSVertexGeom* vertGeom = dataSet->GetImplementation();
  vertGeom->SetGeometry(geom);
  
  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(ImageGeom::Pointer image)
{
  size_t dims[3] = {0, 0, 0};
  float res[3] = {0.0f, 0.0f, 0.0f};
  float origin[3] = {0.0f, 0.0f, 0.0f};

  image->getDimensions(dims);
  image->getResolution(res);
  image->getOrigin(origin);

  VTK_NEW(vtkImageData, vtkImage);
  vtkImage->SetExtent(0, dims[0], 0, dims[1], 0, dims[2]);
  vtkImage->SetDimensions(dims[0] + 1, dims[1] + 1, dims[2] + 1);
  vtkImage->SetSpacing(res[0], res[1], res[2]);
  vtkImage->SetOrigin(origin[0], origin[1], origin[2]);

  return vtkImage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(IGeometry::Pointer geom)
{
  if(std::dynamic_pointer_cast<EdgeGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<EdgeGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<ImageGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<ImageGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<QuadGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<QuadGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<RectGridGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<RectGridGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<TetrahedralGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<TetrahedralGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<TriangleGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<TriangleGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<VertexGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<VertexGeom>(geom));
  }

  // Default to nullptr if the type does not match a supported geometry
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataArray) SIMPLVtkBridge::WrapIDataArray(IDataArray::Pointer array)
{
  if(std::dynamic_pointer_cast<UInt8ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkUnsignedCharArray>(array);
  }
  else if(std::dynamic_pointer_cast<Int8ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkCharArray>(array);
  }
  else if(std::dynamic_pointer_cast<UInt16ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkUnsignedShortArray>(array);
  }
  else if(std::dynamic_pointer_cast<Int16ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkShortArray>(array);
  }
  else if(std::dynamic_pointer_cast<UInt32ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkUnsignedIntArray>(array);
  }
  else if(std::dynamic_pointer_cast<Int32ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkIntArray>(array);
  }
  else if(std::dynamic_pointer_cast<UInt64ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkUnsignedLongLongArray>(array);
  }
  else if(std::dynamic_pointer_cast<Int64ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkLongLongArray>(array);
  }
  else if(std::dynamic_pointer_cast<FloatArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkFloatArray>(array);
  }
  else if(std::dynamic_pointer_cast<DoubleArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkDoubleArray>(array);
  }
  else
  {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//VSRenderController::Pointer SIMPLVtkBridge::WrapImageGeomAsVtkImageData(ImageGeom::Pointer image, FloatArrayType::Pointer cellData)
//{
//  size_t dims[3] = {0, 0, 0};
//  float res[3] = {0.0f, 0.0f, 0.0f};
//  float origin[3] = {0.0f, 0.0f, 0.0f};
//
//  image->getDimensions(dims);
//  image->getResolution(res);
//  image->getOrigin(origin);
//
//  VTK_NEW(vtkImageData, vtk_image);
//  vtk_image->SetExtent(0, dims[0], 0, dims[1], 0, dims[2]);
//  vtk_image->SetDimensions(dims[0] + 1, dims[1] + 1, dims[2] + 1);
//  vtk_image->SetSpacing(res[0], res[1], res[2]);
//  vtk_image->SetOrigin(origin[0], origin[1], origin[2]);
//
//  VTK_NEW(vtkFloatArray, flt);
//  flt->SetVoidArray(cellData->getVoidPointer(0), cellData->getSize(), 1);
//
//  vtk_image->GetCellData()->SetScalars(flt);
//
//  std::shared_ptr<VSRenderController::VtkDataViewStruct_t> vtkDataViewStruct(new VSRenderController::VtkDataViewStruct_t());
//  vtkDataViewStruct->Name = cellData->getName();
//
//  std::shared_ptr<VSRenderController::VtkDataSetStruct_t> vtkDataSetStruct(new VSRenderController::VtkDataSetStruct_t());
//  vtkDataSetStruct->DataSet = vtk_image;
//  vtkDataSetStruct->Name = image->getName();
//  vtkDataSetStruct->ViewOptions.push_back(vtkDataViewStruct);
//
//  VSRenderController::VtkDataStructVector_t vtkDataStructVector;
//  vtkDataStructVector.push_back(vtkDataSetStruct);
//
//  VSRenderController::Pointer vtkDataBlob = VSRenderController::CreateVSRenderController(vtkDataStructVector);
//
//  return vtkDataBlob;
//}
