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

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSAbstractFilterWidget.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSThresholdFilterWidget VSThresholdFilterWidget.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilterWidget.h
 * @brief This class handles the thresholding over a given range for a 
 * specified array. This array does not have to match the data array being 
 * visualized and is set separately within the filter. As this class inherits
 * from VSAbstractFilter, it can be chained with other filters to further
 * specify what part of the volume should be visualized.
 */
class SIMPLVtkLib_EXPORT VSThresholdFilterWidget : public VSAbstractFilterWidget
{
  Q_OBJECT

public:
  /**
  * @brief Consructor
  * @param parentWidget
  * @param parent
  */
  VSThresholdFilterWidget(VSThresholdFilter* filter, vtkRenderWindowInteractor* interactor, QWidget* parent = nullptr);

  /**
  * @brief Deconstructor
  */
  ~VSThresholdFilterWidget();

  /**
  * @brief Sets the filter bounds
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
  * @brief Returns the scalar name used for thresholding
  * @return
  */
  QString getScalarName();

  /**
  * @brief Returns the lower bound for thresholding
  * @return
  */
  double getLowerBound();

  /**
  * @brief Returns the upper bound for thresholding
  * @return
  */
  double getUpperBound();

  /**
  * @brief Sets the lower bound for thresholding
  * @param min
  */
  void setLowerThreshold(double min);

  /**
  * @brief Sets the upper bound for thresholding
  * @param max
  */
  void setUpperThreshold(double max);

protected slots:
  /**
  * @brief Handles changes in the scalar combo box
  * @param index
  */
  void scalarIndexChanged(int index);

  /**
  * @brief Handles any changes in the minimum spin box
  */
  void minSpinBoxValueChanged();

  /**
  * @brief Handles any changes in the maximum spin box
  */
  void maxSpinBoxValueChanged();

  /**
  * @brief Handles any changes in the minimum slider
  */
  void minSliderValueChanged();

  /**
  * @brief Handles any changes in the maximum slider
  */
  void maxSliderValueChanged();

protected:
  /**
  * @brief Populates the scalar combo box with values based on the vtkDataSet received from the VSThresholdFilter
  */
  void setupScalarsComboBox();

  /**
  * @brief Returns the dataArray with the given name
  * @param name
  */
  vtkDataArray* getDataArray(QString name);

  /**
  * @brief Updates the range based on the current selected data array
  */
  void updateRange(QString arrayName = "");

  /**
  * @brief Sets the acceptable range to threshold over
  * @param min
  * @param max
  */
  void setScalarRange(double min, double max);

  /**
  * @brief Initializes the range to fit the first scalar array
  */
  void initRange();

  /**
  * @brief Sets the target array name and updates the bounds accordingly
  * @param arrayName
  */
  void setScalarName(QString arrayName);

  /**
  * @brief Copies the value from a QSlider to a QDoubleSpinBox
  * @param slider
  * @param spinBox
  */
  void sliderToSpinBox(QSlider* slider, QDoubleSpinBox* spinBox);

  /**
  * @brief Copies the value from a QDoubleSpinBox to a QSlider
  * @param spinBox
  * @param slider
  */
  void spinBoxToSlider(QDoubleSpinBox* spinBox, QSlider* slider);

  /**
  * @brief Checks if the min spin box is greater than the max spin box.
  * If it is, then the max spin box is updated and the method returns true.
  * Otherwise, this function returns false.
  * @return
  */
  bool checkMinSpinBox();

  /**
  * @brief Checks if the max spin box is greater than the min spin box.
  * If it is, then the min spin box is updated and the method returns true.
  * Otherwise, this function returns false.
  * @return
  */
  bool checkMaxSpinBox();

private:
  class vsInternals;
  vsInternals* m_Internals;

  VSThresholdFilter* m_ThresholdFilter;
};
