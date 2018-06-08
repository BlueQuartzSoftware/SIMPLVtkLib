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
  m_Internals->posXEdit->setValidator(new QDoubleValidator());
  m_Internals->posYEdit->setValidator(new QDoubleValidator());
  m_Internals->posZEdit->setValidator(new QDoubleValidator());

  m_Internals->rotXEdit->setValidator(new QDoubleValidator());
  m_Internals->rotYEdit->setValidator(new QDoubleValidator());
  m_Internals->rotZEdit->setValidator(new QDoubleValidator());

  m_Internals->scaleXEdit->setValidator(new QDoubleValidator());
  m_Internals->scaleYEdit->setValidator(new QDoubleValidator());
  m_Internals->scaleZEdit->setValidator(new QDoubleValidator());

  connect(m_Internals->posXEdit, SIGNAL(editingFinished()), this, SLOT(translationEditChanged()));
  connect(m_Internals->posYEdit, SIGNAL(editingFinished()), this, SLOT(translationEditChanged()));
  connect(m_Internals->posZEdit, SIGNAL(editingFinished()), this, SLOT(translationEditChanged()));

  connect(m_Internals->rotXEdit, SIGNAL(editingFinished()), this, SLOT(rotationEditChanged()));
  connect(m_Internals->rotYEdit, SIGNAL(editingFinished()), this, SLOT(rotationEditChanged()));
  connect(m_Internals->rotZEdit, SIGNAL(editingFinished()), this, SLOT(rotationEditChanged()));

  connect(m_Internals->scaleXEdit, SIGNAL(editingFinished()), this, SLOT(scaleEditChanged()));
  connect(m_Internals->scaleYEdit, SIGNAL(editingFinished()), this, SLOT(scaleEditChanged()));
  connect(m_Internals->scaleZEdit, SIGNAL(editingFinished()), this, SLOT(scaleEditChanged()));
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
    disconnect(m_Transform, SIGNAL(updatedPosition(double*)), this, SLOT(updateTranslationLabels(double*)));
    disconnect(m_Transform, SIGNAL(updatedRotation(double*)), this, SLOT(updateRotationLabels(double*)));
    disconnect(m_Transform, SIGNAL(updatedScale(double*)), this, SLOT(updateScaleLabels(double*)));
    disconnect(m_Transform, SIGNAL(updatedLocalPosition(double*)), this, SLOT(updateLocalTranslation(double*)));
    disconnect(m_Transform, SIGNAL(updatedLocalRotation(double*)), this, SLOT(updateLocalRotation(double*)));
    disconnect(m_Transform, SIGNAL(updatedLocalScale(double*)), this, SLOT(updateLocalScale(double*)));
  }

  m_Transform = transform;

  bool transformExists = (nullptr != transform);
  if(transformExists)
  {
    // Enable Widgets
    m_Internals->posXEdit->setEnabled(true);
    m_Internals->posYEdit->setEnabled(true);
    m_Internals->posZEdit->setEnabled(true);
    m_Internals->rotXEdit->setEnabled(true);
    m_Internals->rotYEdit->setEnabled(true);
    m_Internals->rotZEdit->setEnabled(true);
    m_Internals->scaleXEdit->setEnabled(true);
    m_Internals->scaleYEdit->setEnabled(true);
    m_Internals->scaleZEdit->setEnabled(true);

    m_Internals->posXLabel->setEnabled(true);
    m_Internals->posYLabel->setEnabled(true);
    m_Internals->posZLabel->setEnabled(true);
    m_Internals->rotXLabel->setEnabled(true);
    m_Internals->rotYLabel->setEnabled(true);
    m_Internals->rotZLabel->setEnabled(true);
    m_Internals->scaleXLabel->setEnabled(true);
    m_Internals->scaleYLabel->setEnabled(true);
    m_Internals->scaleZLabel->setEnabled(true);

    connect(transform, SIGNAL(updatedPosition(double*)), this, SLOT(updateTranslationLabels(double*)));
    connect(transform, SIGNAL(updatedRotation(double*)), this, SLOT(updateRotationLabels(double*)));
    connect(transform, SIGNAL(updatedScale(double*)), this, SLOT(updateScaleLabels(double*)));
    connect(transform, SIGNAL(updatedLocalPosition(double*)), this, SLOT(updateLocalTranslation(double*)));
    connect(transform, SIGNAL(updatedLocalRotation(double*)), this, SLOT(updateLocalRotation(double*)));
    connect(transform, SIGNAL(updatedLocalScale(double*)), this, SLOT(updateLocalScale(double*)));

    // local
    double* localPos = transform->getLocalPosition();
    m_Internals->posXEdit->setText(QString::number(localPos[0]));
    m_Internals->posYEdit->setText(QString::number(localPos[1]));
    m_Internals->posZEdit->setText(QString::number(localPos[2]));

    double* localRot = transform->getLocalRotation();
    m_Internals->rotXEdit->setText(QString::number(localRot[0]));
    m_Internals->rotYEdit->setText(QString::number(localRot[1]));
    m_Internals->rotZEdit->setText(QString::number(localRot[2]));

    double* localScale = transform->getLocalScale();
    m_Internals->scaleXEdit->setText(QString::number(localScale[0]));
    m_Internals->scaleYEdit->setText(QString::number(localScale[1]));
    m_Internals->scaleZEdit->setText(QString::number(localScale[2]));

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
    m_Internals->posXEdit->setText(QString::number(0.0));
    m_Internals->posYEdit->setText(QString::number(0.0));
    m_Internals->posZEdit->setText(QString::number(0.0));

    m_Internals->rotXEdit->setText(QString::number(0.0));
    m_Internals->rotYEdit->setText(QString::number(0.0));
    m_Internals->rotZEdit->setText(QString::number(0.0));

    m_Internals->scaleXEdit->setText(QString::number(0.0));
    m_Internals->scaleYEdit->setText(QString::number(0.0));
    m_Internals->scaleZEdit->setText(QString::number(0.0));

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
    m_Internals->posXEdit->setEnabled(false);
    m_Internals->posYEdit->setEnabled(false);
    m_Internals->posZEdit->setEnabled(false);
    m_Internals->rotXEdit->setEnabled(false);
    m_Internals->rotYEdit->setEnabled(false);
    m_Internals->rotZEdit->setEnabled(false);
    m_Internals->scaleXEdit->setEnabled(false);
    m_Internals->scaleYEdit->setEnabled(false);
    m_Internals->scaleZEdit->setEnabled(false);

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
void VSTransformWidget::translationEditChanged()
{
  if(nullptr == m_Transform)
  {
    return;
  }

  double position[3];
  position[0] = m_Internals->posXEdit->text().toDouble();
  position[1] = m_Internals->posYEdit->text().toDouble();
  position[2] = m_Internals->posZEdit->text().toDouble();

  m_Transform->setLocalPosition(position);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::rotationEditChanged()
{
  if(nullptr == m_Transform)
  {
    return;
  }

  double rotation[3];
  rotation[0] = m_Internals->rotXEdit->text().toDouble();
  rotation[1] = m_Internals->rotYEdit->text().toDouble();
  rotation[2] = m_Internals->rotZEdit->text().toDouble();

  m_Transform->setLocalRotation(rotation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::scaleEditChanged()
{
  if(nullptr == m_Transform)
  {
    return;
  }

  double scale[3];
  scale[0] = m_Internals->scaleXEdit->text().toDouble();
  scale[1] = m_Internals->scaleYEdit->text().toDouble();
  scale[2] = m_Internals->scaleZEdit->text().toDouble();

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateLocalTranslation(double* position)
{
  m_Internals->posXEdit->setText(QString::number(position[0]));
  m_Internals->posYEdit->setText(QString::number(position[1]));
  m_Internals->posZEdit->setText(QString::number(position[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateLocalRotation(double* rotation)
{
  m_Internals->rotXEdit->setText(QString::number(rotation[0]));
  m_Internals->rotYEdit->setText(QString::number(rotation[1]));
  m_Internals->rotZEdit->setText(QString::number(rotation[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateLocalScale(double* scale)
{
  m_Internals->scaleXEdit->setText(QString::number(scale[0]));
  m_Internals->scaleYEdit->setText(QString::number(scale[1]));
  m_Internals->scaleZEdit->setText(QString::number(scale[2]));
}
