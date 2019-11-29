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

#include "RobometListWidget.h"

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
QString RobometListWidget::m_OpenDialogLastFilePath = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RobometListWidget::RobometListWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::RobometListWidget)
{
  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RobometListWidget::~RobometListWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::setWidgetListEnabled(bool b)
{
  foreach(QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::setupGui()
{
  connectSignalsSlots();

  setupMenuField();

  m_Ui->rowStart->setMaximum(std::numeric_limits<int>().max());
  m_Ui->rowEnd->setMaximum(std::numeric_limits<int>().max());
  m_Ui->colStart->setMaximum(std::numeric_limits<int>().max());
  m_Ui->colEnd->setMaximum(std::numeric_limits<int>().max());
  m_Ui->sliceMin->setMaximum(std::numeric_limits<int>().max());
  m_Ui->sliceMax->setMaximum(std::numeric_limits<int>().max());

  m_Ui->absPathLabel->hide();

  m_WidgetList << m_Ui->inputDir << m_Ui->inputDirBtn;
  m_WidgetList << m_Ui->filePrefix << m_Ui->fileExt << m_Ui->sliceMin << m_Ui->sliceMax;
  m_WidgetList << m_Ui->rowStart << m_Ui->rowEnd;
  m_WidgetList << m_Ui->colStart << m_Ui->colEnd;

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
void RobometListWidget::connectSignalsSlots()
{
  // Connections for the various ui widgets
  connect(m_Ui->inputDirBtn, &QPushButton::clicked, this, &RobometListWidget::inputDirBtn_clicked);
  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->inputDir->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &RobometListWidget::inputDir_textChanged);
  connect(m_Ui->inputDir, &QtSLineEdit::textChanged, this, &RobometListWidget::inputDir_textChanged);

  connect(m_Ui->filePrefix, &QtSLineEdit::textChanged, this, [=](const QString& filePrefix) {
    generateExampleInputFile();
    emit filePrefixChanged(filePrefix);
  });

  connect(m_Ui->fileExt, &QtSLineEdit::textChanged, this, [=](const QString& fileExtension) {
    generateExampleInputFile();
    emit fileExtensionChanged(fileExtension);
  });

  connect(m_Ui->sliceMin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit sliceMinChanged(value);
  });

  connect(m_Ui->sliceMax, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit sliceMaxChanged(value);
  });

  connect(m_Ui->colStart, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit montageStartColChanged(value);
  });

  connect(m_Ui->rowStart, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit montageStartRowChanged(value);
  });

  connect(m_Ui->colEnd, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit montageEndColChanged(value);
  });

  connect(m_Ui->rowEnd, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
    generateExampleInputFile();
    emit montageEndRowChanged(value);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::keyPressEvent(QKeyEvent* event)
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
void RobometListWidget::setupMenuField()
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
void RobometListWidget::showFileInFileSystem(const QString& filePath)
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
void RobometListWidget::validateInputFile()
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
void RobometListWidget::inputDirBtn_clicked()
{
  QString filter = tr("Robomet Configuration File (*.csv);;All Files (*.*)");
  QString title = "Select a Robomet configuration file";

  QString outputFile = QFileDialog::getOpenFileName(this, title, getInputDirectory(), filter);

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
void RobometListWidget::inputDir_textChanged(const QString& text)
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
    findPrefix();
    findFileExtension();
    findNumberOfRowsAndColumns();
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
void RobometListWidget::generateExampleInputFile()
{
  QString rowString = StringOperations::GeneratePaddedString(0, k_RowColPadding, '0');
  QString colString = StringOperations::GeneratePaddedString(0, k_RowColPadding, '0');
  QString sliceMinString = StringOperations::GeneratePaddedString(m_Ui->sliceMin->value(), k_SlicePadding, '0');
  QFileInfo fi(m_Ui->inputDir->text());
  QString parentPath = tr("%1%2").arg(fi.path()).arg(QDir::separator());
  QString filename = QString("%1%2%3_%4_%5.%6").arg(parentPath).arg(m_Ui->filePrefix->text()).arg(sliceMinString).arg(rowString).arg(colString).arg(m_Ui->fileExt->text());
  m_Ui->generatedFileNameExample->setText(filename);

  int sliceMin = m_Ui->sliceMin->value();
  int sliceMax = m_Ui->sliceMax->value();
  int montageStartCol = m_Ui->colStart->value();
  int montageStartRow = m_Ui->rowStart->value();
  int montageEndCol = m_Ui->colEnd->value();
  int montageEndRow = m_Ui->rowEnd->value();
  QString prefix = m_Ui->filePrefix->text();
  QString ext = m_Ui->fileExt->text();
  bool hasMissingFiles = false;

  fi.setFile(m_Ui->inputDir->text());

  m_Ui->fileListView->clear();

  int fileExistsCount = 0;
  int totalFileCount = 0;
  for(int slice = sliceMin; slice <= sliceMax; slice++)
  {
    QString sliceString = StringOperations::GeneratePaddedString(slice, k_SlicePadding, '0');
    QString slicePath = tr("%1%2%3_%4").arg(fi.path()).arg(QDir::separator()).arg(m_Ui->filePrefix->text()).arg(sliceString);

    SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
    QString inputPath = validator->convertToAbsolutePath(slicePath);

    // Now generate all the file names the user is asking for and populate the table
    QVector<QString> fileList = generateFileList(slice, montageStartCol, montageStartRow, montageEndCol, montageEndRow, hasMissingFiles, inputPath, prefix, ext);

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
        fileExistsCount++;
      }
      else
      {
        hasMissingFiles = true;
        item->setIcon(redDot);
      }
    }

    totalFileCount += fileList.size();
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

  m_Ui->totalFilesFound->setText(tr("%1/%2").arg(fileExistsCount).arg(totalFileCount));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> RobometListWidget::generateFileList(int sliceNumber, int montageStartCol, int montageStartRow, int montageEndCol, int montageEndRow, bool& hasMissingFiles, const QString& inputPath, const QString& filePrefix,
                                                     const QString& fileExtension)
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
  for(int row = montageStartRow; row <= montageEndRow; row++)
  {
    for(int col = montageStartCol; col <= montageEndCol; col++)
    {
      QString filename;
      if(!filePrefix.isEmpty())
      {
        filename.append(QString("%1_").arg(filePrefix));
      }

      filename.append(QString("%1_%2_%3").arg(QString::number(sliceNumber), k_SlicePadding, '0').arg(QString::number(col), k_RowColPadding, '0').arg(QString::number(row), k_RowColPadding, '0'));

      filename.append(QString(".%5").arg(fileExtension));

      QString filePath = inputPath + QDir::separator() + filename;
      filePath = QDir::toNativeSeparators(filePath);

      QFileInfo fi(filePath);
      if(!fi.exists())
      {
        missingFiles = true;
      }

      fileList.push_back(filePath);
    }
  }

  hasMissingFiles = missingFiles;

  return fileList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::findPrefix()
{
  QString sliceString = StringOperations::GeneratePaddedString(m_Ui->sliceMin->value(), k_SlicePadding, '0');
  QFileInfo inputFi(m_Ui->inputDir->text());

  // Find Prefix
  {
    QDir dir(inputFi.path());
    dir.setFilter(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    QFileInfoList fiList = dir.entryInfoList();
    for(QFileInfo fi : fiList)
    {
      QString folderName = fi.baseName();
      if(folderName.endsWith(tr("_%1").arg(sliceString)))
      {
        folderName.remove(tr("_%1").arg(sliceString));
        m_Ui->filePrefix->setText(folderName);
        return;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::findFileExtension()
{
  QFileInfo fi(m_Ui->inputDir->text());
  QString sliceString = StringOperations::GeneratePaddedString(m_Ui->sliceMin->value(), k_SlicePadding, '0');
  QString slicePath = tr("%1%2%3_%4").arg(fi.path()).arg(QDir::separator()).arg(m_Ui->filePrefix->text()).arg(sliceString);

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(slicePath);

  QDir dir(inputPath);
  dir.setFilter(QDir::Filter::Files);
  QFileInfoList fiList = dir.entryInfoList();
  QMap<QString, int> extCount;
  for(QFileInfo fi : fiList)
  {
    QString ext = fi.completeSuffix();
    extCount.insert(ext, extCount[ext] + 1);
  }

  QString mostCommonExt = "";
  int largestNumber = 0;
  for(QMap<QString, int>::iterator iter = extCount.begin(); iter != extCount.end(); iter++)
  {
    if(iter.value() > largestNumber)
    {
      largestNumber = iter.value();
      mostCommonExt = iter.key();
    }
  }

  m_Ui->fileExt->setText(mostCommonExt);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::findNumberOfRowsAndColumns()
{
  QFileInfo fi(m_Ui->inputDir->text());
  QString sliceString = StringOperations::GeneratePaddedString(m_Ui->sliceMin->value(), k_SlicePadding, '0');
  QString slicePath = tr("%1%2%3_%4").arg(fi.path()).arg(QDir::separator()).arg(m_Ui->filePrefix->text()).arg(sliceString);

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(slicePath);

  QDir dir(inputPath);
  dir.setFilter(QDir::Filter::Files);
  QStringList nameFilters;
  nameFilters << tr("*.%1").arg(m_Ui->fileExt->text());
  dir.setNameFilters(nameFilters);
  QFileInfoList fiList = dir.entryInfoList();
  int maxRow = 0;
  int maxCol = 0;
  for(const auto& fi : fiList)
  {
    QString fileBaseName = fi.baseName();
    QStringList tokens = fileBaseName.split("_");
    if(tokens.size() > 1) // Only get the rows and columns if they exist in the file name
    {
      int col = tokens[tokens.size() - 2].toInt();
      int row = tokens[tokens.size() - 1].toInt();

      if(row > maxRow)
      {
        maxRow = row;
      }

      if(col > maxCol)
      {
        maxCol = col;
      }
    }
  }

  QString montageInfo;
  QTextStream ss(&montageInfo);
  ss << "Max Column: " << maxCol - 1 << "  Max Row: " << maxRow - 1 << "  Image Count: " << (maxCol + 1) * (maxRow + 1);
  m_Ui->montageInfoLabel->setText(montageInfo);

  m_Ui->colStart->setValue(0);
  m_Ui->rowStart->setValue(0);
  m_Ui->colEnd->setValue(maxCol);
  m_Ui->rowEnd->setValue(maxRow);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool RobometListWidget::isComplete() const
{
  if(m_Ui->sliceMin->value() > m_Ui->sliceMax->value())
  {
    return false;
  }

  if(m_Ui->fileListView->count() <= 0)
  {
    return false;
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
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::setInputDirectory(QString val)
{
  m_Ui->inputDir->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RobometListWidget::getInputDirectory()
{
  if(m_Ui->inputDir->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_Ui->inputDir->text();
}

// -----------------------------------------------------------------------------
RobometMontageMetadata RobometListWidget::getMetadata() const
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  RobometMontageMetadata metadata;
  metadata.setFilePath(inputPath);
  metadata.setFilePrefix(m_Ui->filePrefix->text());
  metadata.setFileExtension(m_Ui->fileExt->text());
  metadata.setRowLimits({m_Ui->rowStart->value(), m_Ui->rowEnd->value()});
  metadata.setColumnLimits({m_Ui->colStart->value(), m_Ui->colEnd->value()});
  metadata.setSliceLimits({m_Ui->sliceMin->value(), m_Ui->sliceMax->value()});
  return metadata;
}
