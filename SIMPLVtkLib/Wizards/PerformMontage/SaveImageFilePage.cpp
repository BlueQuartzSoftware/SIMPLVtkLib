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

#include "SaveImageFilePage.h"

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

#include "PerformMontage/PerformMontageConstants.h"

#include "PerformMontageWizard.h"

// Initialize private static member variable
QString SaveImageFilePage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SaveImageFilePage::SaveImageFilePage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::SaveImageFilePage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SaveImageFilePage::~SaveImageFilePage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImageFilePage::setupGui()
{
	connectSignalsSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImageFilePage::connectSignalsSlots()
{
  connect(m_Ui->selectButton, &QPushButton::clicked, this, &SaveImageFilePage::selectBtn_clicked);

	QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &SaveImageFilePage::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &SaveImageFilePage::dataFile_textChanged);

  connect(m_Ui->dataContainerNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->cellAttrMatrixNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageArrayNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SaveImageFilePage::isComplete() const
{
  bool result = true;

  if (m_Ui->dataFileLE->isEnabled())
  {
    if (m_Ui->dataFileLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->dataContainerNameLE->isEnabled())
  {
    if (m_Ui->dataContainerNameLE->text().isEmpty())
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

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImageFilePage::initializePage()
{
  // Add Plane options
  m_Ui->planeCB->addItem("XY");
  m_Ui->planeCB->addItem("XZ");
  m_Ui->planeCB->addItem("YZ");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImageFilePage::registerFields()
{
  registerField(PerformMontage::FieldNames::OutputFilePath, m_Ui->dataFileLE);
  registerField(PerformMontage::FieldNames::OutputDataContainerName, m_Ui->dataContainerNameLE);
  registerField(PerformMontage::FieldNames::OutputCellAttributeMatrixName, m_Ui->cellAttrMatrixNameLE);
  registerField(PerformMontage::FieldNames::OutputImageArrayName, m_Ui->imageArrayNameLE);
  registerField(PerformMontage::FieldNames::OutputPlane, m_Ui->planeCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImageFilePage::selectBtn_clicked()
{
  QString filter = tr("Image File (*.png *.tiff *.jpeg *.bmp);;All Files (*.*)");
  QString title = "Save as Image file";

  QString outputFile = QFileDialog::getSaveFileName(this, title,
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
void SaveImageFilePage::dataFile_textChanged(const QString& text)
{
	Q_UNUSED(text)

		SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
	QString outputPath = validator->convertToAbsolutePath(text);

	emit completeChanged();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImageFilePage::setInputDirectory(QString val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SaveImageFilePage::getInputDirectory()
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
int SaveImageFilePage::nextId() const
{
	return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImageFilePage::cleanupPage()
{
  // Do not return the page to its original values if the user clicks back.
}
