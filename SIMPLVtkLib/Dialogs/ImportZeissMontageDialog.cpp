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

#include "ImportZeissMontageDialog.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

// Initialize private static member variable
QString ImportZeissMontageDialog::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportZeissMontageDialog::ImportZeissMontageDialog(QWidget* parent)
: AbstractImportMontageDialog(parent)
, m_Ui(new Ui::ImportZeissMontageDialog)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportZeissMontageDialog::~ImportZeissMontageDialog() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportZeissMontageDialog::Pointer ImportZeissMontageDialog::New(QWidget* parent)
{
  Pointer sharedPtr(new ImportZeissMontageDialog(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissMontageDialog::setupGui()
{
  m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Import");

  qRegisterMetaType<ZeissListInfo_t>();

  connectSignalsSlots();

  m_Ui->colorWeightingR->setValidator(new QDoubleValidator);
  m_Ui->colorWeightingG->setValidator(new QDoubleValidator);
  m_Ui->colorWeightingB->setValidator(new QDoubleValidator);
  m_Ui->originX->setValidator(new QDoubleValidator);
  m_Ui->originY->setValidator(new QDoubleValidator);
  m_Ui->originZ->setValidator(new QDoubleValidator);
  m_Ui->spacingX->setValidator(new QDoubleValidator);
  m_Ui->spacingY->setValidator(new QDoubleValidator);
  m_Ui->spacingZ->setValidator(new QDoubleValidator);

  m_Ui->montageNameLE->setText("Untitled Montage");

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissMontageDialog::connectSignalsSlots()
{
  connect(m_Ui->dataDisplayTypeCB, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index) { setDisplayType(static_cast<AbstractImportMontageDialog::DisplayType>(index)); });

  connect(m_Ui->zeissListWidget, &ZeissListWidget::inputDirectoryChanged, this, &ImportZeissMontageDialog::zeissListWidgetChanged);
  connect(m_Ui->zeissListWidget, &ZeissListWidget::numberOfRowsChanged, this, &ImportZeissMontageDialog::zeissListWidgetChanged);
  connect(m_Ui->zeissListWidget, &ZeissListWidget::numberOfColumnsChanged, this, &ImportZeissMontageDialog::zeissListWidgetChanged);

  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { checkComplete(); });

  connect(m_Ui->colorWeightingR, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->colorWeightingG, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->colorWeightingB, &QLineEdit::textChanged, [=] { checkComplete(); });

  connect(m_Ui->originX, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->originY, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->originZ, &QLineEdit::textChanged, [=] { checkComplete(); });

  connect(m_Ui->spacingX, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->spacingY, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->spacingZ, &QLineEdit::textChanged, [=] { checkComplete(); });

  // Connect the checkboxes for grayscale, origin, and spacing to respective line edit group boxes
  connect(m_Ui->convertGrayscaleCB, &QCheckBox::stateChanged, [=] {
    bool isChecked = m_Ui->convertGrayscaleCB->isChecked();
    m_Ui->colorWeightingR->setEnabled(isChecked);
    m_Ui->colorWeightingG->setEnabled(isChecked);
    m_Ui->colorWeightingB->setEnabled(isChecked);
  });
  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, [=] {
    bool isChecked = m_Ui->changeOriginCB->isChecked();
    m_Ui->originX->setEnabled(isChecked);
    m_Ui->originY->setEnabled(isChecked);
    m_Ui->originZ->setEnabled(isChecked);
  });
  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, [=] {
    bool isChecked = m_Ui->changeSpacingCB->isChecked();
    m_Ui->spacingX->setEnabled(isChecked);
    m_Ui->spacingY->setEnabled(isChecked);
    m_Ui->spacingZ->setEnabled(isChecked);
  });

  connect(m_Ui->changeTileOverlapCB, &QCheckBox::stateChanged, this, &ImportZeissMontageDialog::changeTileOverlap_stateChanged);
  connect(m_Ui->convertGrayscaleCB, &QCheckBox::stateChanged, this, &ImportZeissMontageDialog::convertGrayscale_stateChanged);
  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, this, &ImportZeissMontageDialog::changeOrigin_stateChanged);
  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, this, &ImportZeissMontageDialog::changeSpacing_stateChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissMontageDialog::zeissListWidgetChanged()
{
  ZeissListInfo_t zeissListInfo = m_Ui->zeissListWidget->getZeissListInfo();
  setZeissListInfo(zeissListInfo);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissMontageDialog::changeTileOverlap_stateChanged(int state)
{
  m_Ui->tileOverlapSB->setEnabled(state);
  if(state == false)
  {
    m_Ui->tileOverlapSB->setValue(15);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissMontageDialog::convertGrayscale_stateChanged(int state)
{
  m_Ui->colorWeightingR->setEnabled(state);
  m_Ui->colorWeightingG->setEnabled(state);
  m_Ui->colorWeightingB->setEnabled(state);
  if(state == false)
  {
    m_Ui->colorWeightingR->setText("0.2125");
    m_Ui->colorWeightingG->setText("0.7154");
    m_Ui->colorWeightingB->setText("0.0721");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissMontageDialog::changeOrigin_stateChanged(int state)
{
  m_Ui->originX->setEnabled(state);
  m_Ui->originY->setEnabled(state);
  m_Ui->originZ->setEnabled(state);
  if(state == false)
  {
    m_Ui->originX->setText("0");
    m_Ui->originY->setText("0");
    m_Ui->originZ->setText("0");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissMontageDialog::changeSpacing_stateChanged(int state)
{
  m_Ui->spacingX->setEnabled(state);
  m_Ui->spacingY->setEnabled(state);
  m_Ui->spacingZ->setEnabled(state);
  if(state == false)
  {
    m_Ui->spacingX->setText("1");
    m_Ui->spacingY->setText("1");
    m_Ui->spacingZ->setText("1");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissMontageDialog::checkComplete() const
{
  bool result = true;

  if(m_Ui->montageNameLE->isEnabled())
  {
    if(m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->zeissListWidget->isEnabled())
  {
    if(!m_Ui->zeissListWidget->isComplete())
    {
      result = false;
    }
  }

  if(m_Ui->tileOverlapSB->isEnabled())
  {
    if(m_Ui->tileOverlapSB->value() < m_Ui->tileOverlapSB->minimum() || m_Ui->tileOverlapSB->value() > m_Ui->tileOverlapSB->maximum())
    {
      result = false;
    }
  }

  if(m_Ui->colorWeightingR->isEnabled())
  {
    if(m_Ui->colorWeightingR->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->colorWeightingG->isEnabled())
  {
    if(m_Ui->colorWeightingG->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->colorWeightingB->isEnabled())
  {
    if(m_Ui->colorWeightingB->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->originX->isEnabled())
  {
    if(m_Ui->originX->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->originY->isEnabled())
  {
    if(m_Ui->originY->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->originZ->isEnabled())
  {
    if(m_Ui->originZ->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->spacingX->isEnabled())
  {
    if(m_Ui->spacingX->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->spacingY->isEnabled())
  {
    if(m_Ui->spacingY->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->spacingZ->isEnabled())
  {
    if(m_Ui->spacingZ->text().isEmpty())
    {
      result = false;
    }
  }

  QPushButton* okBtn = m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
  if(!okBtn)
  {
    throw InvalidOKButtonException();
  }

  okBtn->setEnabled(result);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportZeissMontageDialog::getMontageName()
{
  return m_Ui->montageNameLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportZeissMontageDialog::getOverrideTileOverlap()
{
  return m_Ui->changeTileOverlapCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportZeissMontageDialog::getTileOverlap()
{
  return m_Ui->tileOverlapSB->value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportZeissMontageDialog::getOverrideSpacing()
{
  return m_Ui->changeSpacingCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::tuple<double, double, double> ImportZeissMontageDialog::getSpacing()
{
  double spacingX = m_Ui->spacingX->text().toDouble();
  double spacingY = m_Ui->spacingY->text().toDouble();
  double spacingZ = m_Ui->spacingZ->text().toDouble();
  std::tuple<double, double, double> spacing = std::make_tuple(spacingX, spacingY, spacingZ);
  return spacing;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportZeissMontageDialog::getOverrideOrigin()
{
  return m_Ui->changeOriginCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::tuple<double, double, double> ImportZeissMontageDialog::getOrigin()
{
  double originX = m_Ui->originX->text().toDouble();
  double originY = m_Ui->originY->text().toDouble();
  double originZ = m_Ui->originZ->text().toDouble();
  std::tuple<double, double, double> origin = std::make_tuple(originX, originY, originZ);
  return origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportZeissMontageDialog::getConvertToGrayscale()
{
  return m_Ui->convertGrayscaleCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::tuple<double, double, double> ImportZeissMontageDialog::getColorWeighting()
{
  double r = m_Ui->colorWeightingR->text().toDouble();
  double g = m_Ui->colorWeightingG->text().toDouble();
  double b = m_Ui->colorWeightingB->text().toDouble();
  std::tuple<double, double, double> colorWeighting = std::make_tuple(r, g, b);
  return colorWeighting;
}
