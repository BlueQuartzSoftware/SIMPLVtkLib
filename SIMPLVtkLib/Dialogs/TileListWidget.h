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

#include "ui_TileListWidget.h"

/**
 * @class TileListWidget TileListWidget.h Plugins/EbsdImport/UI/TileListWidget.h
 * @brief This class represents the User Interface for the EBSD file import module
 * of the SIMPLView program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of EBSD data files into an HDF5 file
 * for better data management and archiving.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class SIMPLVtkLib_EXPORT TileListWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  TileListWidget(QWidget* parent = nullptr);

  ~TileListWidget() override;

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
   * @brief getFileListInfo
   * @return
   */
  FileListInfo_t getFileListInfo();

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
   * @brief checkIOFiles
   */
  virtual void checkIOFiles();

  /**
   * @brief validateInputFile
   */
  void validateInputFile();

  /**
   * @brief setWidgetListEnabled
   */
  void setWidgetListEnabled(bool v);

  /**
   * @brief Method to attempt the extraction of the file max slice value and prefix
   */
  void findMaxSliceAndPrefix();

  /**
   * @brief generateExampleInputFile
   */
  void generateExampleInputFile();

  /**
   * @brief setRefFrameZDir
   * @param ref
   */
  void setOrdering(uint32_t ref);

  uint32_t getOrdering();

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
   * @brief fileOrderingChanged
   * @param order The order of the files.  0 is ascending, 1 is descending
   */
  void fileOrderingChanged(int order);

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
   * @brief startIndexChanged
   * @param startIndex
   */
  void startIndexChanged(size_t startIndex);

  /**
   * @brief endIndexChanged
   * @param endIndex
   */
  void endIndexChanged(size_t endIndex);

  /**
   * @brief incrementIndexChanged
   * @param incrementIndex
   */
  void incrementIndexChanged(size_t incrementIndex);

  /**
   * @brief paddingDigitsChanged
   * @param paddingDigits
   */
  void paddingDigitsChanged(size_t paddingDigits);

private:
  QSharedPointer<Ui::TileListWidget> m_Ui;

  QList<QWidget*> m_WidgetList;
  QButtonGroup* m_OrderingGroup = nullptr;
  static QString m_OpenDialogLastFilePath;

  QAction* m_ShowFileAction = nullptr;
  QString m_CurrentText = "";
  bool m_DidCausePreflight = false;

  /**
   * @brief connectSignalsSlots
   */
  void connectSignalsSlots();

  /**
   * @brief showFileInFileSystem
   * @param filePath
   */
  void showFileInFileSystem(const QString& filePath);

public:
  TileListWidget(const TileListWidget&) = delete;            // Copy Constructor Not Implemented
  TileListWidget(TileListWidget&&) = delete;                 // Move Constructor Not Implemented
  TileListWidget& operator=(const TileListWidget&) = delete; // Copy Assignment Not Implemented
  TileListWidget& operator=(TileListWidget&&) = delete;      // Move Assignment Not Implemented
};
