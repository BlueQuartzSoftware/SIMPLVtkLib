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

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSViewController.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
* @class VSController VSController.h SIMPLVtkLib/Visualization/Controllers/VSController.h
* @brief This class handles the top-level controls for SIMPLVtkLib from importing data 
* to updating VisualFilters for rendering data across multiple views.  The VisualFilter 
* tree is stored within the class so that it can be readily accessed or modified from 
* inside or outside the library.
*/
class SIMPLVtkLib_EXPORT VSController : public QObject
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  */
  VSController(QObject* parent = nullptr);

  /**
  * @brief Deconstructor
  */
  virtual ~VSController();

  /**
  * @brief Import data from a DataContainerArray and add any relevant DataContainers
  * as top-level VisualFilters
  * @param dca
  */
  void importData(DataContainerArray::Pointer dca);

  /**
  * @brief Import data from a DataContainer and add a top-level VisualFilter if there
  * is relevant data for visualization
  * @param dc
  */
  void importData(DataContainer::Pointer dc);

  /**
  * @brief Returns the active VSViewController
  * @return
  */
  VSViewController* getActiveViewController();

  /**
  * @brief Returns a vector of top-level data filters
  * @return
  */
  QVector<VSAbstractFilter*> getBaseFilters();

  /**
  * @brief Returns a vector of all visual filters
  * @return
  */
  QVector<VSAbstractFilter*> getAllFilters();

  /**
  * @brief Return the filter model used
  * @return
  */
  VSFilterModel* getFilterModel();

public slots:
  /**
  * @brief Sets the active VSViewController
  * @param activeView
  */
  void setActiveViewController(VSViewController* activeView);

signals:
  void activeViewChanged(VSViewController* activeView);
  void filterAdded(VSAbstractFilter*);
  void filterRemoved(VSAbstractFilter*);
  void dataImported();

private:
  VSViewController* m_ActiveViewController = nullptr;
  VSFilterModel* m_FilterModel;
};
