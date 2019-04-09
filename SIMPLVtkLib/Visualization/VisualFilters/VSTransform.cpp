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

#include "VSTransform.h"

#include <cmath>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform::VSTransform(VSTransform* parent)
{
  m_LocalTransform = VTK_PTR(vtkTransform)::New();

  setParent(parent);
  setupSignals();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform::VSTransform(const VSTransform& copy)
{
  m_LocalTransform = VTK_PTR(vtkTransform)::New();
  m_LocalTransform->DeepCopy(copy.m_LocalTransform);

  setParent(copy.getParent());
  setOriginPosition(copy.getOriginPosition());
  setupSignals();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setupSignals()
{
  connect(this, &VSTransform::emitPosition, this, [=] {
    emit updatedPosition();
    emit valuesChanged();
  });
  connect(this, &VSTransform::emitRotation, this, [=] {
    emit updatedRotation();
    emit valuesChanged();
  });
  connect(this, &VSTransform::emitScale, this, [=] {
    emit updatedScale();
    emit valuesChanged();
  });
  connect(this, &VSTransform::emitAll, this, [=] {
    emit updatedPosition();
    emit updatedRotation();
    emit updatedScale();
    emit valuesChanged();
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::operator=(const VSTransform& copy)
{
  m_LocalTransform->DeepCopy(copy.m_LocalTransform);
  emit emitAll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setParent(VSTransform* parent)
{
  if(m_Parent)
  {
    disconnect(m_Parent, SIGNAL(emitPosition()), this, SIGNAL(emitPosition()));
    disconnect(m_Parent, SIGNAL(emitRotation()), this, SIGNAL(emitRotation()));
    disconnect(m_Parent, SIGNAL(emitScale()), this, SIGNAL(emitScale()));
    disconnect(m_Parent, SIGNAL(emitAll()), this, SIGNAL(emitAll()));
  }

  m_Parent = parent;

  if(parent)
  {
    connect(m_Parent, SIGNAL(emitPosition()), this, SIGNAL(emitPosition()));
    connect(m_Parent, SIGNAL(emitRotation()), this, SIGNAL(emitRotation()));
    connect(m_Parent, SIGNAL(emitScale()), this, SIGNAL(emitScale()));
    connect(m_Parent, SIGNAL(emitAll()), this, SIGNAL(emitAll()));
  }

  emit emitAll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform* VSTransform::getParent() const
{
  return m_Parent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getPosition()
{
  double* position = new double[3];
  getGlobalTransform()->GetPosition(position);
  return position;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getLocalPosition()
{
  double* position = new double[3];
  m_LocalTransform->GetPosition(position);
  return position;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getRotation()
{
  if(nullptr == m_Parent)
  {
    return getLocalRotation();
  }

  double* rotation = new double[3];
  getGlobalTransform()->GetOrientation(rotation);

  for(int i = 0; i < 3; i++)
  {
    if(abs(rotation[i]) < 0.0001)
    {
      rotation[i] = 0.0;
    }
  }

  return rotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getLocalRotation()
{
  double* rotation = new double[3];
  m_LocalTransform->GetOrientation(rotation);

  for(int i = 0; i < 3; i++)
  {
    if(abs(rotation[i]) < 0.0001)
    {
      rotation[i] = 0.0;
    }
  }

  return rotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getScale()
{
  double* scale = new double[3];
  getGlobalTransform()->GetScale(scale);
  return scale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getLocalScale()
{
  double* scale = new double[3];
  m_LocalTransform->GetScale(scale);
  return scale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::translate(double delta[3])
{
  m_LocalTransform->Translate(delta);
  emit emitPosition();
  emit updatedLocalPosition();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::rotate(double amount, double axis[3])
{
  m_LocalTransform->RotateWXYZ(amount, axis);
  emit emitRotation();
  emit updatedLocalRotation();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::scale(double amount)
{
  m_LocalTransform->Scale(amount, amount, amount);
  emit emitScale();
  emit updatedLocalScale();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::scale(double amount[3])
{
  m_LocalTransform->Scale(amount);
  emit emitScale();
  emit updatedLocalScale();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransform) VSTransform::getGlobalTransform()
{
  VTK_NEW(vtkTransform, transform);
  transform->DeepCopy(m_LocalTransform);

  if(m_Parent)
  {
    transform->SetInput(m_Parent->getGlobalTransform());
  }

  return transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransform) VSTransform::getLocalTransform()
{
  VTK_NEW(vtkTransform, transform);
  transform->DeepCopy(m_LocalTransform);

  updateTransform(transform);

  return transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkTransform* VSTransform::createTransform(double position[3], double rotation[3], double scale[3])
{
  vtkTransform* transform = vtkTransform::New();

  // Translate
  transform->Translate(position);

  // Only rotate if there is a value to rotate by
  if(abs(rotation[2]) >= 0.0001)
  {
    transform->RotateZ(rotation[2]);
  }
  if(abs(rotation[1]) >= 0.0001)
  {
    transform->RotateY(rotation[1]);
  }
  if(abs(rotation[0]) >= 0.0001)
  {
    transform->RotateX(rotation[0]);
  }

  // Scale
  transform->Scale(scale);

  // Set the hard value and reset the pipelined transformation
  updateTransform(transform);

  return transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setLocalValues(double position[3], double rotation[3], double scale[3])
{
  m_LocalTransform->Identity();

  // Translate
  m_LocalTransform->Translate(position);

  // Only rotate if there is a value to rotate by
  if(abs(rotation[2]) >= 0.0001)
  {
    m_LocalTransform->RotateZ(rotation[2]);
  }
  if(abs(rotation[1]) >= 0.0001)
  {
    m_LocalTransform->RotateY(rotation[1]);
  }
  if(abs(rotation[0]) >= 0.0001)
  {
    m_LocalTransform->RotateX(rotation[0]);
  }

  // Scale
  m_LocalTransform->Scale(scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSTransform::getPositionVector()
{
  double* positionPtr = getPosition();
  std::vector<double> outputVector(3);
  for(int i = 0; i < 3; i++)
  {
    outputVector[i] = positionPtr[i];
  }

  return outputVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSTransform::getRotationVector()
{
  double* rotationPtr = getRotation();
  std::vector<double> outputVector(3);
  for(int i = 0; i < 3; i++)
  {
    outputVector[i] = rotationPtr[i];
  }

  return outputVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSTransform::getScaleVector()
{
  double* scalePtr = getScale();
  std::vector<double> outputVector(3);
  for(int i = 0; i < 3; i++)
  {
    outputVector[i] = scalePtr[i];
  }

  return outputVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSTransform::getLocalPositionVector()
{
  double* localPositionPtr = getLocalPosition();
  std::vector<double> outputVector(3);
  for(int i = 0; i < 3; i++)
  {
    outputVector[i] = localPositionPtr[i];
  }

  return outputVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSTransform::getLocalRotationVector()
{
  double* localRotationPtr = getLocalRotation();
  std::vector<double> outputVector(3);
  for(int i = 0; i < 3; i++)
  {
    outputVector[i] = localRotationPtr[i];
  }

  return outputVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSTransform::getLocalScaleVector()
{
  double* localScalePtr = getLocalScale();
  std::vector<double> outputVector(3);
  for(int i = 0; i < 3; i++)
  {
    outputVector[i] = localScalePtr[i];
  }

  return outputVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setLocalPositionVector(std::vector<double> positionVector)
{
  double position[3];
  for(int i = 0; i < 3; i++)
  {
    position[i] = positionVector[i];
  }
  setLocalPosition(position);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setLocalRotationVector(std::vector<double> rotationVector)
{
  double rotation[3];
  for(int i = 0; i < 3; i++)
  {
    rotation[i] = rotationVector[i];
  }
  setLocalRotation(rotation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setLocalScaleVector(std::vector<double> scaleVector)
{
  double scale[3];
  for(int i = 0; i < 3; i++)
  {
    scale[i] = scaleVector[i];
  }
  setLocalScale(scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setLocalPosition(double position[3])
{
  double rotation[3];
  double scale[3];

  m_LocalTransform->GetOrientation(rotation);
  m_LocalTransform->GetScale(scale);

  setLocalValues(position, rotation, scale);

  emit emitPosition();
  emit updatedLocalPosition();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setLocalRotation(double rotation[3])
{
  double position[3];
  double scale[3];

  m_LocalTransform->GetPosition(position);
  m_LocalTransform->GetScale(scale);

  setLocalValues(position, rotation, scale);

  emit emitRotation();
  emit updatedLocalRotation();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setLocalScale(double scale[3])
{
  double position[3];
  double rotation[3];

  m_LocalTransform->GetPosition(position);
  m_LocalTransform->GetOrientation(rotation);

  setLocalValues(position, rotation, scale);

  emit emitScale();
  emit updatedLocalScale();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransform) VSTransform::getLocalizeTransform()
{
  VTK_PTR(vtkTransform) transform = getGlobalTransform();
  transform->Inverse();

  return transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransform) VSTransform::getGlobalizeTransform()
{
  return getGlobalTransform();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransform) VSTransform::getTransposedTransform()
{
  VTK_PTR(vtkTransform) transpose = getGlobalTransform();
  VTK_NEW(vtkMatrix4x4, matrix);
  transpose->GetTranspose(matrix);
  transpose->SetMatrix(matrix);

  return transpose;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::localizePoint(double point[3])
{
  getLocalizeTransform()->TransformPoint(point, point);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::localizePoints(vtkPoints* points)
{
  getLocalizeTransform()->TransformPoints(points, points);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::localizeNormal(double normal[3])
{
  getLocalizeTransform()->TransformNormal(normal, normal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::localizeNormals(vtkDataArray* normals)
{
  getLocalizeTransform()->TransformNormals(normals, normals);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::localizePlane(vtkPlane* plane)
{
  if(nullptr == plane)
  {
    return;
  }

  double* normal = plane->GetNormal();
  double* origin = plane->GetOrigin();

  localizeNormal(normal);
  localizePoint(origin);

  plane->SetNormal(normal);
  plane->SetOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::localizePlanes(vtkPlanes* planes)
{
  planes->SetTransform(getGlobalizeTransform());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::localizeTransform(vtkTransform* transform)
{
  transform->SetInput(getLocalizeTransform());
  updateTransform(transform);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::globalizePoint(double point[3])
{
  getGlobalizeTransform()->TransformPoint(point, point);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::globalizePoints(vtkPoints* points)
{
  getGlobalizeTransform()->TransformPoints(points, points);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::globalizeNormal(double normal[3])
{
  getGlobalizeTransform()->TransformNormal(normal, normal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::globalizeNormals(vtkDataArray* normals)
{
  getGlobalizeTransform()->TransformNormals(normals, normals);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::globalizePlane(vtkPlane* plane)
{
  if(nullptr == plane)
  {
    return;
  }

  double* normal = plane->GetNormal();
  double* origin = plane->GetOrigin();

  globalizeNormal(normal);
  globalizePoint(origin);

  plane->SetNormal(normal);
  plane->SetOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::globalizePlanes(vtkPlanes* planes)
{
  planes->SetTransform(getLocalizeTransform());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::globalizeTransform(vtkTransform* transform)
{
  transform->SetInput(getGlobalizeTransform());
  updateTransform(transform);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::updateTransform(vtkTransform* transform)
{
  VTK_NEW(vtkMatrix4x4, matrix);
  matrix->DeepCopy(transform->GetMatrix());

  transform->SetInput(nullptr);
  transform->SetMatrix(matrix);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getOriginPosition() const
{
  return m_OriginPosition;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setOriginPosition(double* originPosition)
{
  m_OriginPosition = originPosition;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getOriginRotation() const
{
  return m_OriginRotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setOriginRotation(double* originRotation)
{
  m_OriginRotation = originRotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getOriginScale() const
{
  return m_OriginScale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setOriginScale(double* originScale)
{
  m_OriginScale= originScale;
}
