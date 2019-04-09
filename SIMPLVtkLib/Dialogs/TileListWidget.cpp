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

#include "TileListWidget.h"

//-- Qt Includes
#include <QtCore/QDir>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SIMPLib/Utilities/StringOperations.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString TileListWidget::m_OpenDialogLastFilePath = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TileListWidget::TileListWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::TileListWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  if(m_Ui->inputDir->text().isEmpty())
  {
    setInputDirectory(QDir::homePath());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TileListWidget::~TileListWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::setWidgetListEnabled(bool b)
{
  foreach(QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::setupGui()
{
  connectSignalsSlots();

  setupMenuField();

  m_Ui->absPathLabel->hide();

  m_WidgetList << m_Ui->inputDir << m_Ui->inputDirBtn;
  m_WidgetList << m_Ui->fileExt << m_Ui->errorMessage << m_Ui->totalDigits << m_Ui->fileSuffix;
  m_WidgetList << m_Ui->filePrefix << m_Ui->totalSlices << m_Ui->startIndex << m_Ui->endIndex;

  m_Ui->errorMessage->setVisible(false);

  m_OrderingGroup = new QButtonGroup(this);
  m_OrderingGroup->addButton(m_Ui->orderAscending);
  m_OrderingGroup->addButton(m_Ui->orderDescending);

  // Update the widget when the data directory changes
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  connect(validator, &SIMPLDataPathValidator::dataDirectoryChanged, [=] {
    blockSignals(true);
    inputDir_textChanged(m_Ui->inputDir->text());
    blockSignals(false);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::connectSignalsSlots()
{
  // Connections for the various ui widgets
  connect(m_Ui->inputDirBtn, &QPushButton::clicked, this, &TileListWidget::inputDirBtn_clicked);

  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->inputDir->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &TileListWidget::inputDir_textChanged);
  connect(m_Ui->inputDir, &QtSLineEdit::textChanged, this, &TileListWidget::inputDir_textChanged);

  connect(m_Ui->filePrefix, &QtSLineEdit::textChanged, this, [=](const QString& filePrefix) {
    generateExampleInputFile();
    emit filePrefixChanged(filePrefix);
  });

  connect(m_Ui->fileSuffix, &QtSLineEdit::textChanged, this, [=](const QString& fileSuffix) {
    generateExampleInputFile();
    emit fileSuffixChanged(fileSuffix);
  });

  connect(m_Ui->fileExt, &QtSLineEdit::textChanged, this, [=](const QString& fileExtension) {
    generateExampleInputFile();
    emit fileExtensionChanged(fileExtension);
  });

  connect(m_Ui->totalDigits, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit paddingDigitsChanged(value);
  });

  connect(m_Ui->startIndex, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit startIndexChanged(value);
  });

  connect(m_Ui->endIndex, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit endIndexChanged(value);
  });

  connect(m_Ui->increment, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit incrementIndexChanged(value);
  });

  connect(m_Ui->orderAscending, &QRadioButton::toggled, this, [=](bool checked) {
    generateExampleInputFile();
    if(checked)
    {
      emit fileOrderingChanged(0);
    }
    else
    {
      emit fileOrderingChanged(1);
    }
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Escape)
  {
    m_Ui->inputDir->setText(m_CurrentText);
    m_Ui->inputDir->setStyleSheet("");
    m_Ui->inputDir->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::setupMenuField()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  QFileInfo fi(inputPath);

  QMenu* lineEditMenu = new QMenu(m_Ui->inputDir);
  m_Ui->inputDir->setButtonMenu(QtSLineEdit::Left, lineEditMenu);
  QLatin1String iconPath = QLatin1String(":/SIMPL/icons/images/caret-bottom.png");

  m_Ui->inputDir->setButtonVisible(QtSLineEdit::Left, true);

  QPixmap pixmap(8, 8);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  const QPixmap mag = QPixmap(iconPath);
  painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
  m_Ui->inputDir->setButtonPixmap(QtSLineEdit::Left, pixmap);

  {
    m_ShowFileAction = new QAction(lineEditMenu);
    m_ShowFileAction->setObjectName(QString::fromUtf8("showFileAction"));
#if defined(Q_OS_WIN)
    m_ShowFileAction->setText("Show in Windows Explorer");
#elif defined(Q_OS_MAC)
    m_ShowFileAction->setText("Show in Finder");
#else
    m_ShowFileAction->setText("Show in File System");
#endif
    lineEditMenu->addAction(m_ShowFileAction);
    connect(m_ShowFileAction, &QAction::triggered, [=] { showFileInFileSystem(m_Ui->inputDir->text()); });
  }

  if(!inputPath.isEmpty() && fi.exists())
  {
    m_ShowFileAction->setEnabled(true);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::showFileInFileSystem(const QString& filePath)
{
  QFileInfo fi(filePath);
  QString path;
  if(fi.isFile())
  {
    path = fi.absoluteFilePath();
  }
  else
  {
    path = fi.absolutePath();
  }

  QtSFileUtils::ShowPathInGui(this, fi.absoluteFilePath());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::validateInputFile()
{
  QString currentPath = m_Ui->inputDir->text();
  QFileInfo fi(currentPath);
  if(!currentPath.isEmpty() && !fi.exists())
  {
    QString defaultName = m_OpenDialogLastFilePath;

    QString title = QObject::tr("Select a replacement input file");

    QString file = QFileDialog::getExistingDirectory(this, title, getInputDirectory(), QFileDialog::ShowDirsOnly);
    if(file.isEmpty())
    {
      file = currentPath;
    }
    file = QDir::toNativeSeparators(file);
    // Store the last used directory into the private instance variable
    QFileInfo fi(file);

    setInputDirectory(fi.filePath());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::checkIOFiles()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  if(QtSFileUtils::VerifyPathExists(inputPath, m_Ui->inputDir))
  {
    findMaxSliceAndPrefix();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::inputDirBtn_clicked()
{
  QString outputFile = QFileDialog::getExistingDirectory(this, tr("Select Input Directory"), getInputDirectory());

  if(!outputFile.isNull())
  {
    m_Ui->inputDir->blockSignals(true);
    m_Ui->inputDir->setText(QDir::toNativeSeparators(outputFile));
    inputDir_textChanged(m_Ui->inputDir->text());
    setOpenDialogLastFilePath(outputFile);
    m_Ui->inputDir->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::inputDir_textChanged(const QString& text)
{
  Q_UNUSED(text)

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  QFileInfo fi(text);
  if(fi.isRelative())
  {
    m_Ui->absPathLabel->setText(inputPath);
    m_Ui->absPathLabel->show();
  }
  else
  {
    m_Ui->absPathLabel->hide();
  }

  m_Ui->inputDir->setToolTip("Absolute File Path: " + inputPath);

  if(QtSFileUtils::VerifyPathExists(inputPath, m_Ui->inputDir))
  {
    m_ShowFileAction->setEnabled(true);
    findMaxSliceAndPrefix();
    QDir dir(inputPath);
    QString dirname = dir.dirName();
    dir.cdUp();

    generateExampleInputFile();
    m_Ui->inputDir->blockSignals(true);
    m_Ui->inputDir->setText(QDir::toNativeSeparators(m_Ui->inputDir->text()));
    m_Ui->inputDir->blockSignals(false);
  }
  else
  {
    m_ShowFileAction->setEnabled(false);
    m_Ui->fileListView->clear();
  }

  emit inputDirectoryChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t TileListWidget::getOrdering()
{
  if(m_Ui->orderAscending->isChecked())
  {
    return SIMPL::RefFrameZDir::LowtoHigh;
  }
  if(m_Ui->orderDescending->isChecked())
  {
    return SIMPL::RefFrameZDir::HightoLow;
  }
  return SIMPL::RefFrameZDir::UnknownRefFrameZDirection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::setOrdering(uint32_t ref)
{
  if(ref == SIMPL::RefFrameZDir::LowtoHigh)
  {
    m_Ui->orderAscending->setChecked(true);
  }
  if(ref == SIMPL::RefFrameZDir::HightoLow)
  {
    m_Ui->orderDescending->setChecked(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::generateExampleInputFile()
{
  QString indexString = StringOperations::GeneratePaddedString(m_Ui->startIndex->value(), m_Ui->totalDigits->value(), '0');
  QString filename = QString("%1%2%3.%4").arg(m_Ui->filePrefix->text()).arg(indexString).arg(m_Ui->fileSuffix->text()).arg(m_Ui->fileExt->text());
  m_Ui->generatedFileNameExample->setText(filename);

  int start = m_Ui->startIndex->value();
  int end = m_Ui->endIndex->value();
  int increment = m_Ui->increment->value();
  bool hasMissingFiles = false;

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(start, end, increment, hasMissingFiles, m_Ui->orderAscending->isChecked(), inputPath, m_Ui->filePrefix->text(),
                                                                  m_Ui->fileSuffix->text(), m_Ui->fileExt->text(), m_Ui->totalDigits->value());
  m_Ui->fileListView->clear();
  QIcon greenDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_green.png"));
  QIcon redDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_red.png"));
  for(QVector<QString>::size_type i = 0; i < fileList.size(); ++i)
  {
    QString filePath(fileList.at(i));
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(filePath, m_Ui->fileListView);
    if(fi.exists())
    {
      item->setIcon(greenDot);
    }
    else
    {
      hasMissingFiles = true;
      item->setIcon(redDot);
    }
  }

  if(hasMissingFiles)
  {
    m_Ui->errorMessage->setVisible(true);
    m_Ui->errorMessage->setText("Alert: Red Dot File(s) on the list do NOT exist on the filesystem. Please make sure all files exist");
  }
  else
  {
    m_Ui->errorMessage->setVisible(true);
    m_Ui->errorMessage->setText("All files exist.");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::findMaxSliceAndPrefix()
{
  if(m_Ui->inputDir->text().length() == 0)
  {
    return;
  }

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  QDir dir(inputPath);

  // Final check to make sure we have a valid file extension
  if(m_Ui->fileExt->text().isEmpty())
  {
    return;
  }

  QString ext = "." + m_Ui->fileExt->text();
  QStringList filters;
  filters << "*" + ext;
  dir.setNameFilters(filters);
  QFileInfoList angList = dir.entryInfoList();

  int minSlice = 0;
  int maxSlice = 0;
  int currValue = 0;
  bool flag = false;
  bool ok;
  QString fPrefix;
  QRegExp rx("(\\d+)");
  QStringList list;
  int pos = 0;
  int digitStart = 0;
  int digitEnd = 0;
  int totalOimFilesFound = 0;
  int minTotalDigits = 1000;
  foreach(QFileInfo fi, angList)
  {
    if((fi.suffix().compare(ext) != 0) && fi.isFile())
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      QString fns = fn;
      int length = fn.length();
      digitEnd = length - 1;
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        --digitEnd;
      }
      pos = digitEnd;

      digitStart = pos = rx.indexIn(fn, pos);
      digitEnd = digitStart;
      while((pos = rx.indexIn(fn, pos)) != -1)
      {
        list << rx.cap(0);
        fPrefix = fn.left(pos);
        pos += rx.matchedLength();
      }
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        ++digitEnd;
      }

      if(digitEnd - digitStart < minTotalDigits)
      {
        minTotalDigits = digitEnd - digitStart;
      }
      m_Ui->totalDigits->setValue(minTotalDigits);
      if(!list.empty())
      {
        currValue = list.front().toInt(&ok);
        if(!flag)
        {
          minSlice = currValue;
          flag = true;
        }
        if(currValue > maxSlice)
        {
          maxSlice = currValue;
        }
        if(currValue < minSlice)
        {
          minSlice = currValue;
        }
      }
      ++totalOimFilesFound;
    }
  }
  this->m_Ui->totalSlices->setText(QString::number(totalOimFilesFound));
  this->m_Ui->filePrefix->setText(fPrefix);
  this->m_Ui->startIndex->setValue(minSlice);
  this->m_Ui->endIndex->setValue(maxSlice);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfo_t TileListWidget::getFileListInfo()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  FileListInfo_t data;
  data.IncrementIndex = m_Ui->increment->value();
  data.EndIndex = m_Ui->endIndex->value();
  data.FileExtension = m_Ui->fileExt->text();
  data.FilePrefix = m_Ui->filePrefix->text();
  data.FileSuffix = m_Ui->fileSuffix->text();
  data.InputPath = inputPath;
  data.Ordering = getOrdering();
  data.PaddingDigits = m_Ui->totalDigits->value();
  data.StartIndex = m_Ui->startIndex->value();

  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool TileListWidget::isComplete() const
{
  bool result = true;

  if(m_Ui->fileListView->count() <= 0)
  {
    result = false;
  }
  else
  {
    int fileCount = m_Ui->fileListView->count();
    for(int i = 0; i < fileCount; i++)
    {
      QListWidgetItem* item = m_Ui->fileListView->item(i);
      QString filePath = item->text();
      QFileInfo fi(filePath);
      if(!fi.exists())
      {
        result = false;
      }
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileListWidget::setInputDirectory(QString val)
{
  m_Ui->inputDir->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString TileListWidget::getInputDirectory()
{
  if(m_Ui->inputDir->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_Ui->inputDir->text();
}
