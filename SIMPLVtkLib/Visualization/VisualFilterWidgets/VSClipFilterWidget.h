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

#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSAbstractFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"

/**
 * @class VSClipFilterWidget VSClipFilterWidget.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSClipFilterWidget.h
 * @brief This class is used to create a clip filter over a set of data.
 * This class can be chained with itself or other classes inheriting from 
 * VSAbstractFilter to be more specific about the data being visualized.
 * VSClipFilterWidget can use both plan and box clip types as well as inverting
 * the clip applied.
 */
class SIMPLVtkLib_EXPORT VSClipFilterWidget : public VSAbstractFilterWidget
{
  Q_OBJECT

public:
  /**
   * @brief VSClipFilterWidget
   * @param filter
   * @param interactor
   * @param widget
   */
  VSClipFilterWidget(VSClipFilter *filter, vtkRenderWindowInteractor* interactor, QWidget* widget = nullptr);

  /**
  * @brief Deconstructor
  */
  ~VSClipFilterWidget();

  /**
  * @brief Sets the visualization filter's bounds
  * @param bounds
  */
  void setBounds(double* bounds);

  /**
  * @brief Applies changes to the filter and updates the output
  */
  void apply() override;

  /**
   * @brief reset
   */
  void reset() override;

  /**
   * @brief Sets whether the filter widget should render drawings in the visualization window
   * @param enabled
   */
  void setRenderingEnabled(bool enabled) override;

  /**
   * @brief setInteractor
   * @param interactor
   */
  void setInteractor(vtkRenderWindowInteractor* interactor) override;

protected slots:
  /**
   * @brief changeClipType
   * @param clipType
   */
  void changeClipType(int clipType);

  /**
  * @brief Calls updates for the VSTransform
  */
  void updateTransform();

private:
  class vsInternals;
  vsInternals*                        m_Internals;

  VSClipFilter*                       m_ClipFilter;
  VSPlaneWidget*                      m_PlaneWidget;
  VSBoxWidget*                        m_BoxWidget;
};
