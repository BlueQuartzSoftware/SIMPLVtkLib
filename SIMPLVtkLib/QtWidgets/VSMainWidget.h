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

#include "SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
* @class VSMainWidget VSMainWidget.h SIMPLVtkLib/QtWidgets/VSMainWidget.h
* @brief This class works as SIMPLVtkLib's main widget that displays information
* relating to the stored VSController. It subclasses from VSMainWidgetBase for 
* most of its implementation but is provided with a UI file for formatting the
* widget.
*/
class SIMPLVtkLib_EXPORT VSMainWidget : public VSMainWidgetBase
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  */
  VSMainWidget(QWidget* parent = nullptr);

protected:
  /**
  * @brief Connect Qt signals and slots
  */
  virtual void connectSlots() override;

protected slots:
  /**
  * @brief Notifies change in the active VSAbstractViewWidget
  * @param viewWidget
  */
  void setActiveView(VSAbstractViewWidget* viewWidget) override;

  /**
   * @brief setCurrentFilter
   * @param filter
   */
  void setCurrentFilter(VSAbstractFilter* filter) override;

  /**
  * @brief Sets the active view camera position to the X+ axis
  */
  void activeCameraXPlus();

  /**
  * @brief Sets the active view camera position to the Y+ axis
  */
  void activeCameraYPlus();

  /**
  * @brief Sets the active view camera position to the Z+ axis
  */
  void activeCameraZPlus();

  /**
  * @brief Sets the active view camera position to the X- axis
  */
  void activeCameraXMinus();

  /**
  * @brief Sets the active view camera position to the Y- axis
  */
  void activeCameraYMinus();

  /**
  * @brief Sets the active view camera position to the Z- axis
  */
  void activeCameraZMinus();

  /**
  * @brief Renders all VSViewWidgets
  */
  void renderAll();

  /**
  * @brief Resets all render view cameras
  */
  void resetCamera();

private:
  class vsInternals;
  vsInternals* m_Internals;
};
