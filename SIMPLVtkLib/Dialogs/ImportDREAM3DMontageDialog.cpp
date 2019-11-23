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

#include "ImportDREAM3DMontageDialog.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtGui/QMovie>

#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

// Initialize private static member variable
QString ImportDREAM3DMontageDialog::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDREAM3DMontageDialog::ImportDREAM3DMontageDialog(QWidget* parent)
: AbstractImportMontageDialog(parent)
, m_Ui(new Ui::ImportDREAM3DMontageDialog)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDREAM3DMontageDialog::~ImportDREAM3DMontageDialog()
{
  delete m_LoadingMovie;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDREAM3DMontageDialog::Pointer ImportDREAM3DMontageDialog::New(QWidget* parent)
{
  Pointer sharedPtr(new ImportDREAM3DMontageDialog(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::setupGui()
{
  m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Import");

  m_Ui->loadHDF5DataWidget->setNavigationButtonsVisibility(false);
  m_Ui->loadHDF5DataWidget->setReadOnly(true);

  connectSignalsSlots();

  m_Ui->rowStart->setValidator(new QIntValidator(m_Ui->rowStart));
  m_Ui->rowEnd->setValidator(new QIntValidator(m_Ui->rowEnd));
  m_Ui->colStart->setValidator(new QIntValidator(m_Ui->colStart));
  m_Ui->colEnd->setValidator(new QIntValidator(m_Ui->colEnd));

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::connectSignalsSlots()
{
  connect(m_Ui->loadHDF5DataWidget, &VSLoadHDF5DataWidget::proxyChanged, this, &ImportDREAM3DMontageDialog::proxyChanged);
  connect(m_Ui->selectButton, &QPushButton::clicked, this, &ImportDREAM3DMontageDialog::selectBtn_clicked);

  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &ImportDREAM3DMontageDialog::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &ImportDREAM3DMontageDialog::dataFile_textChanged);

  connect(m_Ui->montageNameLE, &QLineEdit::textChanged, this, [=] { checkComplete(); });

  connect(m_Ui->rowStart, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->rowEnd, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->colStart, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->colEnd, &QLineEdit::textChanged, [=] { checkComplete(); });

  connect(m_Ui->loadHDF5DataWidget, &VSLoadHDF5DataWidget::proxyChanged, this, &ImportDREAM3DMontageDialog::proxyChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::proxyChanged(DataContainerArrayProxy proxy)
{
  size_t checkCount = 0;
  QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();

  int rowCount = 0;
  int colCount = 0;

  for(QMap<QString, DataContainerProxy>::iterator iter = dataContainers.begin(); iter != dataContainers.end(); iter++)
  {
    DataContainerProxy dcProxy = iter.value();
    if(dcProxy.getFlag() == Qt::Checked || dcProxy.getFlag() == Qt::PartiallyChecked)
    {
      checkCount++;
    }

    QString dcName = dcProxy.getName();
    QString rowColString = dcName.split("_").last();
    QString rowString = rowColString.split("c").first().replace("r", "");
    QString colString = rowColString.split("c").last();
    int rowNumber = rowString.toInt();
    if(rowNumber >= rowCount)
    {
      rowCount = rowNumber;
    }
    int colNumber = colString.toInt();
    if(colNumber >= colCount)
    {
      colCount = colNumber;
    }
  }

  m_Ui->rowEnd->setText(QString::number(rowCount));
  m_Ui->colEnd->setText(QString::number(colCount));

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::checkComplete() const
{
  bool result = true;

  if(m_Ui->montageNameLE->isEnabled())
  {
    if(m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->dataFileLE->isEnabled())
  {
    if(m_Ui->dataFileLE->text().isEmpty())
    {
      result = false;
    }
  }

  QFileInfo fi(m_Ui->dataFileLE->text());
  if(fi.completeSuffix() != "dream3d")
  {
    result = false;
  }

  if(m_LoadingProxy)
  {
    result = false;
  }

  QPushButton* okBtn = m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
  if(okBtn == nullptr)
  {
    throw InvalidOKButtonException();
  }

  okBtn->setEnabled(result);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::initializePage()
{
  QString filePath = m_Ui->dataFileLE->text();

  m_Ui->loadHDF5DataWidget->initialize(filePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::selectBtn_clicked()
{
  QString filter = tr("DREAM3D File (*.dream3d);;All Files (*.*)");
  QString title = "Select a DREAM3D file";

  QString outputFile = QFileDialog::getOpenFileName(this, title, getInputDirectory(), filter);

  if(!outputFile.isNull())
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
void ImportDREAM3DMontageDialog::dataFile_textChanged(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  if(QtSFileUtils::VerifyPathExists(inputPath, m_Ui->dataFileLE))
  {
  }

  QString filePath = m_Ui->dataFileLE->text();

  m_Ui->loadHDF5DataWidget->initialize(filePath);

  checkComplete();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::setInputDirectory(const QString& val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportDREAM3DMontageDialog::getInputDirectory()
{
  if(m_Ui->dataFileLE->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_Ui->dataFileLE->text();
}

// -----------------------------------------------------------------------------
DREAM3DMontageMetadata ImportDREAM3DMontageDialog::getMetadata() const
{
  DREAM3DMontageMetadata metadata;
  metadata.setMontageName(m_Ui->montageNameLE->text());
  metadata.setDataDisplayType(static_cast<MontageMetadata::DisplayType>(m_Ui->dataDisplayTypeCB->currentIndex()));
  metadata.setRowLimits({m_Ui->rowStart->text().toInt(), m_Ui->rowEnd->text().toInt()});
  metadata.setColumnLimits({m_Ui->colStart->text().toInt(), m_Ui->colEnd->text().toInt()});
  metadata.setFilePath(m_Ui->dataFileLE->text());
  metadata.setDataContainerPrefix(m_Ui->dcPrefixLE->text());
  metadata.setAttributeMatrixName(m_Ui->cellAttrMatrixNameLE->text());
  metadata.setDataArrayName(m_Ui->imageArrayNameLE->text());
  return metadata;
}
