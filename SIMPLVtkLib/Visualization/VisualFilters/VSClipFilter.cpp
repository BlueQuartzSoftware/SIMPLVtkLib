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

#include "VSClipFilter.h"

#include <QApplication>
#include <QString>

#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkBoxRepresentation.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkPlanes.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTableBasedClipDataSet.h>
#include <vtkTransform.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

const QString VSClipFilter::PlaneClipTypeString = "Plane";
const QString VSClipFilter::BoxClipTypeString = "Box";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::VSClipFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_ClipAlgorithm = nullptr;
  setParentFilter(parent);

  for(int i = 0; i < 3; i++)
  {
    m_LastPlaneOrigin[i] = 0.0;
    m_LastPlaneNormal[i] = 0.0;

    m_LastBoxOrigin[i] = 0.0;
    m_LastBoxScale[i] = 1.0;
    m_LastBoxRotation[i] = 0.0;
  }

  // Set the direction of the plane normal
  m_LastPlaneNormal[0] = 1.0;
  
  m_LastClipType = VSClipFilter::ClipType::PLANE;
  m_LastPlaneInverted = false;
  m_LastBoxInverted = false;

  setText(getFilterName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::~VSClipFilter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::createFilter()
{
  m_ClipAlgorithm = vtkSmartPointer<vtkTableBasedClipDataSet>::New();
  m_ClipAlgorithm->SetInputConnection(m_ParentFilter->getOutputPort());
  m_ConnectedInput = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSClipFilter::getFilterName()
{
  return "Clip";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkPlanes) VSClipFilter::getBoxFunction(double origin[3], double scale[3], double rotation[3])
{
  VTK_NEW(vtkTransform, viewTransform);
  VTK_NEW(vtkMatrix4x4, matrix);
  viewTransform->SetMatrix(matrix);

  viewTransform->RotateZ(rotation[2]);
  viewTransform->RotateX(rotation[0]);
  viewTransform->RotateY(rotation[1]);

  viewTransform->Translate(origin);
  viewTransform->Scale(scale);

  VTK_NEW(vtkBoxRepresentation, boxRep);
  boxRep->SetTransform(viewTransform);

  vtkSmartPointer<vtkPlanes> planes = vtkSmartPointer<vtkPlanes>::New();
  boxRep->GetPlanes(planes);
  return planes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::apply(double origin[3], double normal[3], bool inverted)
{
  if(nullptr == m_ClipAlgorithm)
  {
    createFilter();
  }

  // Handle Plane-Type clips
  m_LastClipType = ClipType::PLANE;
  m_LastPlaneInverted = inverted;

  // Save the applied values for resetting Plane-Type widgets
  for(int i = 0; i < 3; i++)
  {
    m_LastPlaneOrigin[i] = origin[i];
    m_LastPlaneNormal[i] = normal[i];
  }

  VTK_NEW(vtkPlane, plane);
  plane->SetOrigin(origin);
  plane->SetNormal(normal);

  m_ClipAlgorithm->SetClipFunction(plane);
  m_ClipAlgorithm->SetInsideOut(inverted);
  m_ClipAlgorithm->Update();

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::apply(double origin[3], double scale[3], double rotation[3], bool inverted)
{
  if(nullptr == m_ClipAlgorithm)
  {
    createFilter();
  }

  // Handle Box-Type clips
  m_LastClipType = ClipType::BOX;
  m_LastBoxInverted = inverted;

  // Save the applied values for resetting Box-Type widgets
  for(int i = 0; i < 3; i++)
  {
    m_LastBoxOrigin[i] = origin[i];
    m_LastBoxScale[i] = scale[i];
    m_LastBoxRotation[i] = rotation[i];
  }

  VTK_PTR(vtkPlanes) box = getBoxFunction(origin, scale, rotation);

  m_ClipAlgorithm->SetClipFunction(box);
  m_ClipAlgorithm->SetInsideOut(inverted);
  m_ClipAlgorithm->Update();

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSClipFilter::getOutputPort()
{
  if(m_ConnectedInput && m_ClipAlgorithm)
  {
    return m_ClipAlgorithm->GetOutputPort();
  }
  else if(m_ParentFilter)
  {
    return m_ParentFilter->getOutputPort();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) VSClipFilter::getOutput()
{
  if(m_ConnectedInput && m_ClipAlgorithm)
  {
    return m_ClipAlgorithm->GetOutput();
  }
  else if(m_ParentFilter)
  {
    return m_ParentFilter->getOutput();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  m_InputPort = filter->getOutputPort();

  if(m_ConnectedInput && m_ClipAlgorithm)
  {
    m_ClipAlgorithm->SetInputConnection(filter->getOutputPort());
  }
  else
  {
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSClipFilter::getOutputType()
{
  return UNSTRUCTURED_GRID;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSClipFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipFilter::getLastPlaneInverted()
{
  return m_LastPlaneInverted;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipFilter::getLastPlaneOrigin()
{
  return m_LastPlaneOrigin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipFilter::getLastPlaneNormal()
{
  return m_LastPlaneNormal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipFilter::getLastBoxInverted()
{
  return m_LastBoxInverted;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipFilter::getLastBoxOrigin()
{
  return m_LastBoxOrigin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipFilter::getLastBoxScale()
{
  return m_LastBoxScale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipFilter::getLastBoxRotation()
{
  return m_LastBoxRotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSClipFilter::getLastClipTypeString()
{
  QString clipTypeStr = "";
  if (m_LastClipType == ClipType::PLANE)
  {
    clipTypeStr = PlaneClipTypeString;
  }
  else if (m_LastClipType == ClipType::BOX)
  {
    clipTypeStr = BoxClipTypeString;
  }

  return clipTypeStr;
}
