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

#include "VSLookupTableController.h"

#include <vtkColorTransferFunction.h>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include <limits>

std::vector<VSLookupTableController::RgbPos_t> VSLookupTableController::m_DefaultRgbPosVector = {VSLookupTableController::RgbPos_t{0, 0.231373, 0.298039, 0.752941},
                                                                                                 VSLookupTableController::RgbPos_t{0.5, 0.865003, 0.865003, 0.865003},
                                                                                                 VSLookupTableController::RgbPos_t{1, 0.705882, 0.0156863, 0.14902}};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSLookupTableController::VSLookupTableController()
{
  createColorTransferFunction();
  m_Range = new double[2];
  m_Range[0] = 0.0;
  m_Range[1] = 1.0;

  m_BaseRange = new double[2];
  m_BaseRange[0] = 0.0;
  m_BaseRange[1] = 1.0;

  double nan[3] = {0.5, 0.75, 0.0};
  setNANColor(nan);

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSLookupTableController::~VSLookupTableController()
{
  m_ColorTransferFunction->Delete();
  delete[] m_Range;
  delete[] m_BaseRange;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::createColorTransferFunction()
{
  m_ColorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
  m_ColorTransferFunction->SetColorSpaceToDiverging();
  m_RgbPosVector.clear();

  m_RgbPosVector = std::vector<RgbPos_t>(m_DefaultRgbPosVector.size());
  m_BaseRgbPosVector = std::vector<RgbPos_t>(m_DefaultRgbPosVector.size());
  for(int i = 0; i < m_DefaultRgbPosVector.size(); i++)
  {
    m_RgbPosVector[i] = m_DefaultRgbPosVector[i];
    m_BaseRgbPosVector[i] = m_DefaultRgbPosVector[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkSmartPointer<vtkColorTransferFunction> VSLookupTableController::getColorTransferFunction()
{
  return m_ColorTransferFunction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::parseRgbJson(const QJsonObject& presetObj)
{
  QJsonArray presetArray = presetObj["RGBPoints"].toArray();

  int numColors = presetArray.count() / 4;
  int numComponents = 4;

  double minValue = std::numeric_limits<double>::max();
  double maxValue = std::numeric_limits<double>::min();

  m_RgbPosVector.resize(numColors);
  m_BaseRgbPosVector.resize(numColors);

  m_ColorTransferFunction->RemoveAllPoints();
  for(int i = 0; i < numColors; i++)
  {
    RgbPos_t pos;
    pos.x = static_cast<float>(presetArray[i * numComponents + 0].toDouble());

    pos.r = static_cast<float>(presetArray[i * numComponents + 1].toDouble());
    pos.g = static_cast<float>(presetArray[i * numComponents + 2].toDouble());
    pos.b = static_cast<float>(presetArray[i * numComponents + 3].toDouble());

    if(pos.x < minValue)
    {
      minValue = pos.x;
    }
    if(pos.x > maxValue)
    {
      maxValue = pos.x;
    }

    m_RgbPosVector[i] = pos;
    m_BaseRgbPosVector[i] = pos;

    m_ColorTransferFunction->AddRGBPoint(pos.x, pos.r, pos.g, pos.b);
  }

  double minRange = m_Range[0];
  double maxRange = m_Range[1];

  m_Range[0] = minValue;
  m_Range[1] = maxValue;

  m_BaseRange[0] = minValue;
  m_BaseRange[1] = maxValue;

  m_ColorTransferFunction->SetColorSpaceToRGB();

  setRange(minRange, maxRange);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::copy(const VSLookupTableController& other)
{
  m_RgbPosVector.clear();
  m_BaseRgbPosVector.clear();

  m_ColorTransferFunction->RemoveAllPoints();

  size_t count = other.m_RgbPosVector.size();
  m_RgbPosVector.resize(count);
  for(size_t i = 0; i < count; i++)
  {
    m_RgbPosVector[i] = other.m_RgbPosVector[i];
    RgbPos_t pos = m_RgbPosVector[i];

    m_ColorTransferFunction->AddRGBPoint(pos.x, pos.r, pos.g, pos.b);
  }

  count = other.m_BaseRgbPosVector.size();
  m_BaseRgbPosVector.resize(count);
  for(size_t i = 0; i < count; i++)
  {
    m_BaseRgbPosVector[i] = other.m_BaseRgbPosVector[i];
  }

  int colorSpace = other.m_ColorTransferFunction->GetColorSpace();
  m_ColorTransferFunction->SetColorSpace(colorSpace);

  m_Range[0] = other.m_Range[0];
  m_Range[1] = other.m_Range[1];

  m_BaseRange[0] = other.m_BaseRange[0];
  m_BaseRange[1] = other.m_BaseRange[1];

  setRange(m_Range[0], m_Range[1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSLookupTableController* VSLookupTableController::deepCopy()
{
  VSLookupTableController* other = new VSLookupTableController();
  other->copy(*this);
  return other;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::invert()
{
  double minRange = m_Range[0];
  double maxRange = m_Range[1];

  normalizePositions();

  size_t size = m_RgbPosVector.size();
  std::vector<RgbPos_t> newVector(size);

  for(size_t i = 0; i < size; i++)
  {
    newVector[i] = m_RgbPosVector[size - i - 1];
    newVector[i].x = 1.0 - newVector[i].x;
  }

  m_RgbPosVector = newVector;
  m_BaseRgbPosVector = newVector;

  setRange(minRange, maxRange);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::normalizePositions()
{
  double rangeLength = m_BaseRange[1] - m_BaseRange[0];

  for(int i = 0; i < m_BaseRgbPosVector.size(); i++)
  {
    RgbPos_t pos = m_BaseRgbPosVector[i];
    pos.x = (pos.x - m_BaseRange[0]) / rangeLength;
    m_RgbPosVector[i] = pos;
    m_BaseRgbPosVector[i] = pos;
  }

  m_Range[0] = 0.0;
  m_Range[1] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction)
{
  m_ColorTransferFunction = colorTransferFunction;
  m_RgbPosVector.clear();

  double val[6];
  for(int i = 0; colorTransferFunction->GetNodeValue(i, val) != -1; i++)
  {
    RgbPos_t pos;
    pos.x = val[0];

    pos.r = val[1];
    pos.g = val[2];
    pos.b = val[3];

    m_RgbPosVector.push_back(pos);
  }

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSLookupTableController::getRange()
{
  return m_Range;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setRange(double range[2])
{
  setRange(range[0], range[1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setRange(double min, double max)
{
  normalizePositions();

  double rangeLength = max - min;

  // skew points to fit new range
  for(int i = 0; i < m_RgbPosVector.size(); i++)
  {
    RgbPos_t pos = m_BaseRgbPosVector[i];
    pos.x = pos.x * rangeLength + min;
    m_RgbPosVector[i] = pos;
  }

  m_Range[0] = min;
  m_Range[1] = max;

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::addRgbPoint(double x, double r, double g, double b)
{
  RgbPos_t pos;
  pos.x = x;

  pos.r = r;
  pos.g = g;
  pos.b = b;

  addRgbPoint(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::addRgbPoint(double x, double rgb[3])
{
  RgbPos_t pos;
  pos.x = x;

  pos.r = rgb[0];
  pos.g = rgb[1];
  pos.b = rgb[2];

  addRgbPoint(pos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::addRgbPoint(VSLookupTableController::RgbPos_t rgbPos)
{
  m_RgbPosVector.push_back(rgbPos);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSLookupTableController::getNumberOfRgbPoints()
{
  return (int)m_RgbPosVector.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSLookupTableController::getRgbColor(int index)
{
  double* color = new double[3];

  if(index < m_RgbPosVector.size() && index >= 0)
  {
    RgbPos_t rgb = m_RgbPosVector[index];

    color[0] = rgb.r;
    color[1] = rgb.g;
    color[2] = rgb.b;
  }
  else
  {
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
  }

  return color;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setColor(int index, double r, double g, double b)
{
  if(index >= m_RgbPosVector.size() || index < 0)
  {
    return;
  }

  RgbPos_t rgbPos = m_RgbPosVector[index];
  rgbPos.r = r;
  rgbPos.g = g;
  rgbPos.b = b;

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setColor(int index, double rgb[3])
{
  if(index >= m_RgbPosVector.size() || index < 0)
  {
    return;
  }

  RgbPos_t rgbPos = m_RgbPosVector[index];
  rgbPos.r = rgb[0];
  rgbPos.g = rgb[1];
  rgbPos.b = rgb[2];

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSLookupTableController::getRgbPosition(int index)
{
  if(index >= m_RgbPosVector.size() || index < 0)
  {
    return 0;
  }

  return m_RgbPosVector[index].x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setRgbPositon(int index, double x)
{
  if(index >= m_RgbPosVector.size() || index < 0)
  {
    return;
  }

  m_RgbPosVector[index].x = x;
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::removeRgbPoint(int index)
{
  if(index >= m_RgbPosVector.size() || index < 0)
  {
    return;
  }

  m_RgbPosVector.erase(m_RgbPosVector.begin() + index);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::update()
{
  if(nullptr == m_ColorTransferFunction)
  {
    return;
  }

  m_ColorTransferFunction->RemoveAllPoints();

  for(int i = 0; i < m_RgbPosVector.size(); i++)
  {
    RgbPos_t colorPos = m_RgbPosVector[i];

    m_ColorTransferFunction->AddRGBPoint(colorPos.x, colorPos.r, colorPos.g, colorPos.b);
  }

  double nan[3] = {0.5, 0.75, 0.0};
  setNANColor(nan);
  m_ColorTransferFunction->SetBelowRangeColor(nan);
  m_ColorTransferFunction->SetAboveRangeColor(nan);
  m_ColorTransferFunction->SetNanColor(nan);
  m_ColorTransferFunction->Build();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSLookupTableController::equals(VSLookupTableController* other)
{
  if(nullptr == other)
  {
    return false;
  }

  if(other->m_RgbPosVector.size() != m_RgbPosVector.size())
  {
    return false;
  }

  if(other->m_Range[0] != m_Range[0] || other->m_Range[1] != m_Range[1])
  {
    return false;
  }

  for(int i = 0; i < m_RgbPosVector.size(); i++)
  {
    if(m_RgbPosVector[i].x != other->m_RgbPosVector[i].x)
    {
      return false;
    }
    if(m_RgbPosVector[i].r != other->m_RgbPosVector[i].r)
    {
      return false;
    }
    if(m_RgbPosVector[i].g != other->m_RgbPosVector[i].g)
    {
      return false;
    }
    if(m_RgbPosVector[i].b != other->m_RgbPosVector[i].b)
    {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setNANColor(double color[3])
{
  m_ColorTransferFunction->SetNanColor(color);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSLookupTableController::getNANColor()
{
  return m_ColorTransferFunction->GetNanColor();
}
