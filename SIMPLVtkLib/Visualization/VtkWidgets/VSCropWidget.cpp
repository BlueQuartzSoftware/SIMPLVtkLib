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

#include "VSCropWidget.h"

#include <vtkImageData.h>
#include <vtkImplicitFunction.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

#include <QSpinBox>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropWidget::VSCropWidget(QWidget* parent, vtkSmartPointer<vtkImageData> imageData, vtkRenderWindowInteractor* iren)
: VSAbstractWidget(parent, imageData->GetBounds(), iren)
{
  setupUi(this);

  int imageExtent[6];
  imageData->GetExtent(imageExtent);

  m_ImageData = imageData;

  int* sampleRate = new int[3]{1, 1, 1};

  xMinSpinBox->setMinimum(imageExtent[0]);
  xMaxSpinBox->setMinimum(imageExtent[0]);

  xMinSpinBox->setMaximum(imageExtent[1]);
  xMaxSpinBox->setMaximum(imageExtent[1]);

  yMinSpinBox->setMinimum(imageExtent[2]);
  yMaxSpinBox->setMinimum(imageExtent[2]);

  yMinSpinBox->setMaximum(imageExtent[3]);
  yMaxSpinBox->setMaximum(imageExtent[3]);

  zMinSpinBox->setMinimum(imageExtent[4]);
  zMaxSpinBox->setMinimum(imageExtent[4]);

  zMinSpinBox->setMaximum(imageExtent[5]);
  zMaxSpinBox->setMaximum(imageExtent[5]);

  setVOI(imageExtent);
  setSampleRate(sampleRate);

  connect(xMinSpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));
  connect(xMaxSpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));

  connect(yMinSpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));
  connect(yMaxSpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));

  connect(zMinSpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));
  connect(zMaxSpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));

  connect(sampleISpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));
  connect(sampleJSpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));
  connect(sampleKSpinBox, SIGNAL(editingFinished()), this, SIGNAL(modified()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropWidget::~VSCropWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropWidget::getVOI()
{
  int* voi = new int[6];

  voi[0] = xMinSpinBox->value();
  voi[1] = xMaxSpinBox->value();
  voi[2] = yMinSpinBox->value();
  voi[3] = yMaxSpinBox->value();
  voi[4] = zMinSpinBox->value();
  voi[5] = zMaxSpinBox->value();

  return voi;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropWidget::getSampleRate()
{
  int* sampleRate = new int[3];

  sampleRate[0] = sampleISpinBox->value();
  sampleRate[1] = sampleJSpinBox->value();
  sampleRate[2] = sampleKSpinBox->value();

  return sampleRate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropWidget::setVOI(int* voi)
{
  xMinSpinBox->setValue(voi[0]);
  xMaxSpinBox->setValue(voi[1]);

  yMinSpinBox->setValue(voi[2]);
  yMaxSpinBox->setValue(voi[3]);

  zMinSpinBox->setValue(voi[4]);
  zMaxSpinBox->setValue(voi[5]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropWidget::setSampleRate(int* sampleRate)
{
  sampleISpinBox->setValue(sampleRate[0]);
  sampleJSpinBox->setValue(sampleRate[1]);
  sampleKSpinBox->setValue(sampleRate[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropWidget::enable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropWidget::disable()
{
}
