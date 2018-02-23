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
#include <QtCore/QString>

#include <vtkUnstructuredGrid.h>

//const QString VSClipFilter::PlaneClipTypeString = "Plane";
//const QString VSClipFilter::BoxClipTypeString = "Box";

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
  }

  m_LastBoxTransform = VTK_PTR(vtkTransform)::New();

  // Set the direction of the plane normal
  m_LastPlaneNormal[0] = 1.0;
  
  m_LastClipType = VSClipFilter::ClipType::PLANE;
  m_LastPlaneInverted = false;
  m_LastBoxInverted = false;

  setText(getFilterName());
  setToolTip(getToolTip());
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
  m_ClipAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());
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
QString VSClipFilter::getToolTip() const
{
  return "Clip Filter";
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
void VSClipFilter::apply(VTK_PTR(vtkPlanes) planes, VTK_PTR(vtkTransform) transform, bool inverted)
{
  if(nullptr == planes)
  {
    return;
  }

  if(nullptr == m_ClipAlgorithm)
  {
    createFilter();
  }

  // Handle Box-Type clips
  m_LastClipType = ClipType::BOX;
  m_LastBoxInverted = inverted;
  m_LastBoxTransform->DeepCopy(transform);

  m_ClipAlgorithm->SetClipFunction(planes);
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
  else if(getParentFilter())
  {
    return getParentFilter()->getOutputPort();
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
  else if(getParentFilter())
  {
    return getParentFilter()->getOutput();
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
bool VSClipFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  dataType_t outputType = filter->getOutputType();
  dataType_t requiredType = getRequiredInputType();
  if(compatibleInput(filter->getOutputType(), getRequiredInputType()))
  {
    return true;
  }

  return false;
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
VTK_PTR(vtkTransform) VSClipFilter::getLastBoxTransform() 
{
  return m_LastBoxTransform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::ClipType VSClipFilter::getLastClipType()
{
  return m_LastClipType;
}
