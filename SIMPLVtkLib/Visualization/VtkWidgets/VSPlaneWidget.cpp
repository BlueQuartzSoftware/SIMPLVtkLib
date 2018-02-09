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

#include "VSPlaneWidget.h"

#include <vtkCommand.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkPlane.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkSmartPointer.h>

#include <QDoubleSpinBox>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

class vtkPlaneCallback : public vtkCommand
{
public:
  static vtkPlaneCallback* New()
  {
    return new vtkPlaneCallback();
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkImplicitPlaneWidget2* planeWidget = reinterpret_cast<vtkImplicitPlaneWidget2*>(caller);
    vtkImplicitPlaneRepresentation* rep = reinterpret_cast<vtkImplicitPlaneRepresentation*>(planeWidget->GetRepresentation());
    rep->GetPlane(this->plane);

    rep->DrawPlaneOn();

    qtPlaneWidget->updateSpinBoxes();
    qtPlaneWidget->modified();
  }

  vtkPlaneCallback()
  : plane(0)
  , qtPlaneWidget(0)
  {
  }
  vtkPlane* plane;
  VSPlaneWidget* qtPlaneWidget;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget::VSPlaneWidget(QWidget* parent, double bounds[6], vtkRenderWindowInteractor* iren)
: VSAbstractWidget(parent, bounds, iren)
{
  setupUi(this);

  double normal[3];
  normal[0] = 1.0;
  normal[1] = 0.0;
  normal[2] = 0.0;

  viewPlane = vtkSmartPointer<vtkPlane>::New();
  viewPlane->SetNormal(normal);
  viewPlane->SetOrigin(origin);

  usePlane = vtkSmartPointer<vtkPlane>::New();
  usePlane->SetNormal(normal);
  usePlane->SetOrigin(origin);

  vtkSmartPointer<vtkPlaneCallback> myCallback = vtkSmartPointer<vtkPlaneCallback>::New();
  myCallback->plane = viewPlane;
  myCallback->qtPlaneWidget = this;

  // Implicit Plane Widget
  planeRep = vtkImplicitPlaneRepresentation::New();
  planeRep->SetPlaceFactor(1.25);
  planeRep->PlaceWidget(bounds);
  planeRep->SetNormal(viewPlane->GetNormal());
  planeRep->SetOrigin(viewPlane->GetOrigin());
  planeRep->SetScaleEnabled(0);
  planeRep->SetOutlineTranslation(0);
  planeRep->DrawPlaneOff();
  planeRep->SetInteractionState(vtkImplicitPlaneRepresentation::Pushing);

  planeWidget = vtkImplicitPlaneWidget2::New();
  planeWidget->SetInteractor(iren);
  planeWidget->SetRepresentation(planeRep);
  planeWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

  updateSpinBoxes();

  // adjust the vtkWidget when values are changed
  connect(normalXSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(normalYSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(normalZSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));

  connect(originXSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(originYSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(originZSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget::~VSPlaneWidget()
{
  planeRep->Delete();
  planeWidget->Delete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::getNormals(double normals[3])
{ 
  normals[0] = normalXSpinBox->value();
  normals[1] = normalYSpinBox->value();
  normals[2] = normalZSpinBox->value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setNormals(double normals[3])
{
  viewPlane->SetNormal(normals);

  normalXSpinBox->setValue(normals[0]);
  normalYSpinBox->setValue(normals[1]);
  normalZSpinBox->setValue(normals[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setNormals(double x, double y, double z)
{
  double normals[3] = {x, y, z};
  setNormals(normals);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setOrigin(double origin[3])
{
  VSAbstractWidget::setOrigin(origin);

  viewPlane->SetOrigin(origin);

  originXSpinBox->setValue(origin[0]);
  originYSpinBox->setValue(origin[1]);
  originZSpinBox->setValue(origin[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setOrigin(double x, double y, double z)
{
  double origin[3] = {x, y, z};
  setOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updateBounds()
{
  planeWidget->EnabledOff();
  planeWidget->GetRepresentation()->PlaceWidget(bounds);
  planeWidget->EnabledOn();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updateOrigin()
{
  viewPlane->SetOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::enable()
{
  planeWidget->EnabledOn();

  if (m_renderWindowInteractor)
  {
    m_renderWindowInteractor->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::disable()
{
  planeWidget->EnabledOff();

  if (m_renderWindowInteractor)
  {
    m_renderWindowInteractor->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updateSpinBoxes()
{
  double normals[3];
  planeRep->GetNormal(normals);
  planeRep->GetOrigin(origin);

  setNormals(normals);

  setOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::spinBoxValueChanged()
{
  origin[0] = originXSpinBox->value();
  origin[1] = originYSpinBox->value();
  origin[2] = originZSpinBox->value();

  updatePlaneWidget();

  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updatePlaneWidget()
{
  planeWidget->Off();

  double normals[3];
  getNormals(normals);

  viewPlane->SetNormal(normals);
  viewPlane->SetOrigin(origin);

  planeRep->SetPlane(viewPlane);

  planeWidget->On();

  if (m_renderWindowInteractor)
  {
    m_renderWindowInteractor->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setInteractor(vtkRenderWindowInteractor* interactor)
{
  m_renderWindowInteractor = interactor;
  planeWidget->SetInteractor(interactor);
}
