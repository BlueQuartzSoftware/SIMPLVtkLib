/* ============================================================================
* Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include <QtCore/QSemaphore>
#include <QtCore/QFutureWatcher>

#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSFilterView.h"
#include "SIMPLVtkLib/QtWidgets/VSInfoWidget.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
* @class VSMainWidgetBase VSMainWidgetBase.h SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h
* @brief This class is the superclass for VSMainWidget and contains the base methods 
* for accessing and viewing information regarding visual filters.
*/
class SIMPLVtkLib_EXPORT VSMainWidgetBase : public QWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  */
  VSMainWidgetBase(QWidget* parent = nullptr);

  /**
  * @brief Returns the VSController for the widget
  */
  VSController* getController();

  /**
  * @brief Returns the active view widget
  * @return
  */
  VSAbstractViewWidget* getActiveViewWidget();

  /**
  * @brief Returns a vector of all view widgets contained in the main widget
  * @return
  */
  QVector<VSAbstractViewWidget*> getAllViewWidgets();

  /**
  * @brief Returns the VSFilterView used by the main widget. Returns nullptr if none is used
  * @return
  */
  VSFilterView* getFilterView();

  /**
  * @brief Sets the VSFilterView to use, apply the controller's filter model 
  * and connect to any signals it might have.
  * @param view
  */
  void setFilterView(VSFilterView* view);

  /**
  * @brief Returns the VSInfoWidget used by the main widget. Returns nullptr if none is used
  * @return
  */
  VSInfoWidget* getInfoWidget();

  /**
  * @brief Sets the VSInfoWidget to use for displaying information about the active filter
  * @param infoWidget
  */
  void setInfoWidget(VSInfoWidget* infoWidget);

  /**
   * @brief importFiles
   * @param filePaths
   */
  void importFiles(QStringList filePaths);

public slots:
  /**
  * @brief Create a clip filter and set the given filter as its parent.  If no filter is provided,
  * the current filter is used instead.
  * @param parent
  */
  void createClipFilter(VSAbstractFilter* parent = nullptr);

  /**
  * @brief Create a crop filter and set the given filter as its parent.  If no filter is provided,
  * the current filter is used instead.
  * @param parent
  */
  void createCropFilter(VSAbstractFilter* parent = nullptr);

  /**
  * @brief Create a slice filter and set the given filter as its parent.  If no filter is provided,
  * the current filter is used instead.
  * @param parent
  */
  void createSliceFilter(VSAbstractFilter* parent = nullptr);

  /**
  * @brief Create a mask filter and set the given filter as its parent.  If no filter is provided,
  * the current filter is used instead.
  * @param parent
  */
  void createMaskFilter(VSAbstractFilter* parent = nullptr);

  /**
  * @brief Create a threshold filter and set the given filter as its parent.  If no filter is provided,
  * the current filter is used instead.
  * @param parent
  */
  void createThresholdFilter(VSAbstractFilter* parent = nullptr);

  /**
  * @brief Create a text filter and ste the given filter as its parent.  If no filter is provided,
  * the current filter is used instead.
  * @param parent
  */
  void createTextFilter(VSAbstractFilter* parent = nullptr);

  /**
  * @brief Renders the active view widget
  */
  void renderActiveView();

  /**
  * @brief Renders all view widgets contained in the main widget
  */
  void renderAllViews();

signals:
  void changedActiveView(VSAbstractViewWidget* viewWidget);
  void changedActiveFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* filterWidget);

  void proxyFromFilePathGenerated(DataContainerArrayProxy proxy, const QString &filePath);

protected:
  /**
  * @brief Connect signals and slots for the VSController
  */
  virtual void connectSlots();

  /**
  * @brief Returns the current visual filter
  * @return
  */
  VSAbstractFilter* getCurrentFilter();

  /**
  * @brief Performs handling adding a filter and toggling parent visibility
  * @param filter
  * @param parent
  */
  void finishAddingFilter(VSAbstractFilter* filter, VSAbstractFilter* parent);

protected slots:
  /**
  * @brief Notifies change in the active VSAbstractViewWidget
  * @param viewWidget
  */
  virtual void setActiveView(VSAbstractViewWidget* viewWidget);

  /**
  * @brief Resets the active view widget to nullptr
  */
  virtual void activeViewClosed();

  /**
  * @brief Changes the active visual filter
  * @param filter
  */
  virtual void setCurrentFilter(VSAbstractFilter* filter);

  /**
   * @brief deleteFilter
   * @param filter
   */
  virtual void deleteFilter(VSAbstractFilter* filter);

  /**
   * @brief reloadFilter
   * @param filter
   */
  virtual void reloadFilter(VSAbstractDataFilter* filter);

  /**
  * @brief Connects a new VSAbstractViewWidget to the main widget
  */
  virtual void connectViewWidget(VSAbstractViewWidget* viewWidget);

  /**
  * @brief Handles changes in the active filter view's visibility
  * @param viewSettings
  * @param visible
  */
  virtual void setFilterVisibility(VSFilterViewSettings* viewSettings, bool visible);

  /**
   * @brief
   * @param filter
   */
  void filterAdded(VSAbstractFilter* filter, bool currentFilter);

  /**
   * @brief Removes filter from the FilterToFilterWidget map
   * @param filter
   */
  void filterRemoved(VSAbstractFilter* filter);

  /**
   * @brief launchSIMPLSelectionDialog
   * @param proxy
   */
  void launchSIMPLSelectionDialog(DataContainerArrayProxy proxy, const QString &filePath);

  /**
   * @brief generateError
   * @param title
   * @param msg
   * @param code
   */
  void generateError(const QString &title, const QString &msg, const int &code);

private:
  VSController* m_Controller;
  VSAbstractFilter* m_CurrentFilter = nullptr;
  VSAbstractViewWidget* m_ActiveViewWidget = nullptr;
  VSFilterView* m_FilterView = nullptr;
  VSInfoWidget* m_InfoWidget = nullptr;

  QMap<VSAbstractFilter*,VSAbstractFilterWidget*>  m_FilterToFilterWidgetMap;

  /**
   * @brief openDREAM3DFile
   * @param filePath
   * @param instance
   */
  void openDREAM3DFile(const QString &filePath);
};
