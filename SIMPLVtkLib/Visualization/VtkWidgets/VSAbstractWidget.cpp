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

#include "VSAbstractWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractWidget::VSAbstractWidget(QWidget* parent, VSTransform* transform, double bounds[6], vtkRenderWindowInteractor* iren)
: QWidget(parent)
, m_RenderWindowInteractor(iren)
, m_Transform(transform)
{
  setBounds(bounds);

  connect(transform, SIGNAL(valuesChanged()), this, SLOT(updateGlobalSpace()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractWidget::getBounds(double bounds[6])
{
  for(int i = 0; i < 6; i++)
  {
    bounds[i] = this->m_Bounds[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractWidget::getBounds()
{
  return m_Bounds;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractWidget::calculateLocalOrigin(double* bounds, VSTransform* transform)
{
  double* origin = calculateGlobalOrigin(bounds);
  transform->localizePoint(origin);
  return origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractWidget::calculateGlobalOrigin(double* bounds)
{
  double* origin = new double[3];
  for(int i = 0; i < 3; i++)
  {
    origin[i] = (bounds[i * 2] + bounds[i * 2 + 1]) / 2.0;
  }

  return origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractWidget::setBounds(double bounds[6])
{
  for(int i = 0; i < 3; i++)
  {
    double offset = 0;
    if(bounds[i * 2 + 1] - bounds[i * 2] < MIN_SIZE)
    {
      offset = (MIN_SIZE - (bounds[i * 2 + 1] - bounds[i * 2])) / 2.0;
    }

    this->m_Bounds[i * 2] = bounds[i * 2] - offset;
    this->m_Bounds[i * 2 + 1] = bounds[i * 2 + 1] + offset;
  }

  updateBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderWindowInteractor* VSAbstractWidget::getInteractor()
{
  return m_RenderWindowInteractor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractWidget::setInteractor(vtkRenderWindowInteractor* interactor)
{
  m_RenderWindowInteractor = interactor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractWidget::updateBounds()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform* VSAbstractWidget::getVSTransform()
{
  return m_Transform;
}
