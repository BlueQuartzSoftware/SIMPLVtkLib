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

std::vector<VSLookupTableController::RgbPos_t> VSLookupTableController::m_defaultRgbPosVector = {VSLookupTableController::RgbPos_t{0, 0.231373, 0.298039, 0.752941},
                                                                                                   VSLookupTableController::RgbPos_t{0.5, 0.865003, 0.865003, 0.865003},
                                                                                                   VSLookupTableController::RgbPos_t{1, 0.705882, 0.0156863, 0.14902}};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSLookupTableController::VSLookupTableController()
{
  createColorTransferFunction();
  m_range = new double[2];
  m_range[0] = 0.0;
  m_range[1] = 1.0;

  m_baseRange = new double[2];
  m_baseRange[0] = 0.0;
  m_baseRange[1] = 1.0;

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSLookupTableController::~VSLookupTableController()
{
  m_colorTransferFunction->Delete();
  delete[] m_range;
  delete[] m_baseRange;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::createColorTransferFunction()
{
  m_colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
  m_colorTransferFunction->SetColorSpaceToDiverging();
  m_rgbPosVector.clear();

  m_rgbPosVector = std::vector<RgbPos_t>(m_defaultRgbPosVector.size());
  m_baseRgbPosVector = std::vector<RgbPos_t>(m_defaultRgbPosVector.size());
  for(int i = 0; i < m_defaultRgbPosVector.size(); i++)
  {
    m_rgbPosVector[i] = m_defaultRgbPosVector[i];
    m_baseRgbPosVector[i] = m_defaultRgbPosVector[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkSmartPointer<vtkColorTransferFunction> VSLookupTableController::getColorTransferFunction()
{
  return m_colorTransferFunction;
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

  m_rgbPosVector.clear();
  m_baseRgbPosVector.clear();

  m_colorTransferFunction->RemoveAllPoints();
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

    m_rgbPosVector.push_back(pos);
    m_baseRgbPosVector.push_back(pos);

    m_colorTransferFunction->AddRGBPoint(pos.x, pos.r, pos.g, pos.b);
  }

  double minRange = m_range[0];
  double maxRange = m_range[1];

  m_range[0] = minValue;
  m_range[1] = maxValue;

  m_baseRange[0] = minValue;
  m_baseRange[1] = maxValue;

  m_colorTransferFunction->SetColorSpaceToRGB();

  setRange(minRange, maxRange);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::copy(const VSLookupTableController& other)
{
  m_rgbPosVector.clear();
  m_baseRgbPosVector.clear();

  m_colorTransferFunction->RemoveAllPoints();

  size_t count = other.m_rgbPosVector.size();
  m_rgbPosVector.resize(count);
  for(size_t i = 0; i < count; i++)
  {
    m_rgbPosVector[i] = other.m_rgbPosVector[i];
    RgbPos_t pos = m_rgbPosVector[i];

    m_colorTransferFunction->AddRGBPoint(pos.x, pos.r, pos.g, pos.b);
  }

  count = other.m_baseRgbPosVector.size();
  m_baseRgbPosVector.resize(count);
  for(size_t i = 0; i < count; i++)
  {
    m_baseRgbPosVector[i] = other.m_baseRgbPosVector[i];
  }

  int colorSpace = other.m_colorTransferFunction->GetColorSpace();
  m_colorTransferFunction->SetColorSpace(colorSpace);

  m_range[0] = other.m_range[0];
  m_range[1] = other.m_range[1];

  m_baseRange[0] = other.m_baseRange[0];
  m_baseRange[1] = other.m_baseRange[1];

  setRange(m_range[0], m_range[1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::invert()
{
  double minRange = m_range[0];
  double maxRange = m_range[1];

  normalizePositions();

  size_t size = m_rgbPosVector.size();
  std::vector<RgbPos_t> newVector(size);

  for(size_t i = 0; i < size; i++)
  {
    newVector[i] = m_rgbPosVector[size - i - 1];
    newVector[i].x = 1.0 - newVector[i].x;
  }

  m_rgbPosVector = newVector;
  m_baseRgbPosVector = newVector;

  setRange(minRange, maxRange);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::normalizePositions()
{
  double rangeLength = m_baseRange[1] - m_baseRange[0];

  for(int i = 0; i < m_baseRgbPosVector.size(); i++)
  {
    RgbPos_t pos = m_baseRgbPosVector[i];
    pos.x = (pos.x - m_baseRange[0]) / rangeLength;
    m_rgbPosVector[i] = pos;
    m_baseRgbPosVector[i] = pos;
  }

  m_range[0] = 0.0;
  m_range[1] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction)
{
  m_colorTransferFunction = colorTransferFunction;
  m_rgbPosVector.clear();

  double val[6];
  for(int i = 0; colorTransferFunction->GetNodeValue(i, val) != -1; i++)
  {
    RgbPos_t pos;
    pos.x = val[0];

    pos.r = val[1];
    pos.g = val[2];
    pos.b = val[3];

    m_rgbPosVector.push_back(pos);
  }

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSLookupTableController::getRange()
{
  return m_range;
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
  for(int i = 0; i < m_rgbPosVector.size(); i++)
  {
    RgbPos_t pos = m_baseRgbPosVector[i];
    pos.x = pos.x * rangeLength + min;
    m_rgbPosVector[i] = pos;
  }

  m_range[0] = min;
  m_range[1] = max;

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
  m_rgbPosVector.push_back(rgbPos);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSLookupTableController::getNumberOfRgbPoints()
{
  return (int)m_rgbPosVector.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSLookupTableController::getRgbColor(int index)
{
  double* color = new double[3];

  if(index < m_rgbPosVector.size() && index >= 0)
  {
    RgbPos_t rgb = m_rgbPosVector[index];

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
  if(index >= m_rgbPosVector.size() || index < 0)
  {
    return;
  }

  RgbPos_t rgbPos = m_rgbPosVector[index];
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
  if(index >= m_rgbPosVector.size() || index < 0)
  {
    return;
  }

  RgbPos_t rgbPos = m_rgbPosVector[index];
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
  if(index >= m_rgbPosVector.size() || index < 0)
  {
    return 0;
  }

  return m_rgbPosVector[index].x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::setRgbPositon(int index, double x)
{
  if(index >= m_rgbPosVector.size() || index < 0)
  {
    return;
  }

  m_rgbPosVector[index].x = x;
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::removeRgbPoint(int index)
{
  if(index >= m_rgbPosVector.size() || index < 0)
  {
    return;
  }

  m_rgbPosVector.erase(m_rgbPosVector.begin() + index);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLookupTableController::update()
{
  if(nullptr == m_colorTransferFunction)
  {
    return;
  }

  m_colorTransferFunction->RemoveAllPoints();

  for(int i = 0; i < m_rgbPosVector.size(); i++)
  {
    RgbPos_t colorPos = m_rgbPosVector[i];

    m_colorTransferFunction->AddRGBPoint(colorPos.x, colorPos.r, colorPos.g, colorPos.b);
  }

  m_colorTransferFunction->Build();
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

  if(other->m_rgbPosVector.size() != m_rgbPosVector.size())
  {
    return false;
  }

  if(other->m_range[0] != m_range[0] || other->m_range[1] != m_range[1])
  {
    return false;
  }

  for(int i = 0; i < m_rgbPosVector.size(); i++)
  {
    if(m_rgbPosVector[i].x != other->m_rgbPosVector[i].x)
    {
      return false;
    }
    if(m_rgbPosVector[i].r != other->m_rgbPosVector[i].r)
    {
      return false;
    }
    if(m_rgbPosVector[i].g != other->m_rgbPosVector[i].g)
    {
      return false;
    }
    if(m_rgbPosVector[i].b != other->m_rgbPosVector[i].b)
    {
      return false;
    }
  }

  return true;
}
