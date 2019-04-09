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

#pragma once

#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVector>
#include <QtWidgets/QButtonGroup>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include "SVWidgetsLib/QtSupport/QtSPluginFrame.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

#include "ui_RobometListWidget.h"

typedef struct
{
  qint32 NumberOfColumns = 0;
  qint32 NumberOfRows = 0;
  qint32 SliceMin = 0;
  qint32 SliceMax = 0;
  QString RobometFilePath;
  QString ImagePrefix;
  QString ImageExtension;

  void writeJson(QJsonObject& json)
  {
    json["NumberOfColumns"] = static_cast<double>(NumberOfColumns);
    json["NumberOfRows"] = static_cast<double>(NumberOfRows);
    json["SliceMin"] = static_cast<double>(SliceMin);
    json["SliceMax"] = static_cast<double>(SliceMax);
    json["RobometFilePath"] = RobometFilePath;
    json["ImagePrefix"] = ImagePrefix;
    json["ImageExtension"] = ImageExtension;
  }

  bool readJson(QJsonObject& json)
  {
    if(json["PaddingDigits"].isDouble() && json["Ordering"].isDouble() && json["StartIndex"].isDouble() && json["EndIndex"].isDouble() && json["IncrementIndex"].isDouble() &&
       json["InputPath"].isString() && json["FilePrefix"].isString() && json["FileSuffix"].isString() && json["FileExtension"].isString())
    {
      NumberOfColumns = static_cast<qint32>(json["NumberOfColumns"].toDouble());
      NumberOfRows = static_cast<qint32>(json["NumberOfRows"].toDouble());
      SliceMin = static_cast<qint32>(json["SliceMin"].toDouble());
      SliceMax = static_cast<qint32>(json["SliceMax"].toDouble());
      RobometFilePath = json["RobometFilePath"].toString();
      ImagePrefix = json["ImagePrefix"].toString();
      ImageExtension = json["ImageExtension"].toString();
      return true;
    }
    return false;
  }
} RobometListInfo_t;

Q_DECLARE_METATYPE(RobometListInfo_t)

/**
 * @class RobometListWidget RobometListWidget.h Plugins/EbsdImport/UI/RobometListWidget.h
 * @brief This class represents the User Interface for the EBSD file import module
 * of the SIMPLView program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of EBSD data files into an HDF5 file
 * for better data management and archiving.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class SIMPLVtkLib_EXPORT RobometListWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  RobometListWidget(QWidget* parent = nullptr);

  ~RobometListWidget() override;

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void setupGui();

  /**
   * @brief isComplete
   * @return
   */
  bool isComplete() const;

  /**
   * @brief getRobometListInfo
   * @return
   */
  RobometListInfo_t getRobometListInfo();

protected slots:

  // Slots to catch signals emitted by the various ui widgets
  void inputDirBtn_clicked();
  void inputDir_textChanged(const QString& text);

protected:
  void setInputDirectory(QString val);
  QString getInputDirectory();

  static void setOpenDialogLastFilePath(QString val)
  {
    m_OpenDialogLastFilePath = val;
  }
  static QString getOpenDialogLastFilePath()
  {
    return m_OpenDialogLastFilePath;
  }

  /**
   * @brief validateInputFile
   */
  void validateInputFile();

  /**
   * @brief setWidgetListEnabled
   */
  void setWidgetListEnabled(bool v);

  /**
   * @brief Method to attempt the extraction of the file prefix
   */
  void findPrefix();

  /**
   * @brief findFileExtension
   */
  void findFileExtension();

  /**
   * @brief findNumberOfRowsAndColumns
   */
  void findNumberOfRowsAndColumns();

  /**
   * @brief generateExampleInputFile
   */
  void generateExampleInputFile();

  /**
   * @brief
   * @param event
   */
  void keyPressEvent(QKeyEvent* event) override;

  /**
   * @brief setupMenuField
   */
  void setupMenuField();

signals:
  /**
   * @brief inputDirectoryChanged
   * @param dirPath
   */
  void inputDirectoryChanged(const QString& dirPath);

  /**
   * @brief filePrefixChanged
   * @param filePrefix
   */
  void filePrefixChanged(const QString& filePrefix);

  /**
   * @brief fileSuffixChanged
   * @param fileSuffix
   */
  void fileSuffixChanged(const QString& fileSuffix);

  /**
   * @brief fileExtensionChanged
   * @param fileExtension
   */
  void fileExtensionChanged(const QString& fileExtension);

  /**
   * @brief sliceMinChanged
   * @param sliceNumber
   */
  void sliceMinChanged(size_t sliceNumber);

  /**
   * @brief sliceMaxChanged
   * @param sliceNumber
   */
  void sliceMaxChanged(size_t sliceNumber);

  /**
   * @brief numberOfRowsChanged
   * @param numberOfRows
   */
  void numberOfRowsChanged(size_t numberOfRows);

  /**
   * @brief numberOfColumnsChanged
   * @param numOfCols
   */
  void numberOfColumnsChanged(size_t numOfCols);

  /**
   * @brief slicePaddingChanged
   * @param slicePadding
   */
  void slicePaddingChanged(size_t slicePadding);

  /**
   * @brief rowColPaddingChanged
   * @param rowColPadding
   */
  void rowColPaddingChanged(size_t rowColPadding);

private:
  QSharedPointer<Ui::RobometListWidget> m_Ui;

  QList<QWidget*> m_WidgetList;
  static QString m_OpenDialogLastFilePath;

  QAction* m_ShowFileAction = nullptr;
  QString m_CurrentText = "";
  bool m_DidCausePreflight = false;

  const int k_SlicePadding = 6;
  const int k_RowColPadding = 2;

  /**
   * @brief connectSignalsSlots
   */
  void connectSignalsSlots();

  /**
   * @brief showFileInFileSystem
   * @param filePath
   */
  void showFileInFileSystem(const QString& filePath);

  /**
   * @brief GenerateFileList This method will generate a file list using a slice number and row/column counts
   * @param sliceNumber
   * @param numberOfRows
   * @param numberOfColumns
   * @param hasMissingFiles
   * @param inputPath
   * @param filePrefix
   * @param fileExtension
   * @return
   */
  QVector<QString> generateFileList(int sliceNumber, int numberOfRows, int numberOfColumns, bool& hasMissingFiles, const QString& inputPath, const QString& filePrefix, const QString& fileExtension);

public:
  RobometListWidget(const RobometListWidget&) = delete;            // Copy Constructor Not Implemented
  RobometListWidget(RobometListWidget&&) = delete;                 // Move Constructor Not Implemented
  RobometListWidget& operator=(const RobometListWidget&) = delete; // Copy Assignment Not Implemented
  RobometListWidget& operator=(RobometListWidget&&) = delete;      // Move Assignment Not Implemented
};
