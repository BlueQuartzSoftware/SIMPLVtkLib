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

#include "VSSliceFilterWidget.h"

#include <QtCore/QString>

#include <vtkDataSet.h>
#include <vtkPlane.h>

#include "SIMPLVtkLib/QtWidgets/VSMainWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"

#include "ui_VSSliceFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VSSliceFilterWidget::vsInternals : public Ui::VSSliceFilterWidget
{
public:
  vsInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilterWidget::VSSliceFilterWidget(VSSliceFilter* filter, vtkRenderWindowInteractor *interactor, QWidget* parent)
: VSAbstractFilterWidget(parent)
, m_Internals(new vsInternals())
, m_SliceFilter(filter)
{
  m_Internals->setupUi(this);

  connect(m_SliceFilter->getTransform(), SIGNAL(valuesChanged()), this, SLOT(updateTransform()));

  m_SliceWidget = new VSPlaneWidget(this, m_SliceFilter->getTransform(), m_SliceFilter->getTransformBounds(), interactor);
  m_SliceWidget->show();

  connect(m_SliceWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));

  if (m_SliceFilter->isInitialized() == true)
  {
    m_SliceFilter->setInitialized(false);
    reset();
    apply();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilterWidget::~VSSliceFilterWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilterWidget::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }

  m_SliceWidget->setBounds(bounds);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilterWidget::apply()
{
  VSAbstractFilterWidget::apply();

  double origin[3];
  double normal[3];

  m_SliceWidget->getOrigin(origin);
  m_SliceWidget->getNormals(normal);
  m_SliceWidget->drawPlaneOff();

  m_SliceFilter->apply(origin, normal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilterWidget::reset()
{
  double* origin = m_SliceFilter->getLastOrigin();
  double* normal = m_SliceFilter->getLastNormal();

  m_SliceWidget->setNormals(normal);
  m_SliceWidget->setOrigin(origin);
  m_SliceWidget->updatePlaneWidget();
  m_SliceWidget->drawPlaneOff();

  cancelChanges();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilterWidget::setRenderingEnabled(bool enabled)
{
  VSAbstractFilterWidget::setRenderingEnabled(enabled);

  (enabled) ? m_SliceWidget->enable() : m_SliceWidget->disable();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilterWidget::setInteractor(vtkRenderWindowInteractor* interactor)
{
  bool rendered = getRenderingEnabled();

  setRenderingEnabled(false);
  m_SliceWidget->setInteractor(interactor);
  setRenderingEnabled(rendered);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilterWidget::updateTransform()
{
  setBounds(m_SliceFilter->getTransformBounds());
}
