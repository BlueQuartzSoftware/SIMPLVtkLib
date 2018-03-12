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

#include <QtCore/QString>

#include <QtCore/QJsonArray>
#include <QtCore/QUuid>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
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
  setToolTip(getToolTip());

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
VSCropFilter* VSCropFilter::Create(QJsonObject &json, VSAbstractFilter* parent)
{
  VSCropFilter* filter = new VSCropFilter(parent);

  QJsonArray voiArray = json["Last VOI"].toArray();
  int lastVOI[6];
  lastVOI[0] = voiArray.at(0).toInt();
  lastVOI[1] = voiArray.at(1).toInt();
  lastVOI[2] = voiArray.at(2).toInt();
  lastVOI[3] = voiArray.at(3).toInt();
  lastVOI[4] = voiArray.at(4).toInt();
  lastVOI[5] = voiArray.at(5).toInt();
  filter->setVOI(lastVOI);

  QJsonArray sampleRateArray = json["Last Sample Rate"].toArray();
  int lastSampleRate[3];
  lastSampleRate[0] = sampleRateArray.at(0).toInt();
  lastSampleRate[1] = sampleRateArray.at(1).toInt();
  lastSampleRate[2] = sampleRateArray.at(2).toInt();
  filter->setSampleRate(lastSampleRate);

  filter->setInitialized(true);
  filter->readTransformJson(json);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::createFilter()
{
  m_CropAlgorithm = vtkSmartPointer<vtkExtractVOI>::New();
  m_CropAlgorithm->IncludeBoundaryOn();
  m_CropAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());

  setConnectedInput(true);
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
void VSCropFilter::writeJson(QJsonObject &json)
{
  VSAbstractFilter::writeJson(json);

  QJsonArray lastVOI;
  lastVOI.append(m_LastVoi[0]);
  lastVOI.append(m_LastVoi[1]);
  lastVOI.append(m_LastVoi[2]);
  lastVOI.append(m_LastVoi[3]);
  lastVOI.append(m_LastVoi[4]);
  lastVOI.append(m_LastVoi[5]);
  json["Last VOI"] = lastVOI;

  QJsonArray lastSampleRate;
  lastSampleRate.append(m_LastSampleRate[0]);
  lastSampleRate.append(m_LastSampleRate[1]);
  lastSampleRate.append(m_LastSampleRate[2]);
  json["Last Sample Rate"] = lastSampleRate;

  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSCropFilter::GetUuid()
{
  return QUuid("{65a3f063-5054-5abe-b518-3a2884d96b1c}");
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
QString VSCropFilter::getToolTip() const
{
  return "Crop Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSCropFilter::getOutputPort()
{
  if(getConnectedInput() && m_CropAlgorithm)
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
  if(getConnectedInput() && m_CropAlgorithm)
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

  setInputPort(filter->getOutputPort());

  if(getConnectedInput() && m_CropAlgorithm)
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
bool VSCropFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  if(compatibleInput(filter->getOutputType(), getRequiredInputType()))
  {
    return true;
  }

  return false;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::setVOI(int* voi)
{
  m_LastVoi[0] = voi[0];
  m_LastVoi[1] = voi[1];
  m_LastVoi[2] = voi[2];
  m_LastVoi[3] = voi[3];
  m_LastVoi[4] = voi[4];
  m_LastVoi[5] = voi[5];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::setSampleRate(int* sampleRate)
{
  m_LastSampleRate[0] = sampleRate[0];
  m_LastSampleRate[1] = sampleRate[1];
  m_LastSampleRate[2] = sampleRate[2];
}
