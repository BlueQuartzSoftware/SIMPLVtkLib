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

#include "VSSliceFilter.h"

#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtCore/QJsonArray>

#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGrid.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilter::VSSliceFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_SliceAlgorithm = nullptr;
  setParentFilter(parent);
  setText(getFilterName());
  setToolTip(getToolTip());

  m_LastOrigin[0] = 0.0;
  m_LastOrigin[1] = 0.0;
  m_LastOrigin[2] = 0.0;

  m_LastNormal[0] = 1.0;
  m_LastNormal[1] = 0.0;
  m_LastNormal[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilter* VSSliceFilter::Create(QJsonObject &json, VSAbstractFilter* parent)
{
  VSSliceFilter* filter = new VSSliceFilter(parent);

  QJsonArray lastOrigin = json["Last Origin"].toArray();
  double origin[3];
  origin[0] = lastOrigin.at(0).toDouble();
  origin[1] = lastOrigin.at(1).toDouble();
  origin[2] = lastOrigin.at(2).toDouble();
  filter->setLastOrigin(origin);

  QJsonArray lastNormal = json["Last Normal"].toArray();
  double normals[3];
  normals[0] = lastNormal.at(0).toDouble();
  normals[1] = lastNormal.at(1).toDouble();
  normals[2] = lastNormal.at(2).toDouble();
  filter->setLastNormal(normals);

  filter->setInitialized(true);
  filter->readTransformJson(json);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::createFilter()
{
  m_SliceAlgorithm = vtkSmartPointer<vtkCutter>::New();
  m_SliceAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());
  setConnectedInput(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSSliceFilter::getFilterName()
{
  return "Slice";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSSliceFilter::getToolTip() const
{
  return "Slice Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::apply(double origin[3], double normal[3])
{
  if(nullptr == m_SliceAlgorithm)
  {
    createFilter();
  }

  // Save the applied values for resetting Plane-Type widgets
  for(int i = 0; i < 3; i++)
  {
    m_LastOrigin[i] = origin[i];
    m_LastNormal[i] = normal[i];
  }

  VTK_NEW(vtkPlane, planeWidget);
  planeWidget->SetOrigin(origin);
  planeWidget->SetNormal(normal);

  m_SliceAlgorithm->SetCutFunction(planeWidget);
  m_SliceAlgorithm->Update();

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::readJson(QJsonObject &json)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::writeJson(QJsonObject &json)
{
  VSAbstractFilter::writeJson(json);

  QJsonArray lastOrigin;
  lastOrigin.append(m_LastOrigin[0]);
  lastOrigin.append(m_LastOrigin[1]);
  lastOrigin.append(m_LastOrigin[2]);
  json["Last Origin"] = lastOrigin;

  QJsonArray lastNormal;
  lastNormal.append(m_LastNormal[0]);
  lastNormal.append(m_LastNormal[1]);
  lastNormal.append(m_LastNormal[2]);
  json["Last Normal"] = lastNormal;

  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSSliceFilter::getOutputPort()
{
  if(getConnectedInput() && m_SliceAlgorithm)
  {
    return m_SliceAlgorithm->GetOutputPort();
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
VTK_PTR(vtkDataSet) VSSliceFilter::getOutput()
{
  if(getConnectedInput() && m_SliceAlgorithm)
  {
    return m_SliceAlgorithm->GetOutput();
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
void VSSliceFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  setInputPort(filter->getOutputPort());

  if(getConnectedInput() && m_SliceAlgorithm)
  {
    m_SliceAlgorithm->SetInputConnection(filter->getOutputPort());
  }
  else
  {
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSSliceFilter::GetUuid()
{
  return QUuid("{7a5a24be-13db-5101-8a7c-5a8a7979fbbb}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSSliceFilter::getOutputType()
{
  return POLY_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSSliceFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSliceFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  if(compatibleInput(filter->getOutputType(), getRequiredInputType()))
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSSliceFilter::getLastOrigin()
{
  return m_LastOrigin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSSliceFilter::getLastNormal()
{
  return m_LastNormal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::setLastOrigin(double* origin)
{
  m_LastOrigin[0] = origin[0];
  m_LastOrigin[1] = origin[1];
  m_LastOrigin[2] = origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::setLastNormal(double* normal)
{
  m_LastNormal[0] = normal[0];
  m_LastNormal[1] = normal[1];
  m_LastNormal[2] = normal[2];
}
