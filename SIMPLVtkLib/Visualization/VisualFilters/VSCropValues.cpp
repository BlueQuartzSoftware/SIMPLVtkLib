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

#include "VSCropValues.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropValues::VSCropValues(VSCropFilter* filter)
: VSAbstractFilterValues(filter)
, m_Voi(new int[6])
, m_SampleRate(new int[3])
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropValues::~VSCropValues()
{
  delete[] m_Voi;
  delete[] m_SampleRate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropValues::applyValues()
{
  VSAbstractFilter::FilterListType filters = getSelection();
  for(VSAbstractFilter* filter : filters)
  {
    // Make sure this is the appropriate filter type first
    FilterType* filterType = dynamic_cast<FilterType*>(filter);
    if(filterType)
    {
      filterType->applyValues(this);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropValues::resetValues()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSCropValues::hasChanges() const
{
  VSCropFilter* filter = dynamic_cast<VSCropFilter*>(getFilter());

  int* lastVoi = filter->getVOI();
  int* lastSampleRate = filter->getSampleRate();

  for(int i = 0; i < 3; i++)
  {
    if(m_Voi[i * 2] != lastVoi[i * 2] || m_Voi[i * 2 + 1] != lastVoi[i * 2 + 1])
    {
      return true;
    }

    if(m_SampleRate[i] != lastSampleRate[i])
    {
      return true;
    }
  }

  return false;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VSCropValues::createFilterWidget()
{
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropValues::getVOI() const
{
  return m_Voi;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropValues::getSampleRate() const
{
  return m_SampleRate;
}