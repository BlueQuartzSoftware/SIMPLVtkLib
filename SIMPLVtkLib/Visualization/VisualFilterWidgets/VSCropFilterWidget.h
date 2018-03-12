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

#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSAbstractFilterWidget.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSCropFilterWidget VSCropFilterWidget.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSCropFilterWidget.h
 * @brief This class is a visibility filter that crops a vtkDataSet to X, Y, 
 * and Z bounds. This class can be chained with other VSAbstractFilters to
 * further specify the data allowed to be visualized. This filter requires 
 * the incoming data type to be a vtkImageData, thus restricting it to following 
 * VSSIMPLDataContainerFilters.
 */
class SIMPLVtkLib_EXPORT VSCropFilterWidget : public VSAbstractFilterWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param filter
  * @param interactor
  * @param widget
  */
  VSCropFilterWidget(VSCropFilter* filter, vtkRenderWindowInteractor* interactor, QWidget* widget = nullptr);

  /**
  * @brief Deconstructor
  */
  virtual ~VSCropFilterWidget() = default;

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
  * @brief Returns the volume of interest
  * @return
  */
  int* getVOI();

  /**
  * @brief Returns the sample rate for generating the output
  * @return
  */
  int* getSampleRate();

  /**
  * @brief Sets the volume of interest
  * @return
  */
  void setVOI(int* voi);

  /**
  * @brief Sets the sample rate for generating the output
  * @return
  */
  void setSampleRate(int* sampleRate);

protected:
  /**
  * @brief Prepares initial data for the widget to use based on the parent 
  * filter's output
  */
  void setupGui();

protected slots:
  /**
  * @brief Checks if the X Min value is greater than the X Max value.
  * If it is, the X Max value is set to the X Min value.
  */
  void updatedVoiXMin();

  /**
  * @brief Checks if the X Min value is greater than the X Max value.
  * If it is, the X Min value is set to the X Max value.
  */
  void updatedVoiXMax();

  /**
  * @brief Checks if the Y Min value is greater than the Y Max value.
  * If it is, the Y Max value is set to the Y Min value.
  */
  void updatedVoiYMin();

  /**
  * @brief Checks if the Y Min value is greater than the Y Max value.
  * If it is, the Y Min value is set to the Y Max value.
  */
  void updatedVoiYMax();

  /**
  * @brief Checks if the Z Min value is greater than the Z Max value.
  * If it is, the Z Max value is set to the Z Min value.
  */
  void updatedVoiZMin();

  /**
  * @brief Checks if the Z Min value is greater than the Z Max value.
  * If it is, the Z Min value is set to the Z Max value.
  */
  void updatedVoiZMax();

private:
  class vsInternals;
  vsInternals*                    m_Internals;

  VSCropFilter*                   m_CropFilter;
};
