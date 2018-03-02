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
  setRepresentation(Representation::Default);
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

  if(copy.m_LookupTable)
  {
    m_LookupTable->copy(*(copy.m_LookupTable));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::~VSFilterViewSettings()
{
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
bool VSFilterViewSettings::isValid()
{
  bool valid = m_Mapper && m_Actor && m_ScalarBarWidget;
  return valid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isVisible()
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
  if(nullptr == m_Filter->getOutput())
  {
    return -1;
  }

  vtkCellData* cellData = m_Filter->getOutput()->GetCellData();
  vtkAbstractArray* array = cellData->GetAbstractArray(arrayIndex);
  if(array)
  {
    return array->GetNumberOfComponents();
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getNumberOfComponents(QString arrayName)
{
  if(nullptr == m_Filter->getOutput())
  {
    return -1;
  }

  const char* name = arrayName.toLatin1();

  vtkCellData* cellData = m_Filter->getOutput()->GetCellData();
  vtkAbstractArray* array = cellData->GetAbstractArray(name);
  if(array)
  {
    return array->GetNumberOfComponents();
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::CheckState VSFilterViewSettings::getMapColors()
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
  if(false == isValid())
  {
    return;
  }

  m_ShowFilter = visible;

  emit visibilityChanged(this, m_ShowFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkDataArray* VSFilterViewSettings::getArrayAtIndex(int index)
{
  if(false == isValid())
  {
    return nullptr;
  }

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
  if(false == isValid())
  {
    return;
  }

  // Draw a solid color if index is -1
  if(index == -1)
  {
    m_Mapper->SelectColorArray(-1);
    m_ActiveArray = -1;

    emit activeArrayIndexChanged(this, m_ActiveArray);
    emit requiresRender();
    return;
  }

  VTK_PTR(vtkDataArray) dataArray = getArrayAtIndex(index);
  if(nullptr == dataArray)
  {
    return;
  }

  m_ActiveArray = index;

  emit activeArrayIndexChanged(this, m_ActiveArray);
  setActiveComponentIndex(-1);

  if(isColorArray(dataArray) && m_MapColors == Qt::Checked)
  {
    setMapColors(Qt::CheckState::PartiallyChecked);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setActiveComponentIndex(int index)
{
  if(false == isValid())
  {
    return;
  }

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
vtkDataArray* VSFilterViewSettings::getDataArray()
{
  if(nullptr == m_Filter->getOutput())
  {
    return nullptr;
  }

  return getArrayAtIndex(m_ActiveArray);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isColorArray(vtkDataArray* dataArray)
{
  if(nullptr == dataArray)
  {
    return false;
  }

  if(dataArray->GetNumberOfComponents() == 3)
  {
    QString dataType = dataArray->GetDataTypeAsString();
    if(dataType.compare("unsigned char") == 0)
    {
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateColorMode()
{
  if(false == isValid())
  {
    return;
  }

  vtkDataArray* dataArray = getDataArray();
  bool unmapColorArray = isColorArray(dataArray) && (m_ActiveComponent == -1);

  if(m_MapColors && !unmapColorArray)
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
void VSFilterViewSettings::setMapColors(Qt::CheckState mapColorState)
{
  if(false == isValid())
  {
    return;
  }

  m_MapColors = mapColorState;

  updateColorMode();
  emit mapColorsChanged(this, m_MapColors);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setAlpha(double alpha)
{
  if(false == isValid())
  {
    return;
  }

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
  if(false == isValid())
  {
    return;
  }

  m_LookupTable->invert();
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::loadPresetColors(const QJsonObject& colors)
{
  if(false == isValid())
  {
    return;
  }

  m_LookupTable->parseRgbJson(colors);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setScalarBarVisible(bool visible)
{
  if(false == isValid())
  {
    return;
  }

  m_ShowScalarBar = visible;

  emit showScalarBarChanged(this, m_ShowScalarBar);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupActors()
{
  VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();
  if(nullptr == outputData)
  {
    m_ShowFilter = false;
    return;
  }

  m_Mapper = VTK_PTR(vtkDataSetMapper)::New();
  m_Mapper->SetInputConnection(m_Filter->getTransformedOutputPort());

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
  if(outputData->GetCellData()->GetNumberOfArrays() > 0)
  {
    int currentComponent = m_ActiveComponent;
    setActiveArrayIndex(m_ActiveArray);
    setActiveComponentIndex(m_ActiveComponent);
  }
  else
  {
    setMapColors(Qt::Unchecked);
    setScalarBarVisible(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateInputPort(VSAbstractFilter* filter)
{
  if(false == isValid())
  {
    return;
  }

  m_Mapper->SetInputConnection(filter->getTransformedOutputPort());
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
    disconnect(filter, SIGNAL(transformChanged()), this, SIGNAL(requiresRender()));
  }

  m_Filter = filter;
  if(filter)
  {
    connect(filter, SIGNAL(updatedOutputPort(VSAbstractFilter*)), this, SLOT(updateInputPort(VSAbstractFilter*)));
    connect(filter, SIGNAL(transformChanged()), this, SIGNAL(requiresRender()));

    if(filter->getArrayNames().size() < 1)
    {
      setScalarBarVisible(false);
      setMapColors(Qt::Unchecked);
      m_ActiveArray = -1;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSFilterViewSettings::getSolidColor()
{
  if(false == isValid())
  {
    return nullptr;
  }

  return m_Actor->GetProperty()->GetColor();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setSolidColor(double color[3])
{
  if(false == isValid())
  {
    return;
  }

  m_Actor->GetProperty()->SetColor(color);

  emit solidColorChanged(this, color);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Representation VSFilterViewSettings::getRepresentation()
{
  if(false == isValid())
  {
    return Representation::Invalid;
  }

  vtkProperty* property = m_Actor->GetProperty();
  int rep = property->GetRepresentation();
  int edges = property->GetEdgeVisibility();

  if(1 == edges && Representation::Surface == rep)
  {
    return Representation::SurfaceWithEdges;
  }

  return static_cast<Representation>(rep);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setRepresentation(Representation type)
{
  if(false == isValid())
  {
    return;
  }

  if(type == Representation::SurfaceWithEdges)
  {
    m_Actor->GetProperty()->SetRepresentation(Representation::Surface);
    m_Actor->GetProperty()->EdgeVisibilityOn();
  }
  else
  {
    m_Actor->GetProperty()->SetRepresentation(type);
    m_Actor->GetProperty()->EdgeVisibilityOff();
  }

  emit representationChanged(this, type);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::copySettings(VSFilterViewSettings* copy)
{
  if((nullptr == copy) || (false == copy->isValid()))
  {
    return;
  }

  bool hasUi = copy->getScalarBarWidget();
  if(hasUi)
  {
    vtkRenderWindowInteractor* iren = copy->m_ScalarBarWidget->GetInteractor();
    m_ScalarBarWidget->SetInteractor(iren);
  }

  setVisible(copy->m_ShowFilter);
  setActiveArrayIndex(copy->m_ActiveArray);
  setActiveComponentIndex(copy->m_ActiveComponent);
  setMapColors(copy->m_MapColors);
  setScalarBarVisible(copy->m_ShowScalarBar);
  setAlpha(copy->m_Alpha);
  setSolidColor(copy->getSolidColor());
  setRepresentation(copy->getRepresentation());

  if(hasUi)
  {
    m_LookupTable->copy(*(copy->m_LookupTable));
  }

  emit requiresRender();
}
