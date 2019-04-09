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

#include "ImportRobometMontageDialog.h"

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
QString ImportRobometMontageDialog::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportRobometMontageDialog::ImportRobometMontageDialog(QWidget* parent)
: AbstractMontageDialog(parent)
, m_Ui(new Ui::ImportRobometMontageDialog)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportRobometMontageDialog::~ImportRobometMontageDialog()
{
  disconnectSignalsSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportRobometMontageDialog::Pointer ImportRobometMontageDialog::New(QWidget* parent)
{
  Pointer sharedPtr(new ImportRobometMontageDialog(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::setupGui()
{
  m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Import");

  qRegisterMetaType<RobometListInfo_t>();

  connectSignalsSlots();

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::connectSignalsSlots()
{
  connect(m_Ui->dataDisplayTypeCB, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index) { setDisplayType(static_cast<AbstractMontageDialog::DisplayType>(index)); });

  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { checkComplete(); });

  connect(m_Ui->robometListWidget, &RobometListWidget::inputDirectoryChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::filePrefixChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::fileSuffixChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::fileExtensionChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::sliceMinChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::sliceMaxChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::numberOfRowsChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::numberOfColumnsChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::slicePaddingChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::rowColPaddingChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);

  connect(m_Ui->changeTileOverlapCB, &QCheckBox::stateChanged, this, &ImportRobometMontageDialog::changeTileOverlap_stateChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::disconnectSignalsSlots()
{
  disconnect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), 0, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::robometListWidgetChanged()
{
  RobometListInfo_t robometListInfo = m_Ui->robometListWidget->getRobometListInfo();
  setRobometListInfo(robometListInfo);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::changeTileOverlap_stateChanged(int state)
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
void ImportRobometMontageDialog::checkComplete() const
{
  bool result = true;

  if(m_Ui->montageNameLE->isEnabled())
  {
    if(m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->robometListWidget->isEnabled())
  {
    if(!m_Ui->robometListWidget->isComplete())
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
QString ImportRobometMontageDialog::getMontageName()
{
  return m_Ui->montageNameLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportRobometMontageDialog::getOverrideTileOverlap()
{
  return m_Ui->changeTileOverlapCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportRobometMontageDialog::getTileOverlap()
{
  return m_Ui->tileOverlapSB->value();
}
