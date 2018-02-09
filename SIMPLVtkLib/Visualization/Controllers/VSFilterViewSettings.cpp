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

#include "VSFilterViewSettings.h"

#include <vtkAbstractArray.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::VSFilterViewSettings(VSAbstractFilter* filter)
  : QObject(nullptr)
  , m_ShowFilter(true)
  , m_ShowScalarBar(true)
{
  connectFilter(filter);
  setupActors();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::VSFilterViewSettings(const VSFilterViewSettings& copy)
  : QObject(nullptr)
  , m_ShowFilter(copy.m_ShowFilter)
  , m_ActiveArray(copy.m_ActiveArray)
  , m_ActiveComponent(copy.m_ActiveComponent)
  , m_MapColors(copy.m_MapColors)
  , m_ShowScalarBar(copy.m_ShowScalarBar)
  , m_Alpha(copy.m_Alpha)
{
  connectFilter(copy.m_Filter);
  setupActors();
  m_LookupTable->copy(*(copy.m_LookupTable));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::~VSFilterViewSettings()
{
  //if(m_LookupTable)
  //{
  //  delete m_LookupTable;
  //}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterViewSettings::getFilter()
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::getVisible()
{
  return m_ShowFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getActiveArrayIndex()
{
  return m_ActiveArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getActiveComponentIndex()
{
  return m_ActiveComponent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getNumberOfComponents(int arrayIndex)
{
  vtkCellData* cellData = m_Filter->getOutput()->GetCellData();

  vtkAbstractArray* abstractArray = cellData->GetAbstractArray(arrayIndex);
  if (abstractArray != nullptr)
  {
    return cellData->GetAbstractArray(arrayIndex)->GetNumberOfComponents();
  }
  else
  {
    return 0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getNumberOfComponents(QString arrayName)
{
  const char* name = arrayName.toLatin1();

  vtkCellData* cellData = m_Filter->getOutput()->GetCellData();
  return cellData->GetAbstractArray(name)->GetNumberOfComponents();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::getMapColors()
{
  return m_MapColors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSFilterViewSettings::getAlpha()
{
  return m_Alpha;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isScalarBarVisible()
{
  return m_ShowScalarBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkActor) VSFilterViewSettings::getActor()
{
  return m_Actor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkScalarBarWidget) VSFilterViewSettings::getScalarBarWidget()
{
  return m_ScalarBarWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::show()
{
  setVisible(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::hide()
{
  setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setVisible(bool visible)
{
  m_ShowFilter = visible;

  emit visibilityChanged(this, m_ShowFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkDataArray* VSFilterViewSettings::getArrayAtIndex(int index)
{
  vtkDataSet* dataSet = m_Mapper->GetInput();
  if(dataSet && dataSet->GetCellData())
  {
    return dataSet->GetCellData()->GetArray(index);
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setActiveArrayIndex(int index)
{
  VTK_PTR(vtkDataArray) dataArray = getArrayAtIndex(index);
  if(nullptr == dataArray)
  {
    return;
  }

  m_ActiveArray = index;

  emit activeArrayIndexChanged(this, m_ActiveArray);
  setActiveComponentIndex(-1); 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setActiveComponentIndex(int index)
{
  m_ActiveComponent = index;

  VTK_PTR(vtkScalarsToColors) lookupTable = m_Mapper->GetLookupTable();
  VTK_PTR(vtkDataArray) dataArray = getArrayAtIndex(m_ActiveArray);
  if(nullptr == dataArray)
  {
    return;
  }
  
  // Clamp index with lower bound
  if(index < -1)
  {
    index = -1;
  }

  int numComponents = dataArray->GetNumberOfComponents();
  m_Mapper->ColorByArrayComponent(m_ActiveArray, index);
  m_Mapper->SetScalarModeToUseCellFieldData();
  updateColorMode();

  if(numComponents == 1)
  {
    double* range = dataArray->GetRange();
    m_LookupTable->setRange(range);
    m_ScalarBarActor->SetTitle(dataArray->GetName());
  }
  else if(index == -1)
  {
    double* range = dataArray->GetRange(-1);
    QString dataArrayName = QString(dataArray->GetName());
    QString componentName = dataArrayName + " Magnitude";

    //lookupTable->SetVectorModeToMagnitude();
  
    m_LookupTable->setRange(range);
    m_ScalarBarActor->SetTitle(qPrintable(componentName));
  }
  else if(index < numComponents)
  {
    double* range = dataArray->GetRange(index);
    m_LookupTable->setRange(range);
    m_ScalarBarActor->SetTitle(dataArray->GetComponentName(index));
  }

  m_Mapper->Update();
  
  emit activeComponentIndexChanged(this, m_ActiveComponent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateColorMode()
{
  if(nullptr == m_Mapper)
  {
    return;
  }

  if(m_MapColors)
  {
    m_Mapper->SetColorModeToMapScalars();
  }
  else
  {
    m_Mapper->SetColorModeToDirectScalars();
  }

  m_Mapper->Update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setMapColors(bool mapColors)
{
  m_MapColors = mapColors;

  updateColorMode();
  emit mapColorsChanged(this, m_MapColors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setAlpha(double alpha)
{
  if(alpha < 0.0)
  {
    alpha = 0.0;
  }
  else if(alpha > 1.0)
  {
    alpha = 1.0;
  }

  m_Alpha = alpha;

  vtkProperty* property = m_Actor->GetProperty();
  property->SetOpacity(m_Alpha);
  m_Actor->SetProperty(property);

  emit alphaChanged(this, m_Alpha);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::invertScalarBar()
{
  m_LookupTable->invert();
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::loadPresetColors(const QJsonObject& colors)
{
  m_LookupTable->parseRgbJson(colors);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setScalarBarVisible(bool visible)
{
  m_ShowScalarBar = visible;

  emit showScalarBarChanged(this, m_ShowScalarBar);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupActors()
{
  m_Mapper = VTK_PTR(vtkDataSetMapper)::New();
  m_Mapper->SetInputConnection(m_Filter->getOutputPort());

  m_Actor = VTK_PTR(vtkActor)::New();
  m_Actor->SetMapper(m_Mapper);

  m_LookupTable = new VSLookupTableController();
  m_Mapper->SetLookupTable(m_LookupTable->getColorTransferFunction());

  m_ScalarBarActor = VTK_PTR(vtkScalarBarActor)::New();
  m_ScalarBarActor->SetLookupTable(m_Mapper->GetLookupTable());
  m_ScalarBarWidget = VTK_PTR(vtkScalarBarWidget)::New();
  m_ScalarBarWidget->SetScalarBarActor(m_ScalarBarActor);

  // Scalar Bar Title
  vtkTextProperty* titleProperty = m_ScalarBarActor->GetTitleTextProperty();
  titleProperty->SetJustificationToCentered();
  titleProperty->SetFontSize(titleProperty->GetFontSize() * 1.5);

  // Introduced in 7.1.0rc1, prevents resizing title to fill width
#if VTK_MAJOR_VERSION > 7 || (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1)
  m_ScalarBarActor->UnconstrainedFontSizeOn();
#endif

  m_ScalarBarActor->SetTitleRatio(0.75);

  // Set Mapper to use the active array and component
  int currentComponent = m_ActiveComponent;
  setActiveArrayIndex(m_ActiveArray);
  setActiveComponentIndex(currentComponent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateInputPort(VSAbstractFilter* filter)
{
  m_Mapper->SetInputConnection(filter->getOutputPort());
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::connectFilter(VSAbstractFilter* filter)
{
  if(m_Filter)
  {
    disconnect(m_Filter, SIGNAL(updatedOutputPort(VSAbstractFilter*)), this, SLOT(updateInputPort(VSAbstractFilter*)));
  }

  m_Filter = filter;
  if(filter)
  {
    connect(filter, SIGNAL(updatedOutputPort(VSAbstractFilter*)), this, SLOT(updateInputPort(VSAbstractFilter*)));

    if(filter->getArrayNames().size() < 1)
    {
      setScalarBarVisible(false);
      setMapColors(false);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::copySettings(VSFilterViewSettings* copy)
{
  vtkRenderWindowInteractor* iren = copy->m_ScalarBarWidget->GetInteractor();
  m_ScalarBarWidget->SetInteractor(iren);

  setVisible(copy->m_ShowFilter);
  setActiveArrayIndex(copy->m_ActiveArray);
  setActiveComponentIndex(copy->m_ActiveComponent);
  setMapColors(copy->m_MapColors);
  setScalarBarVisible(copy->m_ShowScalarBar);
  setAlpha(copy->m_Alpha);

  m_LookupTable->copy(*(copy->m_LookupTable));

  emit requiresRender();
}
