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

#include <QtCore/QObject>

#include <vtkMatrix4x4.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkPoints.h>
#include <vtkTransform.h>

#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSTransform VSTransform.h SIMPLVtkLib/Visualization/VisualFilters/VSTransform.h
 * @brief This class stores the local coordinates for placing an object in 3D
 * space including local values for position, rotation, and scale. Global values
 * are found by applying the local value to the parent's global value.
 */
class SIMPLVtkLib_EXPORT VSTransform : public QObject
{
  Q_OBJECT

  Q_PROPERTY(std::vector<double> localPosition READ getLocalPositionVector WRITE setLocalPositionVector NOTIFY updatedLocalPosition)
  Q_PROPERTY(std::vector<double> localRotation READ getLocalRotationVector WRITE setLocalRotationVector NOTIFY updatedLocalRotation)
  Q_PROPERTY(std::vector<double> localScale READ getLocalScaleVector WRITE setLocalScaleVector NOTIFY updatedLocalScale)

  Q_PROPERTY(std::vector<double> globalPosition READ getPositionVector NOTIFY updatedPosition)
  Q_PROPERTY(std::vector<double> globalRotation READ getRotationVector NOTIFY updatedRotation)
  Q_PROPERTY(std::vector<double> globalScale READ getScaleVector NOTIFY updatedScale)

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSTransform(VSTransform* parent = nullptr);

  /**
   * @brief Copy Constructor
   * @param copy
   */
  VSTransform(const VSTransform& copy);

  /**
   * @brief Deconstructor
   */
  virtual ~VSTransform() = default;

  /**
   * @brief Assignment operator
   * @param copy
   */
  void operator=(const VSTransform& copy);

  /**
   * @brief Sets the parent transform
   * @param parent
   */
  void setParent(VSTransform* parent);

  /**
   * @brief Returns the parent transform
   * @return
   */
  VSTransform* getParent() const;

  /**
   * @brief Returns a vector<double> with the global position
   * @return
   */
  std::vector<double> getPositionVector();

  /**
   * @brief Returns a vector<double> with the global rotation
   * @return
   */
  std::vector<double> getRotationVector();

  /**
   * @brief Returns a vector<double> with the global scale
   * @return
   */
  std::vector<double> getScaleVector();

  /**
   * @brief Returns a vector<double> with the local position
   * @return
   */
  std::vector<double> getLocalPositionVector();

  /**
   * @brief Returns a vector<double> with the local rotation
   * @return
   */
  std::vector<double> getLocalRotationVector();

  /**
   * @brief Returns a vector<double> with the local scale
   * @return
   */
  std::vector<double> getLocalScaleVector();

  /**
   * @brief Sets the transform's local position
   * @param position
   */
  void setLocalPositionVector(std::vector<double> position);

  /**
   * @brief Sets the transform's local rotation
   * @param rotation
   */
  void setLocalRotationVector(std::vector<double> rotation);

  /**
   * @brief Sets the transform's local scale
   * @param scale
   */
  void setLocalScaleVector(std::vector<double> scale);

  /**
   * @brief Returns the transform's global position in 3D space
   * @return
   */
  double* getPosition();

  /**
   * @brief Returns the transform's local position in 3D space
   * @return
   */
  double* getLocalPosition();

  /**
   * @brief Returns the transform's global Euler rotation
   * @return
   */
  double* getRotation();

  /**
   * @brief Returns the transform's local Euler rotation
   * @return
   */
  double* getLocalRotation();

  /**
   * @brief Returns the transform's global scale
   * @return
   */
  double* getScale();

  /**
   * @brief Returns the transform's local scale
   * @return
   */
  double* getLocalScale();

  /**
   * @brief Sets the transform's local position
   * @param position
   */
  void setLocalPosition(double position[3]);

  /**
   * @brief Sets the transform's local Euler rotation
   * @param rotation
   */
  void setLocalRotation(double rotation[3]);

  /**
   * @brief Sets the transform's local scale
   * @param scale
   */
  void setLocalScale(double scale[3]);

  /**
   * @brief Translates the object in global space
   * @param delta
   */
  void translate(double delta[3]);

  /**
   * @brief Rotates the transform by the given amount along a specified axis
   * @param amount
   * @param axis
   */
  void rotate(double amount, double axis[3]);

  /**
   * @brief Scales the transform by a given amount
   * @param amount
   */
  void scale(double amount);

  /**
   * @brief Scales the transform by a given amount
   * @param amount
   */
  void scale(double amount[3]);

  /**
   * @brief Returns the vtkTransform describing the global position, rotation, and scale.
   * @return
   */
  VTK_PTR(vtkTransform) getGlobalTransform();

  /**
   * @brief Localizes the given point from global space
   * @param point
   */
  void localizePoint(double point[3]);

  /**
   * @brief Localizes the given points from global space
   * @param points
   */
  void localizePoints(vtkPoints* points);

  /**
   * @brief Localizes the given normal from global space
   * @param orientation
   */
  void localizeNormal(double normal[3]);

  /**
   * @brief Localizes the given normals from global space
   * @param normals
   */
  void localizeNormals(vtkDataArray* normals);

  /**
   * @brief Localizes the given plane from global space
   * @param plane
   */
  void localizePlane(vtkPlane* plane);

  /**
   * @brief Localizes the given planes from global space
   * @param planes
   */
  void localizePlanes(vtkPlanes* planes);

  /**
   * @brief Localizes the given transform from the global space
   * @param transform
   */
  void localizeTransform(vtkTransform* transform);

  /**
   * @brief Performs the transformation on the given point to put it in global space
   * @param point
   */
  void globalizePoint(double point[3]);

  /**
   * @brief Globalizes the given points to put them in global space
   * @param points
   */
  void globalizePoints(vtkPoints* points);

  /**
   * @brief Performs the transformation on the given normal to put it in global space
   * @param normal
   */
  void globalizeNormal(double normal[3]);

  /**
   * @brief Globalizes the given normals to put them in global space
   * @param normals
   */
  void globalizeNormals(vtkDataArray* normals);

  /**
   * @brief Performs the transformation on the given plane to put it in global space
   * @param plane
   */
  void globalizePlane(vtkPlane* plane);

  /**
   * @brief Performs the transformation on the given planes to put them in global space
   * @param planes
   */
  void globalizePlanes(vtkPlanes* planes);

  /**
   * @brief Performs the transformation on the given transformation to put it in the global space
   * @param transform
   */
  void globalizeTransform(vtkTransform* transform);

  /**
   * @brief Get the origin position of the transform
   * @return origin
   */
  double* getOriginPosition() const;

  /**
   * @brief Set the origin position of the transform
   * @param originPosition
   */
  void setOriginPosition(double* originPosition);

  /**
   * @brief Get the origin of the transform
   * @return originRotation
   */
  double* getOriginRotation() const;

  /**
   * @brief Set the origin rotation of the transform
   * @param originRotation
   */
  void setOriginRotation(double* originRotation);

  /**
   * @brief Get the origin scale of the transform
   * @return originScale
   */
  double* getOriginScale() const;

  /**
   * @brief Set the origin scale of the transform
   * @param originScale
   */
  void setOriginScale(double* originScale);

signals:
  void updatedPosition();
  void updatedRotation();
  void updatedScale();
  void emitPosition();
  void emitRotation();
  void emitScale();
  void emitAll();
  void valuesChanged();
  void updatedLocalPosition();
  void updatedLocalRotation();
  void updatedLocalScale();

protected:
  /**
   * @brief Sets up the initial signals
   */
  void setupSignals();

  /**
   * @brief Returns a new vtkTransform based on the given values
   * @param position
   * @param rotation
   * @param scale
   */
  static vtkTransform* createTransform(double position[3], double rotation[3], double scale[3]);

  /**
   * @brief Sets the local transformation to match the given values.
   * @param position
   * @param rotation
   * @param scale
   */
  void setLocalValues(double position[3], double rotation[3], double scale[3]);

  /**
   * @brief Returns the vtkTransform describing the local position, rotation, and scale.
   * @return
   */
  VTK_PTR(vtkTransform) getLocalTransform();

  /**
   * @brief Returns the vtkTransform for localizing data
   * @return
   */
  VTK_PTR(vtkTransform) getLocalizeTransform();

  /**
   * @brief Returns the vtkTransform for globalizing data
   * @return
   */
  VTK_PTR(vtkTransform) getGlobalizeTransform();

  /**
   * @brief Returns the transposed globalizing transform
   * @return
   */
  VTK_PTR(vtkTransform) getTransposedTransform();

  /**
   * @brief Resets the vtkTransform so that any pipelined transforms are removed
   * @param transform
   */
  static void updateTransform(vtkTransform* transform);

private:
  VSTransform* m_Parent = nullptr;

  double* m_OriginPosition;
  double* m_OriginRotation;
  double* m_OriginScale;
  VTK_PTR(vtkTransform) m_LocalTransform;
};
