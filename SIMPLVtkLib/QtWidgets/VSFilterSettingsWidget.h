/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include <QtWidgets/QFrame>

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilterValues.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include "ui_VSFilterSettingsWidget.h"

/**
 * @class VSFilterSettingsWidget VSFilterSettingsWidget.h
 * SIMPLVtkLib/QtWidgets/VSFilterSettingsWidget.h
 * @brief This class handles the filter tree and filter widget for VSMainWidgetBase
 */
class SIMPLVtkLib_EXPORT VSFilterSettingsWidget : public QFrame
{
  Q_OBJECT

public:
  VSFilterSettingsWidget(QWidget* parent = nullptr);
  virtual ~VSFilterSettingsWidget() = default;

  /**
   * @brief Returns the current filter
   * @return
   */
  VSAbstractFilter* getCurrentFilter() const;

signals:
  void filterDeleted(VSAbstractFilter* filter);

public slots:
  /**
   * @brief Changes the filter being displayed
   * @param filters
   */
  void setFilters(VSAbstractFilter::FilterListType filters);

  /**
   * @brief Changes the VSViewController used for fetching VSFilterViewSettings
   * @param viewController
   */
  void setViewWidget(VSAbstractViewWidget* viewWidget);

  /**
   * @brief Applies the active filter with the current widget values.
   */
  void applyFilter();

  /**
   * @brief Resets current widget to the last applied filter values
   */
  void resetFilter();

  /**
   * @brief Deletes the active filter
   */
  void deleteFilter();

protected slots:
  /**
   * @brief Enables the Apply and Reset buttons when the current filter widget has changes waiting
   */
  void changesWaiting();

protected:
  /**
   * @brief Performs initial setup for the GUI
   */
  void setupGui();

private:
  QSharedPointer<Ui::VSFilterSettingsWidget> m_Ui;
  VSAbstractFilter::FilterListType m_Filters;
  VSAbstractViewWidget* m_ViewWidget = nullptr;
  QWidget* m_FilterWidget = nullptr;
};