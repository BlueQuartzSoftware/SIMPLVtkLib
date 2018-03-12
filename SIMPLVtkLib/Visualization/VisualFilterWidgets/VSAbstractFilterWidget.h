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

#include <vtkRenderWindowInteractor.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
* @class VSAbstractFilterWidget VSAbstractFilterWidget.h
* SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilterWidget.h
* @brief This is the base class for all visual filter widgets in SIMPLVtkLib.
* Classes that inherit from this are widgets for the filters that handle various
* vtk algorithms for filtering out parts of the vtkDataSet input into the top-level
* VSSIMPLDataContainerFilter.  Filters can be chained together to be more specific about what
* kind of data should be shown by pushing the output of a filter as the input for
* each of its child filters.
*/
class SIMPLVtkLib_EXPORT VSAbstractFilterWidget : public QWidget
{
  Q_OBJECT

public:
  /**
  * @brief Deconstructor
  */
  virtual ~VSAbstractFilterWidget() = default;

  /**
  * @brief Applies changes to the filter and updates the output
  */
  virtual void apply();

  /**
  * @brief Resets the filter
  */
  virtual void reset();

  /**
   * @brief Sets whether the filter widget should render its vtkWidget in the visualization window
   * @param enabled
   */
  virtual void setRenderingEnabled(bool enabled);

  /**
  * @brief Returns whether the filter widget is rendered
  * @return
  */
  bool getRenderingEnabled();

  /**
   * @brief Sets the vtkRenderWindowInteractor for the filter widget
   * @param interactor
   */
  virtual void setInteractor(vtkRenderWindowInteractor* interactor);

  /**
  * @brief Returns true if there are unapplied changes waiting. Returns false otherwise.
  * @return
  */
  bool hasChanges();

signals:
  void changesMade();

protected:
  /**
  * @brief Constructor
  * @param parent
  */
  VSAbstractFilterWidget(QWidget *parent = nullptr);

  /**
  * @brief Sets the internal unapplied changes variable to false
  */
  void cancelChanges();

protected slots:
  /**
  * @brief Emits a signal alerting that changes were made and sets the internal boolean variable
  */
  void changesWaiting();

private:
  bool m_RenderingEnabled = true;
  bool m_ChangesWaiting = true;
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif
