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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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
    rep->GetPlane(this->m_UsePlane);
    rep->GetPlane(this->m_ViewPlane);

    if(m_VSTransform)
    {
      m_VSTransform->localizePlane(this->m_UsePlane);
    }

    rep->DrawPlaneOn();

    m_VSPlaneWidget->updateSpinBoxes();
    m_VSPlaneWidget->modified();
  }

  vtkPlaneCallback()
  : m_UsePlane(nullptr)
  , m_VSPlaneWidget(nullptr)
  , m_VSTransform(nullptr)
  {
  }

  void setUsePlane(vtkPlane* plane)
  {
    m_UsePlane = plane;
  }

  void setViewPlane(vtkPlane* plane)
  {
    m_ViewPlane = plane;
  }

  void setPlaneWidget(VSPlaneWidget* widget)
  {
    m_VSPlaneWidget = widget;
  }

  void setTransform(VSTransform* transform)
  {
    m_VSTransform = transform;
  }

private:
  vtkPlane* m_UsePlane;
  vtkPlane* m_ViewPlane;
  VSPlaneWidget* m_VSPlaneWidget;
  VSTransform* m_VSTransform;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget::VSPlaneWidget(QWidget* parent, VSTransform* transform, double bounds[6], vtkRenderWindowInteractor* iren)
: VSAbstractWidget(parent, transform, bounds, iren)
{
  setupUi(this);

  m_ViewPlane = vtkSmartPointer<vtkPlane>::New();
  m_UsePlane = vtkSmartPointer<vtkPlane>::New();

  double normal[3] = { 1.0, 0.0, 0.0 };
  double viewNormal[3] = { 1.0, 0.0, 0.0 };
  transform->globalizeNormal(viewNormal);
  double* origin = calculateLocalOrigin(bounds, transform);
  double* viewOrigin = calculateGlobalOrigin(bounds);
  
  m_UsePlane->SetOrigin(origin);
  m_UsePlane->SetNormal(normal);
  m_ViewPlane->SetOrigin(viewOrigin);
  m_ViewPlane->SetNormal(viewNormal);
  
  // Implicit Plane Widget
  m_PlaneRep = vtkImplicitPlaneRepresentation::New();
  m_PlaneRep->SetPlaceFactor(1.25);
  m_PlaneRep->PlaceWidget(bounds);
  m_PlaneRep->SetPlane(m_ViewPlane);
  m_PlaneRep->SetScaleEnabled(0);
  m_PlaneRep->SetOutlineTranslation(0);
  m_PlaneRep->DrawPlaneOff();
  m_PlaneRep->SetInteractionState(vtkImplicitPlaneRepresentation::Pushing);

  VTK_NEW(vtkPlaneCallback, myCallback);
  myCallback->setUsePlane(m_UsePlane);
  myCallback->setViewPlane(m_ViewPlane);
  myCallback->setPlaneWidget(this);
  myCallback->setTransform(transform);

  m_PlaneWidget = vtkImplicitPlaneWidget2::New();
  m_PlaneWidget->SetInteractor(iren);
  m_PlaneWidget->SetRepresentation(m_PlaneRep);
  m_PlaneWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

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
  m_PlaneRep->Delete();
  m_PlaneWidget->Delete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::getNormals(double normals[3])
{ 
  m_UsePlane->GetNormal(normals);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setNormals(double normals[3])
{
  m_UsePlane->SetNormal(normals);
  m_ViewPlane->SetNormal(normals);
  getVSTransform()->globalizePlane(m_ViewPlane);

  normalXSpinBox->setValue(normals[0]);
  normalYSpinBox->setValue(normals[1]);
  normalZSpinBox->setValue(normals[2]);

  drawPlaneOn();

  emit modified();
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
double* VSPlaneWidget::getOrigin()
{
  return m_UsePlane->GetOrigin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::getOrigin(double origin[3])
{
  m_UsePlane->GetOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setOrigin(double origin[3])
{
  m_UsePlane->SetOrigin(origin);
  
  originXSpinBox->setValue(origin[0]);
  originYSpinBox->setValue(origin[1]);
  originZSpinBox->setValue(origin[2]);

  m_ViewPlane->SetOrigin(origin);
  getVSTransform()->globalizePoint(origin);

  drawPlaneOn();

  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updateBounds()
{
  int enabled = m_PlaneWidget->GetEnabled();
  m_PlaneWidget->EnabledOff();
  m_PlaneWidget->GetRepresentation()->PlaceWidget(getBounds());
  m_PlaneWidget->SetEnabled(enabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::enable()
{
  m_PlaneWidget->EnabledOn();

  if(getInteractor())
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::disable()
{
  m_PlaneWidget->EnabledOff();

  if(getInteractor())
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::drawPlaneOn()
{
  m_PlaneRep->DrawPlaneOn();

  if(getInteractor())
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::drawPlaneOff()
{
  m_PlaneRep->DrawPlaneOff();
  
  if(getInteractor())
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updateSpinBoxes()
{
  double normals[3];
  double origin[3];
  m_UsePlane->GetNormal(normals);
  m_UsePlane->GetOrigin(origin);

  normalXSpinBox->setValue(normals[0]);
  normalYSpinBox->setValue(normals[1]);
  normalZSpinBox->setValue(normals[2]);

  originXSpinBox->setValue(origin[0]);
  originYSpinBox->setValue(origin[1]);
  originZSpinBox->setValue(origin[2]);

  setOrigin(origin);

  drawPlaneOn();
  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::spinBoxValueChanged()
{
  double origin[3];
  origin[0] = originXSpinBox->value();
  origin[1] = originYSpinBox->value();
  origin[2] = originZSpinBox->value();
  setOrigin(origin);

  double normal[3];
  normal[0] = normalXSpinBox->value();
  normal[1] = normalYSpinBox->value();
  normal[2] = normalZSpinBox->value();
  setNormals(normal);

  updatePlaneWidget();
  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updatePlaneWidget()
{
  double normals[3];
  double origin[3];
  getNormals(normals);
  m_UsePlane->GetOrigin(origin);

  getVSTransform()->globalizeNormal(normals);
  getVSTransform()->globalizePoint(origin);

  int enabled = m_PlaneWidget->GetEnabled();
  m_PlaneWidget->Off();

  m_ViewPlane->SetNormal(normals);
  m_ViewPlane->SetOrigin(origin);

  m_PlaneRep->SetPlane(m_ViewPlane);
  m_PlaneWidget->SetEnabled(enabled);

  if(getInteractor() && enabled)
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::readJson(QJsonObject &json)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::writeJson(const QJsonObject &json)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setInteractor(vtkRenderWindowInteractor* interactor)
{
  VSAbstractWidget::setInteractor(interactor);
  m_PlaneWidget->SetInteractor(interactor);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updateGlobalSpace()
{
  // reposition the vtkWidget
  updatePlaneWidget();
}
