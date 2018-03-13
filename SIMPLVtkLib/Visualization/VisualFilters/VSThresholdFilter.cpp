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

#include "VSThresholdFilter.h"

#include <QtCore/QString>
#include <QtCore/QUuid>

#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkImplicitDataSet.h>
#include <vtkPointData.h>
#include <vtkThreshold.h>
#include <vtkUnstructuredGrid.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdFilter::VSThresholdFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_ThresholdAlgorithm = nullptr;
  setParentFilter(parent);
  setText(getFilterName());
  setToolTip(getToolTip());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdFilter* VSThresholdFilter::Create(QJsonObject &json, VSAbstractFilter* parent)
{
  VSThresholdFilter* filter = new VSThresholdFilter(parent);

  filter->setLastArrayName(json["Last Array Name"].toString());
  filter->setLastMinValue(json["Last Minimum Value"].toDouble());
  filter->setLastMaxValue(json["Last Maximum Value"].toDouble());

  filter->setInitialized(true);
  filter->readTransformJson(json);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::createFilter()
{
  m_ThresholdAlgorithm = VTK_PTR(vtkThreshold)::New();

  // Parent cell data required
  if(false == (getParentFilter() && getParentFilter()->getOutput() && getParentFilter()->getOutput()->GetCellData()))
  {
    return;
  }

  m_ThresholdAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());
  VTK_PTR(vtkDataArray) dataArray = getParentFilter()->getOutput()->GetCellData()->GetScalars();

  setConnectedInput(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSThresholdFilter::getFilterName()
{
  return "Threshold";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSThresholdFilter::getToolTip() const
{
  return "Threshold Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::apply(QString arrayName, double min, double max)
{
  if(nullptr == m_ThresholdAlgorithm)
  {
    createFilter();
  }

  // Save the applied values for resetting Threshold-Type widgets
  m_LastArrayName = arrayName;
  m_LastMinValue = min;
  m_LastMaxValue = max;

  m_ThresholdAlgorithm->ThresholdBetween(min, max);
  m_ThresholdAlgorithm->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, qPrintable(arrayName));
  m_ThresholdAlgorithm->Update();

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::readJson(QJsonObject &json)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::writeJson(QJsonObject &json)
{
  VSAbstractFilter::writeJson(json);

  json["Uuid"] = GetUuid().toString();
  json["Last Array Name"] = m_LastArrayName;
  json["Last Minimum Value"] = m_LastMinValue;
  json["Last Maximum Value"] = m_LastMaxValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSThresholdFilter::GetUuid()
{
  return QUuid("{cbd16e15-5a0a-5c46-8375-7974b481b57a}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSThresholdFilter::getOutputPort()
{
  if(getConnectedInput() && m_ThresholdAlgorithm)
  {
    return m_ThresholdAlgorithm->GetOutputPort();
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
VTK_PTR(vtkDataSet) VSThresholdFilter::getOutput()
{
  if(getConnectedInput() && m_ThresholdAlgorithm)
  {
    return m_ThresholdAlgorithm->GetOutput();
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
void VSThresholdFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  setInputPort(filter->getOutputPort());

  if(getConnectedInput() && m_ThresholdAlgorithm)
  {
    m_ThresholdAlgorithm->SetInputConnection(filter->getOutputPort());
  }
  else
  {
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSThresholdFilter::getOutputType()
{
  return UNSTRUCTURED_GRID;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSThresholdFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSThresholdFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  // Require scalar values
  vtkDataSet* output = filter->getOutput();
  if(output && output->GetCellData() && output->GetCellData()->GetScalars())
  {
    if(compatibleInput(filter->getOutputType(), getRequiredInputType()))
    {
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSThresholdFilter::getLastArrayName()
{
  return m_LastArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdFilter::getLastMinValue()
{
  return m_LastMinValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdFilter::getLastMaxValue()
{
  return m_LastMaxValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setLastArrayName(QString lastArrayName)
{
  m_LastArrayName = lastArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setLastMinValue(double lastMinValue)
{
  m_LastMinValue = lastMinValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setLastMaxValue(double lastMaxValue)
{
  m_LastMaxValue = lastMaxValue;
}
