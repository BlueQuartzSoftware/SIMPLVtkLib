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

#include "ui_FijiListWidget.h"

typedef struct
{
  QString FijiFilePath;

  void writeJson(QJsonObject& json)
  {
    json["RobometFilePath"] = FijiFilePath;
  }

  bool readJson(QJsonObject& json)
  {
    if(json["Ordering"].isDouble() && json["FilePrefix"].isString() && json["FileSuffix"].isString() && json["FileExtension"].isString())
    {
      FijiFilePath = json["FijiFilePath"].toString();
      return true;
    }
    return false;
  }
} FijiListInfo_t;

Q_DECLARE_METATYPE(FijiListInfo_t)

/**
 * @class FijiListWidget FijiListWidget.h SIMPLVtkLib/Wizards/ImportMontage/UI_Files/FijiListWidget.h
 * @brief This class represents the User Interface for the Fiji file import module
 * of the SIMPLView program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of Fiji data files into an HDF5 file
 * for better data management and archiving.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class SIMPLVtkLib_EXPORT FijiListWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  FijiListWidget(QWidget* parent = nullptr);

  ~FijiListWidget() override;

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
   * @brief getFijiListInfo
   * @return
   */
  FijiListInfo_t getFijiListInfo();

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
   * @brief readFijiConfigFile
   * @return
   */
  QStringList readFijiConfigFile();

  /**
   * @brief generateExampleInputFile
   * @param filenameList
   */
  void generateExampleInputFile(QStringList filenameList);

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
   * @brief numberOfRowsChanged
   * @param numberOfRows
   */
  void numberOfRowsChanged(size_t numberOfRows);

  /**
   * @brief numberOfColumnsChanged
   * @param numOfCols
   */
  void numberOfColumnsChanged(size_t numOfCols);

private:
  QSharedPointer<Ui::FijiListWidget> m_Ui;

  QList<QWidget*> m_WidgetList;
  static QString m_OpenDialogLastFilePath;

  QAction* m_ShowFileAction = nullptr;
  QString m_CurrentText = "";
  bool m_DidCausePreflight = false;

  const int k_SlicePadding = 6;

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
   * @param filenameList
   * @param hasMissingFiles
   * @param inputPath
   * @return
   */
  QVector<QString> generateFileList(QStringList filenameList, bool& hasMissingFiles, const QString& inputPath);

public:
  FijiListWidget(const FijiListWidget&) = delete;            // Copy Constructor Not Implemented
  FijiListWidget(FijiListWidget&&) = delete;                 // Move Constructor Not Implemented
  FijiListWidget& operator=(const FijiListWidget&) = delete; // Copy Assignment Not Implemented
  FijiListWidget& operator=(FijiListWidget&&) = delete;      // Move Assignment Not Implemented
};
