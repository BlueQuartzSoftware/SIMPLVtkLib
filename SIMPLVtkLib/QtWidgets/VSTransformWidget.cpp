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

#include "VSTransformWidget.h"

#include "ui_VSTransformWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VSTransformWidget::VSInternals : public Ui::VSTransformWidget
{
public:
  VSInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransformWidget::VSTransformWidget(QWidget* parent)
  : QWidget(parent)
  , m_Internals(new VSInternals())
{
  m_Internals->setupUi(this);
  setupGui();
  setTransform(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::setupGui()
{
  connect(m_Internals->posXSpinBox, SIGNAL(editingFinished()), this, SLOT(translationSpinBoxesChanged()));
  connect(m_Internals->posYSpinBox, SIGNAL(editingFinished()), this, SLOT(translationSpinBoxesChanged()));
  connect(m_Internals->posZSpinBox, SIGNAL(editingFinished()), this, SLOT(translationSpinBoxesChanged()));

  connect(m_Internals->rotXSpinBox, SIGNAL(editingFinished()), this, SLOT(rotationSpinBoxesChanged()));
  connect(m_Internals->rotYSpinBox, SIGNAL(editingFinished()), this, SLOT(rotationSpinBoxesChanged()));
  connect(m_Internals->rotZSpinBox, SIGNAL(editingFinished()), this, SLOT(rotationSpinBoxesChanged()));

  connect(m_Internals->scaleXSpinBox, SIGNAL(editingFinished()), this, SLOT(scaleSpinBoxesChanged()));
  connect(m_Internals->scaleYSpinBox, SIGNAL(editingFinished()), this, SLOT(scaleSpinBoxesChanged()));
  connect(m_Internals->scaleZSpinBox, SIGNAL(editingFinished()), this, SLOT(scaleSpinBoxesChanged()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform* VSTransformWidget::getTransform()
{
  return m_Transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::setTransform(VSTransform* transform)
{
  if(m_Transform)
  {
    disconnect(transform, SIGNAL(updatedPosition(double*)),
      this, SLOT(updateTranslationLabels(double*)));
    disconnect(transform, SIGNAL(updatedRotation(double*)),
      this, SLOT(updateRotationLabels(double*)));
    disconnect(transform, SIGNAL(updatedScale(double*)),
      this, SLOT(updateScaleLabels(double*)));
  }

  m_Transform = transform;

  bool transformExists = (nullptr != transform);
  if(transformExists)
  {
    // Enable Widgets
    m_Internals->posXSpinBox->setEnabled(true);
    m_Internals->posYSpinBox->setEnabled(true);
    m_Internals->posZSpinBox->setEnabled(true);
    m_Internals->rotXSpinBox->setEnabled(true);
    m_Internals->rotYSpinBox->setEnabled(true);
    m_Internals->rotZSpinBox->setEnabled(true);
    m_Internals->scaleXSpinBox->setEnabled(true);
    m_Internals->scaleYSpinBox->setEnabled(true);
    m_Internals->scaleZSpinBox->setEnabled(true);

    m_Internals->posXLabel->setEnabled(true);
    m_Internals->posYLabel->setEnabled(true);
    m_Internals->posZLabel->setEnabled(true);
    m_Internals->rotXLabel->setEnabled(true);
    m_Internals->rotYLabel->setEnabled(true);
    m_Internals->rotZLabel->setEnabled(true);
    m_Internals->scaleXLabel->setEnabled(true);
    m_Internals->scaleYLabel->setEnabled(true);
    m_Internals->scaleZLabel->setEnabled(true);

    connect(transform, SIGNAL(updatedPosition(double*)),
      this, SLOT(updateTranslationLabels(double*)));
    connect(transform, SIGNAL(updatedRotation(double*)),
      this, SLOT(updateRotationLabels(double*)));
    connect(transform, SIGNAL(updatedScale(double*)),
      this, SLOT(updateScaleLabels(double*)));

    // local
    double* localPos = transform->getLocalPosition();
    m_Internals->posXSpinBox->setValue(localPos[0]);
    m_Internals->posYSpinBox->setValue(localPos[1]);
    m_Internals->posZSpinBox->setValue(localPos[2]);

    double* localRot = transform->getLocalRotation();
    m_Internals->rotXSpinBox->setValue(localRot[0]);
    m_Internals->rotYSpinBox->setValue(localRot[1]);
    m_Internals->rotZSpinBox->setValue(localRot[2]);

    double* localScale = transform->getLocalScale();
    m_Internals->scaleXSpinBox->setValue(localScale[0]);
    m_Internals->scaleYSpinBox->setValue(localScale[1]);
    m_Internals->scaleZSpinBox->setValue(localScale[2]);

    // global
    double* globalPos = transform->getPosition();
    updateTranslationLabels(globalPos);

    double* globalRot = transform->getRotation();
    updateRotationLabels(globalRot);

    double* globalScale = transform->getScale();
    updateScaleLabels(globalScale);
  }
  else
  {
    // local
    m_Internals->posXSpinBox->setValue(0.0);
    m_Internals->posYSpinBox->setValue(0.0);
    m_Internals->posZSpinBox->setValue(0.0);

    m_Internals->rotXSpinBox->setValue(0.0);
    m_Internals->rotYSpinBox->setValue(0.0);
    m_Internals->rotZSpinBox->setValue(0.0);

    m_Internals->scaleXSpinBox->setValue(0.0);
    m_Internals->scaleYSpinBox->setValue(0.0);
    m_Internals->scaleZSpinBox->setValue(0.0);

    // global
    double* globalPos = new double[3];
    double* globalRot = new double[3];
    double* globalScale = new double[3];
    
    for(int i = 0; i < 3; i++)
    {
      globalPos[i] = 0.0;
      globalRot[i] = 0.0;
      globalScale[i] = 1.0;
    }

    updateTranslationLabels(globalPos);
    updateRotationLabels(globalRot);
    updateScaleLabels(globalScale);

    delete[] globalPos;
    delete[] globalRot;
    delete[] globalScale;

    // Disable widgets
    m_Internals->posXSpinBox->setEnabled(false);
    m_Internals->posYSpinBox->setEnabled(false);
    m_Internals->posZSpinBox->setEnabled(false);
    m_Internals->rotXSpinBox->setEnabled(false);
    m_Internals->rotYSpinBox->setEnabled(false);
    m_Internals->rotZSpinBox->setEnabled(false);
    m_Internals->scaleXSpinBox->setEnabled(false);
    m_Internals->scaleYSpinBox->setEnabled(false);
    m_Internals->scaleZSpinBox->setEnabled(false);

    m_Internals->posXLabel->setEnabled(false);
    m_Internals->posYLabel->setEnabled(false);
    m_Internals->posZLabel->setEnabled(false);
    m_Internals->rotXLabel->setEnabled(false);
    m_Internals->rotYLabel->setEnabled(false);
    m_Internals->rotZLabel->setEnabled(false);
    m_Internals->scaleXLabel->setEnabled(false);
    m_Internals->scaleYLabel->setEnabled(false);
    m_Internals->scaleZLabel->setEnabled(false);
  }
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::translationSpinBoxesChanged()
{
  if(nullptr == m_Transform)
  {
    return;
  }

  double position[3];
  position[0] = m_Internals->posXSpinBox->value();
  position[1] = m_Internals->posYSpinBox->value();
  position[2] = m_Internals->posZSpinBox->value();

  m_Transform->setLocalPosition(position);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::rotationSpinBoxesChanged()
{
  if(nullptr == m_Transform)
  {
    return;
  }

  double rotation[3];
  rotation[0] = m_Internals->rotXSpinBox->value();
  rotation[1] = m_Internals->rotYSpinBox->value();
  rotation[2] = m_Internals->rotZSpinBox->value();

  m_Transform->setLocalRotation(rotation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::scaleSpinBoxesChanged()
{
  if(nullptr == m_Transform)
  {
    return;
  }

  double scale[3];
  scale[0] = m_Internals->scaleXSpinBox->value();
  scale[1] = m_Internals->scaleYSpinBox->value();
  scale[2] = m_Internals->scaleZSpinBox->value();

  m_Transform->setLocalScale(scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateTranslationLabels(double* position)
{
  if(nullptr == position)
  {
    return;
  }

  QLocale locale = QLocale::system();

  QString value1 = locale.toString(position[0]);
  QString value2 = locale.toString(position[1]);
  QString value3 = locale.toString(position[2]);

  m_Internals->posXLabel->setText(value1);
  m_Internals->posYLabel->setText(value2);
  m_Internals->posZLabel->setText(value3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateRotationLabels(double* rotation)
{
  if(nullptr == rotation)
  {
    return;
  }

  QLocale locale = QLocale::system();

  QString value1 = locale.toString(rotation[0]);
  QString value2 = locale.toString(rotation[1]);
  QString value3 = locale.toString(rotation[2]);

  m_Internals->rotXLabel->setText(value1);
  m_Internals->rotYLabel->setText(value2);
  m_Internals->rotZLabel->setText(value3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateScaleLabels(double* scale)
{
  if(nullptr == scale)
  {
    return;
  }

  QLocale locale = QLocale::system();

  QString value1 = locale.toString(scale[0]);
  QString value2 = locale.toString(scale[1]);
  QString value3 = locale.toString(scale[2]);

  m_Internals->scaleXLabel->setText(value1);
  m_Internals->scaleYLabel->setText(value2);
  m_Internals->scaleZLabel->setText(value3);
}
