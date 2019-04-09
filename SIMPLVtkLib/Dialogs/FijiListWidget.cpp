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

#include "FijiListWidget.h"

//-- Qt Includes
#include <QtCore/QDir>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SIMPLib/Utilities/StringOperations.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString FijiListWidget::m_OpenDialogLastFilePath = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FijiListWidget::FijiListWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::FijiListWidget)
{
  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FijiListWidget::~FijiListWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FijiListWidget::setWidgetListEnabled(bool b)
{
  foreach(QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FijiListWidget::setupGui()
{
  connectSignalsSlots();

  setupMenuField();

  m_WidgetList << m_Ui->inputDir << m_Ui->inputDirBtn;

  m_Ui->errorMessage->setVisible(false);

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
void FijiListWidget::connectSignalsSlots()
{
  // Connections for the various ui widgets
  connect(m_Ui->inputDirBtn, &QPushButton::clicked, this, &FijiListWidget::inputDirBtn_clicked);
  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->inputDir->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &FijiListWidget::inputDir_textChanged);
  connect(m_Ui->inputDir, &QtSLineEdit::textChanged, this, &FijiListWidget::inputDir_textChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FijiListWidget::keyPressEvent(QKeyEvent* event)
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
void FijiListWidget::setupMenuField()
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
void FijiListWidget::showFileInFileSystem(const QString& filePath)
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
void FijiListWidget::validateInputFile()
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
void FijiListWidget::inputDirBtn_clicked()
{
  QString filter = tr("Fiji Configuration File (*.txt);;All Files (*.*)");
  QString title = "Select a Fiji configuration file";

  QStringList outputFiles = QFileDialog::getOpenFileNames(this, title, getInputDirectory(), filter);

  for(QString outputFile : outputFiles)
  {
    if(!outputFile.isNull())
    {
      m_Ui->inputDir->blockSignals(true);
      m_Ui->inputDir->setText(QDir::toNativeSeparators(outputFile));
      inputDir_textChanged(m_Ui->inputDir->text());
      setOpenDialogLastFilePath(outputFile);
      m_Ui->inputDir->blockSignals(false);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FijiListWidget::inputDir_textChanged(const QString& text)
{
  Q_UNUSED(text)

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  m_Ui->inputDir->setToolTip("Absolute File Path: " + inputPath);

  if(QtSFileUtils::VerifyPathExists(inputPath, m_Ui->inputDir))
  {
    m_ShowFileAction->setEnabled(true);
    QStringList filenameList = readFijiConfigFile();
    QDir dir(inputPath);
    QString dirname = dir.dirName();
    dir.cdUp();

    generateExampleInputFile(filenameList);
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
void FijiListWidget::generateExampleInputFile(QStringList filenameList)
{
  QFileInfo fi(m_Ui->inputDir->text());
  bool hasMissingFiles = false;

  fi.setFile(m_Ui->inputDir->text());

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(fi.path());

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = generateFileList(filenameList, hasMissingFiles, inputPath);
  m_Ui->fileListView->clear();
  QIcon greenDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_green.png"));
  QIcon redDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_red.png"));
  int fileExistsCount = 0;
  for(QVector<QString>::size_type i = 0; i < filenameList.size(); ++i)
  {
    QString filePath(fileList.at(i));
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(filePath, m_Ui->fileListView);
    if(fi.exists())
    {
      item->setIcon(greenDot);
      fileExistsCount++;
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

  m_Ui->totalFilesFound->setText(tr("%1/%2").arg(fileExistsCount).arg(fileList.size()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> FijiListWidget::generateFileList(QStringList filenameList, bool& hasMissingFiles, const QString& inputPath)
{
  QVector<QString> fileList;
  //  QDir dir(inputPath);
  //  if(!dir.exists())
  //  {
  //    return fileList;
  //  }

  if(inputPath.isEmpty())
  {
    return fileList;
  }

  bool missingFiles = false;
  for(QString filename : filenameList)
  {
    QString filePath = inputPath + QDir::separator() + filename;
    filePath = QDir::toNativeSeparators(filePath);

    QFileInfo fi(filePath);
    if(!fi.exists())
    {
      missingFiles = true;
    }

    fileList.push_back(filePath);
  }

  hasMissingFiles = missingFiles;

  return fileList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList FijiListWidget::readFijiConfigFile()
{
  QFile file(m_Ui->inputDir->text());
  if(!file.open(QIODevice::ReadOnly))
  {
    qDebug() << file.errorString();
  }

  QStringList fileNameList;

  while(!file.atEnd())
  {
    QByteArray line = file.readLine().trimmed();

    // this is either an empty line, a comment, or a dimension declaration
    if(line.isEmpty() || line[0] == '#' || line.startsWith("dim"))
    {
      continue;
    }

    fileNameList.append(line.split(';').first());
  }

  return fileNameList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FijiListInfo_t FijiListWidget::getFijiListInfo()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  FijiListInfo_t data;
  data.FijiFilePath = inputPath;

  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FijiListWidget::isComplete() const
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
void FijiListWidget::setInputDirectory(QString val)
{
  m_Ui->inputDir->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FijiListWidget::getInputDirectory()
{
  if(m_Ui->inputDir->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_Ui->inputDir->text();
}
