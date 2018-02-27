/* ============================================================================
* Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include <QVTKOpenGLWidget.h>
#include <vtkSmartPointer.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderer.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"

/**
 * @class VisualizationWidget VisualizationWidget.h SIMPLVtkLib/QtWidgets/VisualizationWidget.h
 * @brief This class is the base for VTK visualization widgets and subclasses from QVTKWidget.
 * It contains methods for saving screenshots, rendering, and camera settings so that those
 * functions do not have to be rewritten in every 3D visualization class.
 */
class SIMPLVtkLib_EXPORT VSVisualizationWidget : public QVTKOpenGLWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  * @param numLayers
  */
  VSVisualizationWidget(QWidget* parent, unsigned int numLayers = 1);

  /**
  * @brief Deconstructor
  */
  virtual ~VSVisualizationWidget();

  /**
  * @brief Saves a screenshot to a given file path
  * @param fileName
  */
  void saveScreenshot(QString fileName);

  /**
  * @brief Performs a deep copy based on another visualization widget
  */
  void copy(VSVisualizationWidget* other);

  /**
  * @brief Returns the renderer
  * @return
  */
  VTK_PTR(vtkRenderer) getRenderer();

signals:
  void mousePressed();

public slots:
  /**
  * @brief Renders the VTK context
  */
  void render();

  /**
  * @brief Resets the camera's position, rotation, and zoom
  */
  virtual void resetCamera();

  /**
  * @brief Creates a dialog asking where to save a screenshot of the VTK renderer
  */
  void saveScreenshotPath();

  /**
  * @brief Aligns the camera along the X+ axis
  */
  void camXPlus();

  /**
  * @brief Aligns the camera along the Y+ axis
  */
  void camYPlus();

  /**
  * @brief Aligns the camera along the Z+ axis
  */
  void camZPlus();

  /**
  * @brief Aligns the camera along the X- axis
  */
  void camXMinus();

  /**
  * @brief Aligns the camera along the Y- axis
  */
  void camYMinus();

  /**
  * @brief Aligns the camera along the Z- axis
  */
  void camZMinus();

  /**
  * @brief Clears the render window
  */
  virtual void clearRenderWindow();

protected:
  /**
  * @brief Sets up the GUI as well as initializes the Renderer and Axes
  */
  void setupGui();

  /**
  * @brief Initializes the renderers and axes
  */
  virtual void initializeRendererAndAxes();

  /**
  * @brief Finds the camera's focal point and distance
  * @param focalPoint
  * @param distance
  */
  void getCameraFocalPointAndDistance(double* focalPoint, double& distance);

  /**
  * @brief overrides the mousePressEvent
  */
  virtual void mousePressEvent(QMouseEvent* event) override;

private:
  VTK_PTR(vtkOrientationMarkerWidget) m_OrientationWidget = nullptr;
  VTK_PTR(vtkRenderer) m_Renderer = nullptr;

  unsigned int m_NumRenderLayers;
};
