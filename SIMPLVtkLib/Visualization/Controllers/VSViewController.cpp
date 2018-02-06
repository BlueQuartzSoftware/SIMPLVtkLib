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

#include "VSViewController.h"

#include <algorithm>

#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewController::VSViewController(VSController* controller)
  : QObject(controller)
  , m_VSController(controller)
{
  connectController(controller);
  createFilterSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewController::VSViewController(const VSViewController& copy)
  : QObject(copy.m_VSController)
  , m_VSController(copy.m_VSController)
{
  connectController(copy.m_VSController);
  copyFilterSettings(copy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewController::connectController(VSController* controller)
{
  if(m_VSController)
  {
    disconnect(controller, SIGNAL(filterAdded(VSAbstractFilter*)), this, SLOT(addFilter(VSAbstractFilter*)));
    disconnect(controller, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(removeFilter(VSAbstractFilter*)));
  }

  m_VSController = controller;
  connect(controller, SIGNAL(filterAdded(VSAbstractFilter*)), this, SLOT(addFilter(VSAbstractFilter*)));
  connect(controller, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(removeFilter(VSAbstractFilter*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController* VSViewController::getController()
{
  return m_VSController;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewController::createFilterSettings()
{
  // Get all visual filters from VSController
  QVector<VSAbstractFilter*> visualFilters = m_VSController->getAllFilters();
  
  // Create a VSFilterViewSetting for each filter
  int count = visualFilters.size();
  m_FilterViewSettings.resize(count);
  for(int i = 0; i < count; i++)
  {
    m_FilterViewSettings[i] = new VSFilterViewSettings(visualFilters[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewController::copyFilterSettings(const VSViewController& copy)
{
  std::vector<VSFilterViewSettings*> copySettings = copy.m_FilterViewSettings;

  size_t count = copySettings.size();
  m_FilterViewSettings.resize(count);

  for(size_t i = 0; i < count; i++)
  {
    VSFilterViewSettings* viewSettings = new VSFilterViewSettings(*(copySettings[i]));
    m_FilterViewSettings[i] = viewSettings;

    connect(viewSettings, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)),
      this, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)));
    connect(viewSettings, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)),
      this, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)));
    connect(viewSettings, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)),
      this, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)));
    connect(viewSettings, SIGNAL(mapColorsChanged(VSFilterViewSettings*, bool)),
      this, SIGNAL(mapColorsChanged(VSFilterViewSettings*, bool)));
    connect(viewSettings, SIGNAL(alphaChanged(VSFilterViewSettings*, double)),
      this, SIGNAL(alphaChanged(VSFilterViewSettings*, double)));
    connect(viewSettings, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)),
      this, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)));
    connect(viewSettings, SIGNAL(requiresRender()),
      this, SIGNAL(requiresRender()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSViewController::getViewSettings(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return nullptr;
  }

  size_t count = m_FilterViewSettings.size();
  for(size_t i = 0; i < count; i++)
  {
    if(m_FilterViewSettings[i]->getFilter() == filter)
    {
      return m_FilterViewSettings[i];
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<VSFilterViewSettings*> VSViewController::getAllViewSettings()
{
  return m_FilterViewSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSViewController::getViewSettingsIndex(VSFilterViewSettings* settings)
{
  auto iter = std::find(m_FilterViewSettings.begin(), m_FilterViewSettings.end(), settings);
  if(iter == m_FilterViewSettings.end())
  {
    return -1;
  }

  return std::distance(m_FilterViewSettings.begin(), iter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewController::addFilter(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  VSFilterViewSettings* viewSettings = new VSFilterViewSettings(filter);

  connect(viewSettings, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)), 
    this, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)));
  connect(viewSettings, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)),
    this, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)));
  connect(viewSettings, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)),
    this, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)));
  connect(viewSettings, SIGNAL(mapColorsChanged(VSFilterViewSettings*, bool)),
    this, SIGNAL(mapColorsChanged(VSFilterViewSettings*, bool)));
  connect(viewSettings, SIGNAL(alphaChanged(VSFilterViewSettings*, double)),
    this, SIGNAL(alphaChanged(VSFilterViewSettings*, double)));
  connect(viewSettings, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)),
    this, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)));
  connect(viewSettings, SIGNAL(requiresRender()),
    this, SIGNAL(requiresRender()));

  m_FilterViewSettings.push_back(viewSettings);

  if(filter->getParentFilter())
  {
    VSFilterViewSettings* parentSettings = getViewSettings(filter->getParentFilter());
    if(parentSettings)
    {
      viewSettings->copySettings(parentSettings);
    }
  }

  emit filterAdded(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewController::removeFilter(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  VSFilterViewSettings* viewSettings = getViewSettings(filter);
  if(viewSettings)
  {
    // Stop rendering the filter
    viewSettings->setVisible(false);
    viewSettings->setScalarBarVisible(false);
    
    // Remove the FilterViewSetting from the controller
    int index = getViewSettingsIndex(viewSettings);
    m_FilterViewSettings.erase(m_FilterViewSettings.begin() + index);

    // Delete the FilterViewSetting
    viewSettings->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewController::markActive()
{
  m_VSController->setActiveViewController(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewController::copy(VSViewController* other)
{
  m_VSController = other->m_VSController;
  connectController(other->m_VSController);

  if(other)
  {
    copyFilterSettings(*other);
  }
}
