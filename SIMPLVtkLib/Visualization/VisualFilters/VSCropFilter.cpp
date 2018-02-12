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

#include "VSCropFilter.h"

#include <QString>

#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSCropWidget.h"
#include <vtkExtractVOI.h>

#include <vtkRenderWindowInteractor.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropFilter::VSCropFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_CropAlgorithm = nullptr;
  setText(getFilterName());

  for(int i = 0; i < 3; i++)
  {
    m_LastVoi[i] = 0;
    m_LastVoi[i+3] = 0;
    m_LastSampleRate[i] = 1;
  }

  m_CropAlgorithm = nullptr;
  setParentFilter(parent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::createFilter()
{
  m_CropAlgorithm = vtkSmartPointer<vtkExtractVOI>::New();
  m_CropAlgorithm->IncludeBoundaryOn();
  m_CropAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());

  m_ConnectedInput = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::apply(int voi[6], int sampleRate[3])
{
  if(nullptr == m_CropAlgorithm)
  {
    createFilter();
  }

  // Save the applied values for resetting Crop-Type widgets
  m_CropAlgorithm->SetVOI(voi);
  m_CropAlgorithm->SetSampleRate(sampleRate);
  m_CropAlgorithm->Update();

  for(int i = 0; i < 6; i++)
  {
    m_LastVoi[i] = voi[i];
  }

  for(int i = 0; i < 3; i++)
  {
    m_LastSampleRate[i] = sampleRate[i];
  }

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSCropFilter::getFilterName()
{
  return "Crop";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSCropFilter::getOutputPort()
{
  if(m_ConnectedInput && m_CropAlgorithm)
  {
    return m_CropAlgorithm->GetOutputPort();
  }
  else if(getParentFilter())
  {
    return getParentFilter()->getOutputPort();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) VSCropFilter::getOutput()
{
  if(m_ConnectedInput && m_CropAlgorithm)
  {
    return m_CropAlgorithm->GetOutput();
  }
  else if(getParentFilter())
  {
    return getParentFilter()->getOutput();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  m_InputPort = filter->getOutputPort();

  if(m_ConnectedInput && m_CropAlgorithm)
  {
    m_CropAlgorithm->SetInputConnection(filter->getOutputPort());
  }
  else
  {
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSCropFilter::getOutputType()
{
  return IMAGE_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSCropFilter::getRequiredInputType()
{
  return IMAGE_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropFilter::getVOI()
{
  return m_LastVoi;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropFilter::getSampleRate()
{
  return m_LastSampleRate;
}
