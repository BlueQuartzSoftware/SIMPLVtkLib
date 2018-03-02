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

#include <vtkThreshold.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSThresholdFilter VSThresholdFilter.h 
 * SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h
 * @brief This class handles the thresholding over a given range for a 
 * specified array. This array does not have to match the data array being 
 * visualized and is set separately within the filter. As this class inherits
 * from VSAbstractFilter, it can be chained with other filters to further
 * specify what part of the volume should be visualized.
 */
class SIMPLVtkLib_EXPORT VSThresholdFilter : public VSAbstractFilter
{
  Q_OBJECT

public:
  /**
  * @brief Consructor
  * @param parentWidget
  * @param parent
  */
  VSThresholdFilter(VSAbstractFilter* parent);

  /**
   * @brief Create
   * @param json
   * @param parent
   * @return
   */
  static VSThresholdFilter* Create(QJsonObject &json, VSAbstractFilter* parent);

  /**
  * @brief Returns the filter name
  * @return
  */
  const QString getFilterName() override;

  /**
  * @brief Returns the tooltip to use for the filter
  * @return
  */
  virtual QString getToolTip() const override;

  /**
  * @brief Applies a threshold over a specified array between a given min and max
  * @param arrayName
  * @param min
  * @param max
  */
  void apply(QString arrayName, double min, double max);

  /**
  * @brief Returns the output port to be used by vtkMappers and subsequent filters
  * @return
  */
  virtual vtkAlgorithmOutput* getOutputPort() override;

  /**
  * @brief Returns a smart pointer containing the output data from the filter
  * @return
  */
  virtual VTK_PTR(vtkDataSet) getOutput() override;

  /**
  * @brief Returns the output data type
  * @return
  */
  dataType_t getOutputType() override;

  /**
  * @brief Returns the required input data type
  * @return
  */
  static dataType_t getRequiredInputType();

  /**
  * @brief Returns true if this filter type can be added as a child of
  * the given filter.  Returns false otherwise.
  * @param
  * @return
  */
  static bool compatibleWithParent(VSAbstractFilter* filter);

  /**
  * @brief Returns the name of the array last used for thresholding
  * @return
  */
  QString getLastArrayName();

  /**
  * @brief Returns the last minimum value for thresholding
  * @return
  */
  double getLastMinValue();

  /**
  * @brief Returns the last maximum value for thresholding
  * @return
  */
  double getLastMaxValue();

  /**
  * @brief Sets the name of the array last used for thresholding
  * @param lastArrayName
  */
  void setLastArrayName(QString lastArrayName);

  /**
  * @brief Sets the last minimum value for thresholding
  * @return
  */
  void setLastMinValue(double lastMinValue);

  /**
  * @brief Sets the last maximum value for thresholding
  * @return
  */
  void setLastMaxValue(double lastMaxValue);

  /**
   * @brief Reads values from a json file into the filter
   * @param json
   */
  void readJson(QJsonObject &json);

  /**
   * @brief Writes values to a json file from the filter
   * @param json
   */
  void writeJson(QJsonObject &json) override;

  /**
   * @brief getUuid
   * @return
   */
  static QUuid GetUuid();

protected:
  /**
  * @brief Initializes the algorithm and connects it to the vtkMapper
  */
  void createFilter() override;

  /**
  * @brief This method updates the input port and connects it to the vtkAlgorithm if it exists
  * @param filter
  */
  void updateAlgorithmInput(VSAbstractFilter* filter) override;

private:
  VTK_PTR(vtkThreshold) m_ThresholdAlgorithm;

  QString m_LastArrayName;
  double m_LastMinValue = 0.0;
  double m_LastMaxValue = 99.9;
};
