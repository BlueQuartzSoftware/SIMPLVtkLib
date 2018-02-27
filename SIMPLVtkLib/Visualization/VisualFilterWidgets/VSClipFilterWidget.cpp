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

#include "VSClipFilterWidget.h"

#include <QApplication>
#include <QtCore/QString>

#include <vtkDataArray.h>
#include <vtkDataSet.h>

#include "ui_VSClipFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VSClipFilterWidget::vsInternals : public Ui::VSClipFilterWidget
{
public:
  vsInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilterWidget::VSClipFilterWidget(VSClipFilter* filter, vtkRenderWindowInteractor* interactor, QWidget* widget)
: VSAbstractFilterWidget(widget)
, m_Internals(new vsInternals())
, m_ClipFilter(filter)
{
  m_Internals->setupUi(this);

  connect(m_ClipFilter->getTransform(), SIGNAL(valuesChanged()), this, SLOT(updateTransform()));

  m_PlaneWidget = new VSPlaneWidget(nullptr, m_ClipFilter->getTransform(), m_ClipFilter->getTransformBounds(), interactor);
  m_BoxWidget = new VSBoxWidget(nullptr, m_ClipFilter->getTransform(), m_ClipFilter->getTransformBounds(), interactor);

  changeClipType(static_cast<int>(VSClipFilter::ClipType::PLANE));

  connect(m_Internals->clipTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeClipType(int)));

  connect(m_PlaneWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));
  connect(m_BoxWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));
  connect(m_Internals->insideOutCheckBox, SIGNAL(clicked()), this, SLOT(changesWaiting()));

  if (m_ClipFilter->isInitialized() == true)
  {
    m_ClipFilter->setInitialized(false);
    reset();
    apply();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilterWidget::~VSClipFilterWidget()
{
  delete m_PlaneWidget;
  delete m_BoxWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }

  m_PlaneWidget->setBounds(bounds);
  m_BoxWidget->setBounds(bounds);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::changeClipType(int clipTypei)
{
  m_Internals->clipTypeComboBox->setCurrentIndex(clipTypei);
  VSClipFilter::ClipType clipType = static_cast<VSClipFilter::ClipType>(clipTypei);

  // Remove and disable any unrelated widgets
  // Add and enable the corresponding widget
  switch(clipType)
  {
  case VSClipFilter::ClipType::PLANE:
    m_Internals->gridLayout->removeWidget(m_BoxWidget);
    m_BoxWidget->disable();
    m_BoxWidget->setParent(nullptr);

    m_PlaneWidget->enable();
    m_Internals->gridLayout->addWidget(m_PlaneWidget);
    m_PlaneWidget->updatePlaneWidget();
    break;
  case VSClipFilter::ClipType::BOX:
    m_Internals->gridLayout->removeWidget(m_PlaneWidget);
    m_PlaneWidget->disable();
    m_PlaneWidget->setParent(nullptr);

    m_BoxWidget->enable();
    m_Internals->gridLayout->addWidget(m_BoxWidget);
    m_BoxWidget->updateBoxWidget();
    break;
  default:
    // Remove and disable all widgets
    m_Internals->gridLayout->removeWidget(m_PlaneWidget);
    m_PlaneWidget->disable();
    m_PlaneWidget->setParent(nullptr);

    m_Internals->gridLayout->removeWidget(m_BoxWidget);
    m_BoxWidget->disable();
    m_BoxWidget->setParent(nullptr);
    break;
  }

  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::apply()
{
  VSAbstractFilterWidget::apply();

  int clipTypei = m_Internals->clipTypeComboBox->currentIndex();
  VSClipFilter::ClipType clipType = static_cast<VSClipFilter::ClipType>(clipTypei);

  switch(clipType)
  {
  case VSClipFilter::ClipType::PLANE:
    {
      double normal[3];
      double origin[3];
      m_PlaneWidget->getNormals(normal);
      m_PlaneWidget->getOrigin(origin);
      m_PlaneWidget->drawPlaneOff();

      m_ClipFilter->apply(origin, normal, m_Internals->insideOutCheckBox->isChecked());
      m_ClipFilter->setInitialized(false);
    }
    break;
  case VSClipFilter::ClipType::BOX:
    {
      VTK_PTR(vtkTransform) boxTransform = m_BoxWidget->getTransform();
      VTK_PTR(vtkPlanes) planes = m_BoxWidget->getPlanes();

      m_ClipFilter->apply(planes, boxTransform, m_Internals->insideOutCheckBox->isChecked());
      m_ClipFilter->setInitialized(false);
    }
    break;
  default:
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::reset()
{ 
  VSClipFilter::ClipType clipType = m_ClipFilter->getLastClipType();
  
  // Set the inverted variable based on the last applied clip type
  bool inverted = false;
  switch(clipType)
  {
  case VSClipFilter::ClipType::PLANE:
    inverted = m_ClipFilter->getLastPlaneInverted();
    break;
  case VSClipFilter::ClipType::BOX:
    inverted = m_ClipFilter->getLastBoxInverted();
    break;
  default:
    inverted = false;
    break;
  }
  m_Internals->insideOutCheckBox->setChecked(inverted);
  
  // Reset Plane Type
  {
    double* origin = m_ClipFilter->getLastPlaneOrigin();
    double* normal = m_ClipFilter->getLastPlaneNormal();
    
    m_PlaneWidget->setNormals(normal);
    m_PlaneWidget->setOrigin(origin);
    m_PlaneWidget->updatePlaneWidget();
    m_PlaneWidget->drawPlaneOff();
  }
  
  // Reset Box Type
  {
    VTK_PTR(vtkTransform) transform = m_ClipFilter->getLastBoxTransform();
    m_BoxWidget->setTransform(transform);
  }

  changeClipType(static_cast<int>(clipType));
  cancelChanges();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::setInteractor(vtkRenderWindowInteractor* interactor)
{
  bool rendered = getRenderingEnabled();
  setRenderingEnabled(false);

  if (m_PlaneWidget)
  {
    m_PlaneWidget->setInteractor(interactor);
  }

  if (m_BoxWidget)
  {
    m_BoxWidget->setInteractor(interactor);
  }

  setRenderingEnabled(rendered);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::setRenderingEnabled(bool enabled)
{
  VSAbstractFilterWidget::setRenderingEnabled(enabled);

  if (m_Internals->clipTypeComboBox->currentIndex() == static_cast<int>(VSClipFilter::ClipType::PLANE))
  {
    (enabled) ? m_PlaneWidget->enable() : m_PlaneWidget->disable();
  }
  else
  {
    (enabled) ? m_BoxWidget->enable() : m_BoxWidget->disable();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::updateTransform()
{
  setBounds(m_ClipFilter->getTransformBounds());
}
