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

#ifndef _vsviewwidget_h
#define _vsviewwidget_h

#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/QtWidgets/VSVisualizationWidget.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSViewController.h"

/**
* @class VSViewWidget VSViewWidget.h SIMPLVtkLib/QtWidgets/VSViewWidget.h
* @brief This abstract class applies the VSViewController and VSFilterViewSettings to
* a VSVisualizationWidget.
*/
class VSViewWidget : public QWidget
{
  Q_OBJECT

public:
  VSViewWidget(QWidget* parent = nullptr);

  /**
  * @brief Returns the VSViewController used by the widget
  * @return
  */
  VSViewController* getViewController();

  /**
  * @brief Sets the VSViewController for the widget
  * @param controller
  */
  void setViewController(VSViewController* controller);

  /**
  * @brief Returns a pointer to the active VSFilterViewSettings
  * @return
  */
  VSFilterViewSettings* getActiveFilterSettings();

  /**
  * @brief Sets the active VSFilterViewSettings
  * @param settings
  */
  virtual void setActiveFilterSettings(VSFilterViewSettings* settings);

protected slots:
  /**
  * @brief Change filter visibility
  * @param viewSettings
  * @param filterVisible
  */
  void changeFilterVisibility(VSFilterViewSettings* viewSettings, bool filterVisible);

  /**
  * @brief Change the active filter's array index
  * @param index
  */
  void changeFilterArrayIndex(int index);

  /**
  * @brief Change the active filter's component index
  * @param index
  */
  void changeFilterComponentIndex(int index);

  /**
  * @brief Change the active filter's color map setting
  * @param mapColors
  */
  void changeFilterMapColors(bool mapColors);

  /**
  * @brief Change the active filter's scalar bar visibility
  * @param showScalarBar
  */
  void changeFilterShowScalarBar(bool showScalarBar);

  /**
  * @brief Visiblity changed for filter
  * @param viewSettings
  * @param filterVisible
  */
  virtual void filterVisibilityChanged(VSFilterViewSettings* viewSettings, bool filterVisible);

  /**
  * @brief Active array changed for filter
  * @param viewSettings
  * @param index
  */
  virtual void filterArrayIndexChanged(VSFilterViewSettings* viewSettings, int index);

  /**
  * @brief Active array component changed for filter
  * @param viewSettings
  * @param index
  */
  virtual void filterComponentIndexChanged(VSFilterViewSettings* viewSettings, int index);

  /**
  * @brief Color mapping changed for filter
  * @param viewSettings
  * @param mapColors
  */
  virtual void filterMapColorsChanged(VSFilterViewSettings* viewSettings, bool mapColors);

  /**
  * @brief ScalarBar visibility changed for filter
  * @param viewSettings
  * @param showScalarBar
  */
  void filterShowScalarBarChanged(VSFilterViewSettings* viewSettings, bool showScalarBar);

protected:
  /**
  * @brief Returns the VSVisualizationWidget used
  */
  virtual VSVisualizationWidget* getVisualizationWidget() = 0;

  /**
  * @brief Handle mouse press events including marking the VSViewController as the active one
  * @param event
  */
  virtual void mousePressEvent(QMouseEvent* event) override;

private:
  VSViewController* m_ViewController = nullptr;
  VSFilterViewSettings* m_ActiveFilterSettings = nullptr;
};

#endif
