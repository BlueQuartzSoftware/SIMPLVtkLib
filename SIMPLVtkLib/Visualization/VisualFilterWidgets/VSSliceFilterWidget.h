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

#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSAbstractFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSSliceFilterWidget VSSliceFilterWidget.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilterWidget.h
 * @brief This class controls the slice filter and, as with other classes 
 * inheriting from VSAbstractFilter, can be chained together to further 
 * specify what part of the volume should be rendered.
 */
class SIMPLVtkLib_EXPORT VSSliceFilterWidget : public VSAbstractFilterWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param filter
  * @param interactor
  * @param parent
  */
  VSSliceFilterWidget(VSSliceFilter* filter, vtkRenderWindowInteractor* interactor, QWidget* parent = nullptr);

  /**
  * @brief Deconstructor
  */
  virtual ~VSSliceFilterWidget() = default;

  /**
  * @brief Sets the filter's bounds
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
  * @brief Sets the vtkRenderWindowInteractor for the filter widget
  * @param interactor
  */
  void setInteractor(vtkRenderWindowInteractor* interactor) override;

protected slots:
  /**
  * @brief Updates the filter widget when the transform is updated
  */
  void updateTransform();

private:
  class vsInternals;
  vsInternals*                    m_Internals;

  VSSliceFilter*                  m_SliceFilter;
  VSPlaneWidget*                  m_SliceWidget;
};
