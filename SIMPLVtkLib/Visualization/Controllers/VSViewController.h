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

#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class VSController;

/**
* @class VSViewController VSViewController.h 
* SIMPLVtkLib/Visualization/Controllers/VSViewController.h
* @brief This class handles visibility settings for each visualization widget and 
* connects it to the appropriate VSController through Qt's signals and slots.
*/
class SIMPLVtkLib_EXPORT VSViewController : public QObject
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param controller
  */
  VSViewController(VSController* controller);
  
  /**
  * @brief Copy constructor
  * @param copy
  */
  VSViewController(const VSViewController& copy);

  /**
  * @brief Returns the VSFilterViewSettings for the given visual filter
  * @param filter
  * @return
  */
  VSFilterViewSettings* getViewSettings(VSAbstractFilter* filter);

  /**
  * @brief Returns a vector of VSFilterViewSettings handled by this controller
  * @return
  */
  std::vector<VSFilterViewSettings*> getAllViewSettings();

  /**
  * @brief Performs a deep copy based on another view controller
  * @param other
  */
  void copy(VSViewController* other);

  /**
  * @brief Returns the VSController used
  * @return
  */
  VSController* getController();

signals:
  void filterAdded(VSAbstractFilter* filter);
  void visibilityChanged(VSFilterViewSettings*, bool);
  void activeArrayIndexChanged(VSFilterViewSettings*, int);
  void activeComponentIndexChanged(VSFilterViewSettings*, int);
  void mapColorsChanged(VSFilterViewSettings*, bool);
  void alphaChanged(VSFilterViewSettings*, double);
  void showScalarBarChanged(VSFilterViewSettings*, bool);
  void requiresRender();

public slots:
  /**
  * @brief Adds a new visual filter to the VSViewController
  * @param filter
  */
  void addFilter(VSAbstractFilter* filter);

  /**
  * @brief Removes the given visual filter from the VSViewController
  * @param filter
  */
  void removeFilter(VSAbstractFilter* filter);

  /**
  * @brief Sets this VSViewController as the active one for the VSController
  */
  void markActive();

protected:
  /**
  * @brief Applies a fresh set of VSFilterViewSettings
  */
  void createFilterSettings();

  /**
  * @brief Applies a copy of another VSViewController's VSFilterViewSettings
  * @param copy
  */
  void copyFilterSettings(const VSViewController& copy);

  /**
  * @brief Returns the index for the given VSFilterViewSettings
  * @param settings
  * @return
  */
  int getViewSettingsIndex(VSFilterViewSettings* settings);

  /**
  * @brief Connect Qt signals and slots from a given VSController
  * @param controller
  */
  void connectController(VSController* controller);

private:
  VSController* m_VSController;
  std::vector<VSFilterViewSettings*> m_FilterViewSettings;
};
