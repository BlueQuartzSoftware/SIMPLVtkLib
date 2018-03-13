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

#pragma once

#include <vtkMappedUnstructuredGrid.h>
#include <vtkIdTypeArray.h>

#include "SIMPLib/Geometry/VertexGeom.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
* @class VtkVertexGeom VtkVertexGeom.h SIMPLVtkLib/SIMPLBridge/VtkVertexGeom.h
* @brief This class is used as an implementation class for vtkMappedUnstructuredGrid to
* be used with DREAM.3D's VertexGeom.  The implementation maps the cell and point IDs
* from the DREAM.3D geometry but the vertex points must be copied into the
* unstructured grid separately.
*/
class SIMPLVtkLib_EXPORT VSVertexGeom : public vtkObject
{
public:
  static VSVertexGeom* New();
  void 	PrintSelf(ostream &os, vtkIndent indent) override;
  vtkTypeMacro(VSVertexGeom, vtkObject)

  /**
  * @brief Sets the DREAM.3D geometry
  * @param geom
  */
  void SetGeometry(VertexGeom::Pointer geom);

  /**
  * @brief Returns the number of cells in the geometry
  * @return
  */
  vtkIdType GetNumberOfCells();

  /**
  * @brief Returns the cell type for the given cell ID
  * @param cellId
  * @return
  */
  int GetCellType(vtkIdType cellId);

  /**
  * @brief Gets a list of point IDs used by the cell ID
  * @param cellId
  * @param ptIds
  */
  void GetCellPoints(vtkIdType cellId, vtkIdList *ptIds);

  /**
  * @brief Gets a list of cell IDs that use the given point ID
  * @param ptId
  * @param cellIds
  */
  void GetPointCells(vtkIdType ptId, vtkIdList *cellIds);

  /**
  * @brief Returns the maximum cell size
  * @return
  */
  int GetMaxCellSize();

  /**
  * @brief Gets a list of all cell IDs of a given type
  * @param type
  * @param array
  */
  void GetIdsOfCellsOfType(int type, vtkIdTypeArray *array);

  /**
  * @brief Returns whether or not all cells are of the same type
  * @return
  */
  int IsHomogeneous();

  /**
  * @brief Required by vtkMappedUnstructuredGrid but should not be called on this read-only implementation
  * @param numCells
  * @param extSize
  */
  void Allocate(vtkIdType numCells, int extSize = 1000);

  /**
  * @brief Required by vtkMappedUnstructuredGrid but should not be called on this read-only implementation
  * @param type
  * @param ptIds
  * @return
  */
  vtkIdType InsertNextCell(int type, vtkIdList *ptIds);

  /**
  * @brief Required by vtkMappedUnstructuredGrid but should not be called on this read-only implementation
  * @param type
  * @param npts
  * @param ptIds
  * @return
  */
  vtkIdType InsertNextCell(int type, vtkIdType npts, vtkIdType *ptIds);

  /**
  * @brief Required by vtkMappedUnstructuredGrid but should not be called on this read-only implementation
  * @param type
  * @param npts
  * @param ptIds
  * @param nfaces
  * @param faces
  * @return
  */
  vtkIdType InsertNextCell(int type, vtkIdType npts, vtkIdType *ptIds, vtkIdType nfaces, vtkIdType *faces);

  /**
  * @brief Required by vtkMappedUnstructuredGrid but should not be called on this read-only implementation
  * @param cellId
  * @param npts
  * @param pts
  */
  void ReplaceCell(vtkIdType cellId, int npts, vtkIdType *pts);

protected:
  /**
  * @brief Default constructor
  */
  VSVertexGeom();

private:
  VertexGeom::Pointer m_Geom = nullptr;
  float m_MaxCellSize = 0.0f;
};

vtkMakeMappedUnstructuredGrid(VSVertexGrid, VSVertexGeom)
