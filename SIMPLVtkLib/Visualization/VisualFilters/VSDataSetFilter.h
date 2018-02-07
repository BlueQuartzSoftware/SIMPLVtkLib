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

#include "VSAbstractFilter.h"

#include <vtkSmartPointer.h>

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkTrivialProducer;
class vtkAlgorithmOutput;

/**
* @class VSDataSetFilter VSDataSetFilter.h
* SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h
* @brief This class stores a WrappedDataContainerPtr and provides an output port
* for other filters to connect to for converting SIMPLib DataContainers to something 
* VTK can render.
*/
class SIMPLVtkLib_EXPORT VSDataSetFilter : public VSAbstractFilter
{
  Q_OBJECT

public:
  /**
  * @brief Constuctor
  * @param parentWidget
  * @param dataSetStruct
  */
  VSDataSetFilter(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDataContainer);

  /**
  * @brief Returns the bounds of the vtkDataSet
  * @return
  */
  double* getBounds() const;

  /**
  * @brief Returns the output port for the filter
  * @return
  */
  virtual vtkAlgorithmOutput* getOutputPort() override;

  /**
  * @brief Returns the output data for the filter
  */
  VTK_PTR(vtkDataSet) getOutput() override;

  /**
  * @brief Returns the filter's name
  * @return
  */
  const QString getFilterName() override;

  /**
  * @brief Returns the output data type for the filter
  * @return
  */
  dataType_t getOutputType() override;

  /**
  * @brief Returns the required input data type
  * @return
  */
  static dataType_t getRequiredInputType();

  /**
  * @brief Returns the VtkDataSetStruct_t used by the filter
  * @return
  */
  SIMPLVtkBridge::WrappedDataContainerPtr getWrappedDataContainer() override;

protected:
  /**
  * @brief Initializes the trivial producer and connects it to the vtkMapper
  */
  void createFilter() override;

  /**
  * @brief This method is empty as there should never be a case where a VSDataSetFilter
  * is a child of another filter.
  * @param filter
  */
  void updateAlgorithmInput(VSAbstractFilter* filter) override;

private:
  SIMPLVtkBridge::WrappedDataContainerPtr m_WrappedDataContainer = nullptr;
  VTK_PTR(vtkTrivialProducer) m_TrivialProducer = nullptr;
};
