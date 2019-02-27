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

#include "EnterDREAM3DDataPage.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"

#include "ImportMontage/ImportMontageConstants.h"

#include "ImportMontageWizard.h"

// Initialize private static member variable
QString EnterDREAM3DDataPage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterDREAM3DDataPage::EnterDREAM3DDataPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::EnterDREAM3DDataPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterDREAM3DDataPage::~EnterDREAM3DDataPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::setupGui()
{
	connectSignalsSlots();

	m_Ui->numOfRowsSB->setMinimum(1);
	m_Ui->numOfRowsSB->setMaximum(std::numeric_limits<int>().max());

	m_Ui->numOfColsSB->setMinimum(1);
	m_Ui->numOfColsSB->setMaximum(std::numeric_limits<int>().max());

	m_Ui->tileOverlapSB->setMinimum(0);
	m_Ui->tileOverlapSB->setMaximum(100);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::connectSignalsSlots()
{
  connect(m_Ui->selectButton, &QPushButton::clicked, this, &EnterDREAM3DDataPage::selectBtn_clicked);

	QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &EnterDREAM3DDataPage::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &EnterDREAM3DDataPage::dataFile_textChanged);

  connect(m_Ui->montageNameLE, &QLineEdit::textChanged, this, [=] { emit completeChanged(); });

  connect(m_Ui->imageDataContainerPrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->cellAttrMatrixNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageArrayNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->numOfRowsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->numOfColsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterDREAM3DDataPage::isComplete() const
{
  bool result = true;

  if (m_Ui->montageNameLE->isEnabled())
  {
    if (m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->dataFileLE->isEnabled())
  {
    if (m_Ui->dataFileLE->text().isEmpty())
    {
      result = false;
    }
  }

  QFileInfo fi(m_Ui->dataFileLE->text());
  if (fi.completeSuffix() != "dream3d")
  {
    result = false;
  }

  if (m_Ui->numOfRowsSB->isEnabled())
  {
    if (m_Ui->numOfRowsSB->value() < m_Ui->numOfRowsSB->minimum() || m_Ui->numOfRowsSB->value() > m_Ui->numOfRowsSB->maximum())
    {
      result = false;
    }
  }

  if (m_Ui->numOfColsSB->isEnabled())
  {
    if (m_Ui->numOfColsSB->value() < m_Ui->numOfColsSB->minimum() || m_Ui->numOfColsSB->value() > m_Ui->numOfColsSB->maximum())
    {
      result = false;
    }
  }

  if (m_Ui->imageDataContainerPrefixLE->isEnabled())
  {
    if (m_Ui->imageDataContainerPrefixLE->text().isEmpty())
    {
      result = false;
    }
  }
   
  if (m_Ui->cellAttrMatrixNameLE->isEnabled())
  {
    if (m_Ui->cellAttrMatrixNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->imageArrayNameLE->isEnabled())
  {
    if (m_Ui->imageArrayNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->tileOverlapSB->isEnabled())
  {
    if (m_Ui->tileOverlapSB->value() < m_Ui->tileOverlapSB->minimum() || m_Ui->tileOverlapSB->value() > m_Ui->tileOverlapSB->maximum())
    {
      result = false;
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::registerFields()
{
  registerField(ImportMontage::DREAM3D::FieldNames::MontageName, m_Ui->montageNameLE);
  registerField(ImportMontage::DREAM3D::FieldNames::DataFilePath, m_Ui->dataFileLE);
  registerField(ImportMontage::DREAM3D::FieldNames::NumberOfRows, m_Ui->numOfRowsSB);
  registerField(ImportMontage::DREAM3D::FieldNames::NumberOfColumns, m_Ui->numOfColsSB);
  registerField(ImportMontage::DREAM3D::FieldNames::DataContainerPrefix, m_Ui->imageDataContainerPrefixLE);
  registerField(ImportMontage::DREAM3D::FieldNames::CellAttributeMatrixName, m_Ui->cellAttrMatrixNameLE);
  registerField(ImportMontage::DREAM3D::FieldNames::ImageArrayName, m_Ui->imageArrayNameLE);
  registerField(ImportMontage::DREAM3D::FieldNames::TileOverlap, m_Ui->tileOverlapSB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::selectBtn_clicked()
{
  QString filter = tr("DREAM3D File (*.dream3d);;All Files (*.*)");
  QString title = "Select a DREAM3D file";

  QString outputFile = QFileDialog::getOpenFileName(this, title,
		getInputDirectory(), filter);

	if (!outputFile.isNull())
	{
    m_Ui->dataFileLE->blockSignals(true);
    m_Ui->dataFileLE->setText(QDir::toNativeSeparators(outputFile));
    dataFile_textChanged(m_Ui->dataFileLE->text());
		setOpenDialogLastFilePath(outputFile);
    m_Ui->dataFileLE->blockSignals(false);
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::dataFile_textChanged(const QString& text)
{
	Q_UNUSED(text)

		SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
	QString inputPath = validator->convertToAbsolutePath(text);

  if (QtSFileUtils::VerifyPathExists(inputPath, m_Ui->dataFileLE))
	{
	}

  if (!inputPath.isEmpty())
  {
    QFileInfo fi(inputPath);
    QString ext = fi.completeSuffix();
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(inputPath, QMimeDatabase::MatchContent);

    setFinalPage(false);
  }

	emit completeChanged();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::setInputDirectory(QString val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnterDREAM3DDataPage::getInputDirectory()
{
  if (m_Ui->dataFileLE->text().isEmpty())
	{
		return QDir::homePath();
	}
  return m_Ui->dataFileLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnterDREAM3DDataPage::nextId() const
{
  return ImportMontageWizard::WizardPages::LoadHDF5Data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::cleanupPage()
{
  // Do not return the page to its original values if the user clicks back.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterDREAM3DDataPage::validatePage()
{
  return true;
}