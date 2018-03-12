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

#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"

/**
* @class VSViewWidget VSViewWidget.h SIMPLVtkLib/QtWidgets/VSViewWidget.h
* @brief This class handles a single visualization widget and any corresponding 
* widget layout.  As it inherits from VSAbstractViewWidget, this class also 
* handles the VSViewController and VSFilterViewSettings that go along with it.
*/
class SIMPLVtkLib_EXPORT VSViewWidget : public VSAbstractViewWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  * @param windowFlags
  */
  VSViewWidget(QWidget* parent = nullptr, Qt::WindowFlags windowFlags = Qt::WindowFlags());

  /**
  * @brief Copy constructor
  */
  VSViewWidget(const VSViewWidget& other);

  /**
  * @brief Deconstructor
  */
  virtual ~VSViewWidget() = default;

  /**
  * @brief Clones the widget and its view controller
  * @return
  */
  virtual VSAbstractViewWidget* clone() override;

  /**
  * @brief Returns the VSVisualizationWidget used
  */
  virtual VSVisualizationWidget* getVisualizationWidget() const override;

  /**
   * @brief setActive
   * @param active
   */
  void setActive(bool active) override;

protected:
  /**
  * @brief Connect Qt signals and slots
  */
  void connectSlots();

protected slots:
  /**
  * @brief ScalarBar visibility changed for filter
  * @param viewSettings
  * @param showScalarBar
  */
  void setFilterShowScalarBar(VSFilterViewSettings* viewSettings, bool showScalarBar) override;

private:
  class VSInternals;
  VSInternals* m_Internals;

  /**
   * @brief toActiveState
   */
  void toActiveState();

  /**
   * @brief toInactiveState
   */
  void toInactiveState();
};
