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

#include "VSBoxWidget.h"

#include <cmath>

#include <vtkBox.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkPlanes.h>
#include <vtkPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

#include <vtkSmartPointer.h>

#include <QtWidgets/QDoubleSpinBox>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class vtkBoxCallback : public vtkCommand
{
public:
  static vtkBoxCallback* New()
  {
    return new vtkBoxCallback();
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkBoxWidget2* boxWidget = reinterpret_cast<vtkBoxWidget2*>(caller);
    vtkBoxRepresentation* rep = reinterpret_cast<vtkBoxRepresentation*>(boxWidget->GetRepresentation());
    rep->GetTransform(m_UseTransform);
    rep->GetTransform(m_ViewTransform);

    if(m_VSTransform)
    {
      m_VSTransform->localizeTransform(m_UseTransform);
    }

    m_VSBoxWidget->updateSpinBoxes();
    m_VSBoxWidget->modified();
  }

  vtkBoxCallback()
  : m_UseTransform(nullptr)
  , m_ViewTransform(nullptr)
  , m_VSTransform(nullptr)
  , m_VSBoxWidget(nullptr)
  {
  }

  void setUseTransform(vtkTransform* transform)
  {
    m_UseTransform = transform;
  }

  void setViewTransform(vtkTransform* transform)
  {
    m_ViewTransform = transform;
  }

  void setVSTransform(VSTransform* transform)
  {
    m_VSTransform = transform;
  }

  void setVSBoxWidget(VSBoxWidget* widget)
  {
    m_VSBoxWidget = widget;
  }

private:
  vtkTransform* m_UseTransform;
  vtkTransform* m_ViewTransform;
  VSTransform* m_VSTransform;
  VSBoxWidget* m_VSBoxWidget;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSBoxWidget::VSBoxWidget(QWidget* parent, VSTransform* transform, double bounds[6], vtkRenderWindowInteractor* iren)
: VSAbstractWidget(parent, transform, bounds, iren)
{
  setupUi(this);

  m_UseTransform = VTK_PTR(vtkTransform)::New();
  m_ViewTransform = VTK_PTR(vtkTransform)::New();
  getVSTransform()->globalizeTransform(m_ViewTransform);

  VTK_NEW(vtkBoxCallback, myCallback);
  myCallback->setUseTransform(m_UseTransform);
  myCallback->setViewTransform(m_ViewTransform);
  myCallback->setVSTransform(getVSTransform());
  myCallback->setVSBoxWidget(this);

  // Implicit Plane Widget
  m_BoxRep = VTK_PTR(vtkBoxRepresentation)::New();
  m_BoxRep->SetPlaceFactor(1.25);
  m_BoxRep->PlaceWidget(bounds);
  m_BoxRep->SetTransform(m_ViewTransform);

  m_BoxWidget = VTK_PTR(vtkBoxWidget2)::New();
  m_BoxWidget->SetInteractor(iren);
  m_BoxWidget->SetRepresentation(m_BoxRep);
  m_BoxWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

  updateSpinBoxes();

  // adjust the vtkWidget when values are changed
  connect(translationXSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(translationYSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(translationZSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));

  connect(rotationXSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(rotationYSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(rotationZSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));

  connect(scaleXSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(scaleYSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(scaleZSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSBoxWidget::~VSBoxWidget()
{
  m_BoxWidget->EnabledOff();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setScale(double scale[3])
{
  double position[3];
  double rotation[3];
  m_UseTransform->GetPosition(position);
  m_UseTransform->GetOrientation(rotation);

  scaleXSpinBox->setValue(scale[0]);
  scaleYSpinBox->setValue(scale[1]);
  scaleZSpinBox->setValue(scale[2]);

  setValues(position, rotation, scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setScale(double x, double y, double z)
{
  double scale[3] = {x, y, z};
  setScale(scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setTranslation(double origin[3])
{
  double rotation[3];
  double scale[3];
  m_UseTransform->GetOrientation(rotation);
  m_UseTransform->GetScale(scale);

  translationXSpinBox->setValue(origin[0]);
  translationYSpinBox->setValue(origin[1]);
  translationZSpinBox->setValue(origin[2]);

  setValues(origin, rotation, scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setRotation(double rotation[3])
{
  double position[3];
  double scale[3];
  m_UseTransform->GetPosition(position);
  m_UseTransform->GetScale(scale);

  rotationXSpinBox->setValue(rotation[0]);
  rotationYSpinBox->setValue(rotation[1]);
  rotationZSpinBox->setValue(rotation[2]);

  setValues(position, rotation, scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setRotation(double x, double y, double z)
{
  double rotation[3] = {x, y, z};

  setRotation(rotation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::enable()
{
  m_BoxWidget->EnabledOn();

  if(getInteractor())
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::disable()
{
  m_BoxWidget->EnabledOff();

  if(getInteractor())
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::updateSpinBoxes()
{
  double scale[3];
  double rotation[3];
  double origin[3];

  getTranslation(origin);
  getScale(scale);
  getRotation(rotation);

  translationXSpinBox->setValue(origin[0]);
  translationYSpinBox->setValue(origin[1]);
  translationZSpinBox->setValue(origin[2]);

  scaleXSpinBox->setValue(scale[0]);
  scaleYSpinBox->setValue(scale[1]);
  scaleZSpinBox->setValue(scale[2]);

  rotationXSpinBox->setValue(rotation[0]);
  rotationYSpinBox->setValue(rotation[1]);
  rotationZSpinBox->setValue(rotation[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::spinBoxValueChanged()
{
  double origin[3];
  origin[0] = translationXSpinBox->value();
  origin[1] = translationYSpinBox->value();
  origin[2] = translationZSpinBox->value();

  double scale[3];
  scale[0] = scaleXSpinBox->value();
  scale[1] = scaleYSpinBox->value();
  scale[2] = scaleZSpinBox->value();

  double rotation[3];
  rotation[0] = rotationXSpinBox->value();
  rotation[1] = rotationYSpinBox->value();
  rotation[2] = rotationZSpinBox->value();

  setValues(origin, rotation, scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::updateBoxWidget()
{
  m_UseTransform->SetInput(nullptr);
  m_ViewTransform->DeepCopy(m_UseTransform);
  getVSTransform()->globalizeTransform(m_ViewTransform);

  int enabled = m_BoxWidget->GetEnabled();
  m_BoxWidget->EnabledOff();
  m_BoxRep->SetTransform(m_ViewTransform);
  m_BoxWidget->SetEnabled(enabled);

  getInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::readJson(QJsonObject &json)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::writeJson(const QJsonObject &json)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::getTranslation(double translation[3])
{
  m_UseTransform->GetPosition(translation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::getScale(double scale[3])
{
  m_UseTransform->GetScale(scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::getRotation(double rotation[3])
{
  m_UseTransform->GetOrientation(rotation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransform) VSBoxWidget::getTransform()
{
  VTK_NEW(vtkTransform, transform);
  transform->DeepCopy(m_UseTransform);
  return transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setTransform(VTK_PTR(vtkTransform) transform)
{
  VTK_NEW(vtkMatrix4x4, matrix);
  matrix->DeepCopy(transform->GetMatrix());

  m_UseTransform->SetInput(nullptr);
  m_UseTransform->SetMatrix(matrix);
  
  updateBoxWidget();
  updateSpinBoxes();

  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkPlanes) VSBoxWidget::getPlanes()
{
  VTK_NEW(vtkPlanes, planes);
  m_BoxRep->GetPlanes(planes);

  getVSTransform()->localizePlanes(planes);
  return planes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setValues(double position[3], double rotation[3], double scale[3])
{
  VTK_NEW(vtkTransform, transform);
  
  transform->Translate(position);
  transform->RotateZ(rotation[2]);
  transform->RotateY(rotation[1]);
  transform->RotateX(rotation[0]);
  transform->Scale(scale);

  m_UseTransform->SetInput(nullptr);
  m_UseTransform->SetMatrix(transform->GetMatrix());

  updateBoxWidget();
  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setInteractor(vtkRenderWindowInteractor* interactor)
{
  VSAbstractWidget::setInteractor(interactor);
  m_BoxWidget->SetInteractor(interactor);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::updateGlobalSpace()
{
  // reposition the vtkWidget
  updateBoxWidget();
}
