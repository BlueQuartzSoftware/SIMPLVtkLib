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

#include <vtkImplicitFunction.h>

#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSAbstractWidget.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkTransform;
class vtkBox;
class vtkPlanes;
class vtkBoxWidget2;
class vtkBoxRepresentation;

/**
 * @class VSBoxWidget VSBoxWidget.h SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h
 * @brief This class handles the storage and editing of vtkBoxRepresentation values.
 */
class SIMPLVtkLib_EXPORT VSBoxWidget : public VSAbstractWidget
{
  Q_OBJECT

  Q_PROPERTY(std::vector<double> translation READ getTranslationVector WRITE setTranslationVector NOTIFY translationChanged)
  Q_PROPERTY(std::vector<double> rotation READ getRotationVector WRITE setRotationVector NOTIFY rotationChanged)
  Q_PROPERTY(std::vector<double> scale READ getScaleVector WRITE setScaleVector NOTIFY scaleChanged)

public:
  /**
   * @brief Constructor
   * @param parent
   * @param bounds
   * @param iren
   */
  VSBoxWidget(QObject* parent, VSTransform* transform, double bounds[6], vtkRenderWindowInteractor* iren);

  /**
   * @brief Copy Constructor
   * @param copy
   */
  VSBoxWidget(const VSBoxWidget& copy);

  /**
   * @brief Default Constructor
   */
  VSBoxWidget();

  /**
   * @brief Deconstructor
   */
  virtual ~VSBoxWidget();

  /**
   * @brief Sets the box's scale
   * @param scale
   */
  void setScale(double scale[3]);

  /**
   * @brief Sets the box's scale
   * @param x
   * @param y
   * @param z
   */
  void setScale(double x, double y, double z);

  /**
   * @brief Sets the box's rotation
   * @param rotation
   */
  void setRotation(double rotation[3]);

  /**
   * @brief Sets the box's rotation
   * @param x
   * @param y
   * @param z
   */
  void setRotation(double x, double y, double z);

  /**
   * @brief Sets the object's origin
   * @param origin
   */
  void setTranslation(double origin[3]);

  /**
   * @brief Returns a vector containing the object's translation
   * @return
   */
  std::vector<double> getTranslationVector();

  /**
   * @brief Returns a vector containing the object's rotation
   * @return
   */
  std::vector<double> getRotationVector();

  /**
   * @brief Returns a vector containing the object's scale
   * @return
   */
  std::vector<double> getScaleVector();

  /**
   * @brief Sets the object's translation
   * @param translationVector
   */
  void setTranslationVector(std::vector<double> translationVector);

  /**
   * @brief Sets the object's rotation
   * @param rotationVector
   */
  void setRotationVector(std::vector<double> rotationVector);

  /**
  * @brief Sets the object's scale
  * @param scaleVector
  */
  void setScaleVector(std::vector<double> scaleVector);

  /**
   * @param Gets the box's transform
   * @return
   */
  VTK_PTR(vtkTransform) getTransform();

  /**
   * @param Sets the box's transform
   * @param transform
   */
  void setTransform(VTK_PTR(vtkTransform) transform);

  /**
   * @brief Returns the vtkPlanes function generated by the box
   * @return
   */
  VTK_PTR(vtkPlanes) getPlanes();

  /**
   * @brief Returns true if the VTK widget is enabled. Returns false otherwise
   * @return
   */
  bool isEnabled() const;

  /**
   * @brief Enables the box widget
   */
  Q_INVOKABLE void enable() override;

  /**
   * @brief Disables the box widget
   */
  Q_INVOKABLE void disable() override;

  /**
   * @brief Gets the box scale
   * @param scale
   */
  void getScale(double scale[3]);

  /**
   * @brief Gets the box rotation
   * @param rotation
   */
  void getRotation(double rotation[3]);

  /**
   * @brief Gets the box translation
   * @param translation
   */
  void getTranslation(double translation[3]);

  /**
   * @brief Updates the box widget
   */
  void updateBoxWidget();

  /**
   * @brief Sets the current vtkRenderWindowInteractor
   * @param interactor
   */
  void setInteractor(vtkRenderWindowInteractor* interactor) override;

  /**
   * @brief Reads values from a json file into the widget
   * @param json
   */
  void readJson(QJsonObject& json) override;

  /**
   * @brief Writes values to a json file from the widget
   * @param json
   */
  void writeJson(const QJsonObject& json) override;

signals:
  void translationChanged();
  void rotationChanged();
  void scaleChanged();

protected slots:
  /**
   * @brief Updates the vtk widget for positioning in global space
   */
  virtual void updateGlobalSpace() override;

protected:
  /**
   * @brief Initializes the VTK Widget
   */
  void setupWidget();

  /**
   * @brief Sets the widget's transformation variables
   * @param position
   * @param rotation
   * @param scale
   */
  void setValues(double position[3], double rotation[3], double scale[3]);

private:
  VTK_PTR(vtkTransform) m_UseTransform;
  VTK_PTR(vtkTransform) m_ViewTransform;
  VTK_PTR(vtkBoxWidget2) m_BoxWidget;
  VTK_PTR(vtkBoxRepresentation) m_BoxRep;
};

Q_DECLARE_METATYPE(VSBoxWidget)
