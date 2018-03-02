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

#include "SIMPLVtkLib/Visualization/VtkWidgets/VSAbstractWidget.h"
#include "ui_VSPlaneWidget.h"

#include <vtkSmartPointer.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkPlane;
class vtkImplicitPlaneWidget2;
class vtkImplicitPlaneRepresentation;

/**
 * @class VSPlaneWidget VSPlaneWidget.h 
 * SIMPLView/VtkSIMPL/VtkWidgets/VSPlaneWidget.h
 * @brief This class handles the management of vtkImplicitPlaneWidget2 for 
 * VSAbstractFilters that use plane-type VTK widgets for their execution. 
 * This class inherits from QWidget and displays editable values representing 
 * the normal and origin of the plane. Editing these values also changes the 
 * vtkImplicitPlaneWidget2 orientation.
 */
class SIMPLVtkLib_EXPORT VSPlaneWidget : public VSAbstractWidget, private Ui::VSPlaneWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  * @param bounds
  * @param iren
  */
  VSPlaneWidget(QWidget* parent, VSTransform* transform, double bounds[6], vtkRenderWindowInteractor* iren);

  /**
  * @brief Deconstructor
  */
  ~VSPlaneWidget();

  /**
  * @brief Copies the normals into the double array passed in
  * @param normals
  */
  void getNormals(double normals[3]);

  /**
  * @brief Sets the normals to the given value
  * @param normals
  */
  void setNormals(double normals[3]);

  /**
  * @brief Sets the normals to match the given values
  * @param x
  * @param y
  * @param z
  */
  void setNormals(double x, double y, double z);

  /**
  * @brief Returns the origin from the m_UsePlane
  * @return
  */
  double* getOrigin();

  /**
  * @brief Copies the origin from the m_UsePlane into the double array passed in
  * @param origin
  */
  void getOrigin(double origin[3]);

  /**
  * @brief Sets the origin to the given value
  * @param origin
  */
  void setOrigin(double origin[3]);

  /**
   * @brief setInteractor
   * @param interactor
   */
  void setInteractor(vtkRenderWindowInteractor* interactor) override;

  /**
  * @brief Enables the plane widget
  */
  void enable() override;

  /**
  * @brief Disables the plane widget
  */
  void disable() override;

  /**
  * @brief Enables rendering the plane internals
  */
  void drawPlaneOn();

  /**
  * @brief Disables rendering the plane internals
  */
  void drawPlaneOff();

  /**
  * @brief Updates the origin and normal values based on the VTK plane widget 
  * before applying those values to the input widgets.
  */
  void updateSpinBoxes();

  /**
  * @brief Updates the VTK plane
  */
  void updatePlaneWidget();

  /**
   * @brief Reads values from a json file into the widget
   * @param json
   */
  void readJson(QJsonObject &json) override;

  /**
   * @brief Writes values to a json file from the widget
   * @param json
   */
  void writeJson(const QJsonObject &json) override;

public slots:
  /**
  * @brief Updates the VTK plane widget with the input widget values.
  */
  void spinBoxValueChanged();

protected slots:
  /**
  * @brief Updates the vtk widget for positioning in global space
  */
  virtual void updateGlobalSpace() override;

protected:
  /**
  * @brief Updates the bounds representation for the VTK plane widget.
  */
  void updateBounds() override;

private:
  vtkSmartPointer<vtkPlane> m_UsePlane;
  vtkSmartPointer<vtkPlane> m_ViewPlane;
  vtkImplicitPlaneWidget2* m_PlaneWidget;
  vtkImplicitPlaneRepresentation* m_PlaneRep;
};
