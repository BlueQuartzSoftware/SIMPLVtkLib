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

#pragma once

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <string>

#include <vtkDataArray.h>
#include <vtkDataSet.h>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkImageData;
class vtkDataArray;
class vtkScalarsToColors;
class vtkScalarBarActor;

/**
 * @class SIMPLVtkBridge SIMPLVtkBridge.h SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h
 * @brief This class handles conversion of data from SIMPLib to VTK through the
 * wrapping of DataContainerArrays, DataContainers, AttributeMatrices, and DataArrays
 * to collections of vtkDataSets and vtkDataArrays.
 */
class SIMPLVtkLib_EXPORT SIMPLVtkBridge
{
public:
  struct WrappedDataArray
  {
    QString m_ArrayName;
    IDataArray::Pointer m_SIMPLArray;
    VTK_PTR(vtkDataArray) m_VtkArray;
  };

  using WrappedDataArrayPtr = std::shared_ptr<WrappedDataArray>;
  using WrappedDataArrayPtrCollection = std::vector<WrappedDataArrayPtr>;

  struct WrappedDataContainer
  {
    VTK_PTR(vtkDataSet) m_DataSet;
    WrappedDataArrayPtrCollection m_CellData;
    QString m_Name;
    DataContainer::Pointer m_DataContainer;
    double m_Origin[3] = { 0.0, 0.0, 0.0 };
  };

  using WrappedDataContainerPtr = std::shared_ptr<WrappedDataContainer>;
  using WrappedDataContainerPtrCollection = std::vector<WrappedDataContainerPtr>;

  /**
   * @brief Deconstructor
   */
  virtual ~SIMPLVtkBridge() = default;

  /**
   * @brief Wraps the DataContainers from SIMPLib's DataContainerArray in vtkDataSets where applicable
   * and returns a vector of WrappedDataContainerPtrs with information about the vtkDataSets.
   * Not all DataContainers can be wrapped in a vtkDataSet.
   * @param dca
   * @return
   */
  static WrappedDataContainerPtrCollection WrapDataContainerArrayAsStruct(DataContainerArray::Pointer dca, AttributeMatrix::Types types = AttributeMatrix::Types(1, AttributeMatrix::Type::Cell));

  /**
   * @brief Wraps a DataContainer from SIMPLib in a vtkDataSet if applicable and returns a WrappedDataContainerPtr
   * with information about the vtkDataSet including smart pointers to both the SIMPLib DataArrays and vtkDataArrays.
   * Because data is not copied from SIMPLib's DataArrays, the data will be lost if all references to its smart pointers
   * go out of scope.
   * @param dc
   * @param types
   * @return
   */
  static WrappedDataContainerPtr WrapDataContainerAsStruct(DataContainer::Pointer dc, AttributeMatrix::Types types = AttributeMatrix::Types(1, AttributeMatrix::Type::Cell));

  /**
   * @brief Wraps a DataContainer geometry from SIMPLib in a vtkDataSet if applicable and returns a WrappeddataContainerPtr
   * without any data arrays wrapped and imported. A nullptr is returned if there are no eligible AttributeMatrixes to wrap.
   * @param dc
   * @param types
   * @return
   */
  static WrappedDataContainerPtr WrapGeometryPtr(DataContainer::Pointer dc, AttributeMatrix::Types types = AttributeMatrix::Types(1, AttributeMatrix::Type::Cell));

  /**
   * @brief Finish wrapping the given DataContainer
   * @param wrappedDc
   */
  static void FinishWrappingDataContainerStruct(WrappedDataContainerPtr wrappedDc, AttributeMatrix::Types types = AttributeMatrix::Types(1, AttributeMatrix::Type::Cell));

  /**
   * @brief Wraps the DataArrays contained within SIMPLib's AttributeMatrix in vtkDataArrays for use in VTK
   * and returns a vector of structs containing information about those vtkDataArrays
   * @param am
   * @return
   */
  static WrappedDataArrayPtrCollection WrapAttributeMatrixAsStructs(AttributeMatrix::Pointer am);

  /**
   * @brief Wraps a DataArray from SIMPLib in a vtkDataArray and returns a struct containing the wrapped
   * DataArray's name and smart pointers to both the SIMPLib and VTK data.  The vtkDataArray does not copy
   * the SIMPLib data, meaning that the SIMPLib DataArray cannot go out of scope before the vtkDataArray
   * or the data will not be accessible.
   * @param da
   * @return
   */
  static WrappedDataArrayPtr WrapIDataArrayAsStruct(IDataArray::Pointer da);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's EdgeGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(EdgeGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's ImageGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(ImageGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's QuadGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(QuadGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's RectGridGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(RectGridGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's TetrahedralGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(TetrahedralGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's TriangleGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(TriangleGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's VertexGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(VertexGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's IGeometry
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(IGeometry::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataArray from SIMPLib's SharedVertexList
   * @param vertexArray
   * @return
   */
  static VTK_PTR(vtkDataArray) WrapVertices(SharedVertexList::Pointer vertexArray);

  /**
   * @brief Creates and returns a vtkDataArray from SIMPLib's IDataArray
   * @param array
   * @return
   */
  static VTK_PTR(vtkDataArray) WrapIDataArray(IDataArray::Pointer array);

  template <typename T> static VTK_PTR(T) WrapIDataArrayTemplate(IDataArray::Pointer array)
  {
    VTK_NEW(T, vtkArray);
    vtkArray->SetNumberOfComponents(array->getNumberOfComponents());
    vtkArray->SetNumberOfTuples(array->getNumberOfTuples());

    vtkArray->SetVoidArray(array->getVoidPointer(0), array->getSize(), 1);

    for(int i = 0; i < vtkArray->GetNumberOfComponents(); i++)
    {
      int strlen = std::strlen(array->getName().toStdString().c_str()) + ((i + 1) / 10 + 1) + 2;
      char* componentName = new char[strlen];
      std::strcpy(componentName, array->getName().toStdString().c_str());
      std::strcat(componentName, "_");
      std::strcat(componentName, std::to_string(i).c_str());

      vtkArray->SetComponentName(i, componentName);
    }

    return vtkArray;
  }

protected:
  /**
   * @brief Constructor
   */
  SIMPLVtkBridge();

private:
  SIMPLVtkBridge(const SIMPLVtkBridge&); // Copy Constructor Not Implemented
  void operator=(const SIMPLVtkBridge&); // Operator '=' Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif
