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

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <QtWidgets/QWidget>
#include <vector>
#include <vtkSmartPointer.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkRenderWindowInteractor;
class vtkImplicitFunction;

/**
* @class VSAbstractWidget VSAbstractWidget.h 
* SIMPLVtkLib/Visualization/VtkWidgets/VSAbstractWidget.h
* @brief This is the base class for combined Qt/vtk widget representations.
* Subclasses will be displayed in the GUI as a subclass of QWidget and may 
* display a representation of the stored data through a vtkWidget in the the 
* given vtkRenderWindowInteractor.
*/
class SIMPLVtkLib_EXPORT VSAbstractWidget : public QWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  * @param bounds
  * @param iren
  */
  VSAbstractWidget(QWidget* parent, double bounds[6], vtkRenderWindowInteractor* iren);

  /**
  * @brief Copies the vtkWidget bounds
  * @param bounds
  */
  void getBounds(double bounds[6]);

  /**
  * @brief Returns the vtkWidget bounds
  * @return
  */
  double* getBounds();

  /**
  * @brief Copies the vtkWidget origin
  * @param origin
  */
  void getOrigin(double origin[3]);
  
  /**
  * @brief Returns the vtkWidget origin
  * @return
  */
  double* getOrigin();

  /**
  * @brief Sets the bounds
  * @param bounds
  */
  void setBounds(double bounds[6]);

  /**
  * @brief Sets the origin
  * @param origin
  */
  virtual void setOrigin(double origin[3]);

  /**
  * @brief Sets the origin
  * @param x
  * @param y
  * @param z
  */
  virtual void setOrigin(double x, double y, double z);

  /**
  * @brief Enables the vtkWidget
  */
  virtual void enable() = 0;

  /**
  * @brief Disables the vtkWidget
  */
  virtual void disable() = 0;

  /**
  * @brief Returns the vtkRenderWindowInteractor 
  * @return
  */
  vtkRenderWindowInteractor* getInteractor();

  /**
  * @brief Sets the vtkRenderWindowInteractor to render to
  * @param interactor
  */
  virtual void setInteractor(vtkRenderWindowInteractor* interactor);

signals:
  void modified();

protected:
  /**
  * @brief Updates the widget bounds
  */
  virtual void updateBounds();

  /**
  * @brief Updates the widget origin
  */
  virtual void updateOrigin();

  const double MIN_SIZE = 6.0;

private:
  vtkRenderWindowInteractor * m_RenderWindowInteractor;
  double m_Bounds[6];
  double m_Origin[3];
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif
