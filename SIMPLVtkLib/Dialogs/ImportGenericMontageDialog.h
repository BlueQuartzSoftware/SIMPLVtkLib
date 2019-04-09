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

#include "SIMPLVtkLib/Dialogs/AbstractImportMontageDialog.h"

#include "SIMPLVtkLib/Dialogs/Utilities/MontageSettings.h"

#include "ui_ImportGenericMontageDialog.h"

class ImportGenericMontageDialog : public AbstractImportMontageDialog
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(ImportGenericMontageDialog)

  ~ImportGenericMontageDialog() override;

  /**
   * @brief New
   * @param parent
   * @return
   */
  static Pointer New(QWidget* parent = nullptr);

  SIMPL_INSTANCE_PROPERTY(FileListInfo_t, FileListInfo)
  Q_PROPERTY(FileListInfo_t FileListInfo READ getFileListInfo WRITE setFileListInfo)

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  virtual void setupGui();

  /**
   * @brief checkComplete
   * @return
   */
  void checkComplete() const override;

  /**
   * @brief getMontageName
   * @return
   */
  QString getMontageName();

  /**
   * @brief getMontageDimensions
   * @return
   */
  std::tuple<int, int> getMontageDimensions();

  /**
   * @brief getTileOverlap
   * @return
   */
  int getTileOverlap();

  /**
   * @brief getOutputFileName
   * @return
   */
  QString getOutputFileName();

  /**
   * @brief getMontageType
   * @return
   */
  MontageSettings::MontageType getMontageType();

  /**
   * @brief getMontageOrder
   * @return
   */
  MontageSettings::MontageOrder getMontageOrder();

  /**
   * @brief getOverrideSpacing
   * @return
   */
  bool getOverrideSpacing();

  /**
   * @brief getSpacing
   * @return
   */
  std::tuple<double, double, double> getSpacing();

  /**
   * @brief getOverrideOrigin
   * @return
   */
  bool getOverrideOrigin();

  /**
   * @brief getOrigin
   * @return
   */
  std::tuple<double, double, double> getOrigin();

protected:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  ImportGenericMontageDialog(QWidget* parent = nullptr);

protected slots:
  void tileListWidgetChanged();

private slots:
  void updateOrderChoices(MontageSettings::MontageType collectionType);
  void changeOrigin_stateChanged(int state);
  void changeSpacing_stateChanged(int state);

private:
  QSharedPointer<Ui::ImportGenericMontageDialog> m_Ui;

  QString m_OpenDialogLastDirectory;

  /**
   * @brief connectSignalsSlots
   */
  void connectSignalsSlots();

public:
  ImportGenericMontageDialog(const ImportGenericMontageDialog&) = delete;            // Copy Constructor Not Implemented
  ImportGenericMontageDialog(ImportGenericMontageDialog&&) = delete;                 // Move Constructor Not Implemented
  ImportGenericMontageDialog& operator=(const ImportGenericMontageDialog&) = delete; // Copy Assignment Not Implemented
  ImportGenericMontageDialog& operator=(ImportGenericMontageDialog&&) = delete;      // Move Assignment Not Implemented
};
