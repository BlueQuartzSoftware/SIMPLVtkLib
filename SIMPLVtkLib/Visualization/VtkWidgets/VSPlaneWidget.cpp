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

#include <array>

#include <vtkCommand.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkPlane.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

#include <QDoubleSpinBox>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

using Array3Type = std::array<double, 3>;

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

  vtkPlaneCallback() = default;

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
  vtkPlane* m_UsePlane = nullptr;
  vtkPlane* m_ViewPlane = nullptr;
  VSPlaneWidget* m_VSPlaneWidget = nullptr;
  VSTransform* m_VSTransform = nullptr;
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

  Array3Type normal = {1.0, 0.0, 0.0};
  Array3Type viewNormal = {1.0, 0.0, 0.0};
  transform->globalizeNormal(viewNormal);
  Array3Type origin = calculateLocalOrigin(bounds, transform);
  Array3Type viewOrigin = calculateGlobalOrigin(bounds);

  m_UsePlane->SetOrigin(origin.data());
  m_UsePlane->SetNormal(normal.data());
  m_ViewPlane->SetOrigin(viewOrigin.data());
  m_ViewPlane->SetNormal(viewNormal.data());

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
double* VSPlaneWidget::getNormal() const
{
  return m_ViewPlane->GetNormal();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::getNormal(double normals[3]) const
{
  m_ViewPlane->GetNormal(normals);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setNormal(double normals[3])
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
void VSPlaneWidget::setNormal(double x, double y, double z)
{
  double normals[3] = {x, y, z};
  setNormal(normals);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSPlaneWidget::getOrigin() const
{
  return m_ViewPlane->GetOrigin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::getOrigin(double origin[3]) const
{
  m_ViewPlane->GetOrigin(origin);
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
  //  getVSTransform()->globalizePoint(origin);

  drawPlaneOn();
  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSPlaneWidget::getUsePlaneNormal()
{
  return m_UsePlane->GetNormal();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSPlaneWidget::getUsePlaneOrigin()
{
  return m_UsePlane->GetOrigin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setUsePlaneOrigin(double origin[3])
{
  m_UsePlane->SetOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setUsePlaneNormal(double normal[3])
{
  m_UsePlane->SetNormal(normal);
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
  setNormal(normal);

  updatePlaneWidget();
  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updatePlaneWidget()
{
  Array3Type normals;
  Array3Type origin;
  getNormal(normals.data());
  m_UsePlane->GetOrigin(origin.data());

  getVSTransform()->globalizeNormal(normals);
  getVSTransform()->globalizePoint(origin);

  int enabled = m_PlaneWidget->GetEnabled();
  m_PlaneWidget->Off();

  m_ViewPlane->SetNormal(normals.data());
  m_ViewPlane->SetOrigin(origin.data());

  m_PlaneRep->SetPlane(m_ViewPlane);
  m_PlaneWidget->SetEnabled(enabled);

  if(nullptr != getInteractor() && enabled)
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::readJson(QJsonObject& json)
{
  QJsonObject planeObj = json["VSPlane"].toObject();

  QJsonArray lastPlaneOrigin = planeObj["Use Origin"].toArray();
  QJsonArray lastPlaneNormal = planeObj["Use Normal"].toArray();
  QJsonArray planeOrigin = planeObj["View Origin"].toArray();
  QJsonArray planeNormal = planeObj["View Normal"].toArray();
  double lastOrigin[3];
  double lastNormal[3];
  double origin[3];
  double normal[3];
  for(int i = 0; i < 3; i++)
  {
    lastOrigin[i] = lastPlaneOrigin[i].toDouble();
    lastNormal[i] = lastPlaneNormal[i].toDouble();
    origin[i] = planeOrigin[i].toDouble();
    normal[i] = planeNormal[i].toDouble();
  }

  m_UsePlane->SetNormal(lastNormal);
  m_UsePlane->SetOrigin(lastOrigin);
  m_ViewPlane->SetNormal(normal);
  m_ViewPlane->SetOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::writeJson(const QJsonObject& json)
{
  QJsonObject planeObj;

  QJsonArray lastPlaneOrigin;
  QJsonArray lastPlaneNormal;
  QJsonArray planeOrigin;
  QJsonArray planeNormal;
  for(int i = 0; i < 3; i++)
  {
    lastPlaneOrigin.append(m_UsePlane->GetOrigin()[i]);
    lastPlaneNormal.append(m_UsePlane->GetNormal()[i]);
    planeOrigin.append(m_ViewPlane->GetOrigin()[i]);
    planeNormal.append(m_ViewPlane->GetNormal()[i]);
  }
  planeObj["Use Origin"] = lastPlaneOrigin;
  planeObj["Use Normal"] = lastPlaneNormal;
  planeObj["View Origin"] = planeOrigin;
  planeObj["View Normal"] = planeNormal;

  json["VSPlane"] = planeObj;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSPlaneWidget::equals(double* origin, double* normal) const
{
  double* currentOrigin = getOrigin();
  double* currentNormal = getNormal();

  for(int i = 0; i < 3; i++)
  {
    if(currentOrigin[i] != origin[i] || currentNormal[i] != normal[i])
    {
      return false;
    }
  }
  return true;
}
