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

#include <QtGui/QDoubleValidator>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

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
    disconnect(m_Transform, &VSTransform::updatedPosition, this, &VSTransformWidget::updateTranslationLabels);
    disconnect(m_Transform, &VSTransform::updatedRotation, this, &VSTransformWidget::updateRotationLabels);
    disconnect(m_Transform, &VSTransform::updatedScale, this, &VSTransformWidget::updateScaleLabels);
    disconnect(m_Transform, &VSTransform::updatedLocalPosition, this, &VSTransformWidget::updateLocalTranslation);
    disconnect(m_Transform, &VSTransform::updatedLocalRotation, this, &VSTransformWidget::updateLocalRotation);
    disconnect(m_Transform, &VSTransform::updatedLocalScale, this, &VSTransformWidget::updateLocalScale);
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

    connect(transform, &VSTransform::updatedPosition, this, &VSTransformWidget::updateTranslationLabels);
    connect(transform, &VSTransform::updatedRotation, this, &VSTransformWidget::updateRotationLabels);
    connect(transform, &VSTransform::updatedScale, this, &VSTransformWidget::updateScaleLabels);
    connect(transform, &VSTransform::updatedLocalPosition, this, &VSTransformWidget::updateLocalTranslation);
    connect(transform, &VSTransform::updatedLocalRotation, this, &VSTransformWidget::updateLocalRotation);
    connect(transform, &VSTransform::updatedLocalScale, this, &VSTransformWidget::updateLocalScale);

    // local
    double* localPos = transform->getLocalPosition();
    double* originPosition = transform->getOriginPosition();
    if(originPosition != nullptr)
    {
      for(int i = 0; i < 3; i++)
      {
        localPos[i] -= originPosition[i];
      }
    }
    m_Internals->posXEdit->setText(QString::number(localPos[0]));
    m_Internals->posYEdit->setText(QString::number(localPos[1]));
    m_Internals->posZEdit->setText(QString::number(localPos[2]));

    double* localRot = transform->getLocalRotation();
    double* originRotation = transform->getOriginRotation();
    if(originRotation != nullptr)
    {
      for(int i = 0; i < 3; i++)
      {
        localRot[i] -= originRotation[i];
      }
    }
    m_Internals->rotXEdit->setText(QString::number(localRot[0]));
    m_Internals->rotYEdit->setText(QString::number(localRot[1]));
    m_Internals->rotZEdit->setText(QString::number(localRot[2]));

    double* localScale = transform->getLocalScale();
    double* originScale = transform->getOriginScale();
    if(originScale != nullptr)
    {
      for(int i = 0; i < 3; i++)
      {
        localScale[i] /= originScale[i];
      }
    }
    m_Internals->scaleXEdit->setText(QString::number(localScale[0]));
    m_Internals->scaleYEdit->setText(QString::number(localScale[1]));
    m_Internals->scaleZEdit->setText(QString::number(localScale[2]));

    // global
    updateTranslationLabels();
    updateRotationLabels();
    updateScaleLabels();
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
    updateTranslationLabels();
    updateRotationLabels();
    updateScaleLabels();

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
void VSTransformWidget::setFilters(VSAbstractFilter::FilterListType filters)
{
  if(filters.size() == 0)
  {
    setTransform(nullptr);
  }
  else
  {
    m_SelectedFilters = filters;
    setTransform(filters.front()->getTransform());
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

  for(VSAbstractFilter* filter : m_SelectedFilters)
  {
    double* originPosition = filter->getTransform()->getOriginPosition();
    position[0] = m_Internals->posXEdit->text().toDouble() + originPosition[0];
    position[1] = m_Internals->posYEdit->text().toDouble() + originPosition[1];
    position[2] = m_Internals->posZEdit->text().toDouble() + originPosition[2];
    filter->getTransform()->setLocalPosition(position);
  }
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

  for(VSAbstractFilter* filter : m_SelectedFilters)
  {
    double* originRotation = filter->getTransform()->getOriginRotation();
    rotation[0] = m_Internals->rotXEdit->text().toDouble() + originRotation[0];
    rotation[1] = m_Internals->rotYEdit->text().toDouble() + originRotation[1];
    rotation[2] = m_Internals->rotZEdit->text().toDouble() + originRotation[2];
    filter->getTransform()->setLocalRotation(rotation);
  }
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

  for(VSAbstractFilter* filter : m_SelectedFilters)
  {
    double* originScale = filter->getTransform()->getOriginScale();
    scale[0] = m_Internals->scaleXEdit->text().toDouble() * originScale[0];
    scale[1] = m_Internals->scaleYEdit->text().toDouble() * originScale[1];
    scale[2] = m_Internals->scaleZEdit->text().toDouble() * originScale[2];
    filter->getTransform()->setLocalScale(scale);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateTranslationLabels()
{
  double* position;
  if(nullptr == m_Transform)
  {
    position = new double[3];
    for(int i = 0; i < 3; i++)
    {
      position[i] = 0.0;
    }
  }
  else
  {
    position = m_Transform->getPosition();
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
void VSTransformWidget::updateRotationLabels()
{
  double* rotation;
  if(nullptr == m_Transform)
  {
    rotation = new double[3];
    for(int i = 0; i < 3; i++)
    {
      rotation[i] = 0.0;
    }
  }
  else
  {
    rotation = m_Transform->getRotation();
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
void VSTransformWidget::updateScaleLabels()
{
  double* scale;
  if(nullptr == m_Transform)
  {
    scale = new double[3];
    for(int i = 0; i < 3; i++)
    {
      scale[i] = 1.0;
    }
  }
  else
  {
    scale = m_Transform->getScale();
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
void VSTransformWidget::updateLocalTranslation()
{
  double* position;
  double* origin;
  if(nullptr == m_Transform)
  {
    position = new double[3];
    for(int i = 0; i < 3; i++)
    {
      position[i] = 0.0;
    }
  }
  else
  {
    position = m_Transform->getLocalPosition();
    double* origin = m_Transform->getOriginPosition();
    for(int i = 0; i < 3; i++)
    {
      position[i] -= origin[i];
    }
  }

  m_Internals->posXEdit->setText(QString::number(position[0]));
  m_Internals->posYEdit->setText(QString::number(position[1]));
  m_Internals->posZEdit->setText(QString::number(position[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateLocalRotation()
{
  double* rotation;
  if(nullptr == m_Transform)
  {
    rotation = new double[3];
    for(int i = 0; i < 3; i++)
    {
      rotation[i] = 0.0;
    }
  }
  else
  {
    rotation = m_Transform->getLocalRotation();
  }

  m_Internals->rotXEdit->setText(QString::number(rotation[0]));
  m_Internals->rotYEdit->setText(QString::number(rotation[1]));
  m_Internals->rotZEdit->setText(QString::number(rotation[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransformWidget::updateLocalScale()
{
  double* scale;
  if(nullptr == m_Transform)
  {
    scale = new double[3];
    for(int i = 0; i < 3; i++)
    {
      scale[i] = 0.0;
    }
  }
  else
  {
    scale = m_Transform->getLocalScale();
  }

  m_Internals->scaleXEdit->setText(QString::number(scale[0]));
  m_Internals->scaleYEdit->setText(QString::number(scale[1]));
  m_Internals->scaleZEdit->setText(QString::number(scale[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSTransformWidget::getFilters()
{
  return m_SelectedFilters;
}
