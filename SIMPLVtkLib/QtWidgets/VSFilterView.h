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

#include <QtWidgets/QTreeView>

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
* @class VSFilterView VSFilterView.h SIMPLVtkLib/QtWidgets/VSFilterView.h
* @brief This class is used for viewing a VSController's VSFilterModel in a 
* QTreeView and handling user interaction with that model
*/
class SIMPLVtkLib_EXPORT VSFilterView : public QTreeView
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  */
  VSFilterView(QWidget* parent = nullptr);

  /**
  * @brief Deconstructor
  */
  virtual ~VSFilterView() = default;

  /**
  * @brief Sets the VSController to use and interact with through the tree view
  * @param controller
  */
  void setController(VSController* controller);

signals:
  void filterClicked(VSAbstractFilter* filter);
  void deleteFilterRequested(VSAbstractFilter* filter);
  void reloadFilterRequested(VSAbstractDataFilter* filter);
  void reloadFileFilterRequested(VSFileNameFilter* filter);

public slots:
  /**
  * @brief Changes the view controller used for accessing VSFilterViewSettings
  * @param view
  */
  void setViewWidget(VSAbstractViewWidget* viewWidget);

  /**
  * @brief Handle changes in filter visibility for a given view
  * @param filter
  * @param visible
  */
  void setFilterVisibility(VSFilterViewSettings* filter, bool visible);

  /**
  * @brief Handles changing the active filter through other widgets or controls
  * @param filter
  * @param widget
  */
  void setActiveFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* widget);

protected slots:
  /**
  * @brief Handles mouse clicks on an item and emits a signal if the item is a
  * VSAbstractFilter instance
  * @param index
  */
  void itemClicked(const QModelIndex& index);

  /**
  * @brief Handles changes in the current item
  * @param current
  * @param previous
  */
  void setCurrentItem(const QModelIndex& current, const QModelIndex& previous);

  /**
  * @brief Handles a new filter being inserted and automatically expands it
  * @param filter
  */
  void insertFilter(VSAbstractFilter* filter, bool currentFilter);

  /**
   * @brief requestContextMenu
   * @param pos
   */
  void requestContextMenu(const QPoint& pos);

protected:
  /**
  * @brief Connect Qt signals and slots
  */
  void connectSlots();

  /**
  * @brief Performs any initial setup work for the GUI
  */
  void setupGui();

  /**
  * @brief Returns the visual filter at the given index or nullptr if no 
  * visual filter exists at that location
  * @param index
  * @return
  */
  VSAbstractFilter* getFilterFromIndex(const QModelIndex& index);

private:
  VSController* m_Controller = nullptr;
  VSAbstractViewWidget* m_ViewWidget = nullptr;
};
