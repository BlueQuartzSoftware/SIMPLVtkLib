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

#include "Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkThreshold;
class VSMaskWidget;

/**
 * @class VSMaskFilter VSMaskFilter.h 
 * SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h
 * @brief This class handles the masking process for vtkDataSets based on any 
 * numeric or boolean array.  Numeric values greater than or equal to 1 are 
 * visible whereas values less than 1 are hidden.
 */
class SIMPLVtkLib_EXPORT VSMaskFilter : public VSAbstractFilter
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parentWidget
  * @param parent
  */
  VSMaskFilter(VSAbstractFilter* parent);

  /**
  * Brief Returns the filter name
  * @return
  */
  const QString getFilterName() override;

  /**
  * @brief Returns the tooltip to use for the filter
  * @return
  */
  virtual QString getToolTip() const override;

  /**
  * @brief Applies the mask filter on the specified array
  */
  void apply(QString arrayName);

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
  * @brief Returns the name of the array last applied as a mask
  * @return
  */
  QString getLastArrayName();

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
  VTK_PTR(vtkThreshold) m_MaskAlgorithm;

  QString m_LastArrayName;
};
