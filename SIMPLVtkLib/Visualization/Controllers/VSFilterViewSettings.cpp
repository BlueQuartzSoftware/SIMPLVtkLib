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
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetMapper.h>
#include <vtkImageActor.h>
#include <vtkImageProperty.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageData.h>
#include <vtkMapper.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>

double* VSFilterViewSettings::NULL_COLOR = new double[3]{ 0.0, 0.0, 0.0 };

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
VSAbstractFilter* VSFilterViewSettings::getFilter()
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isValid()
{
  bool valid = m_Mapper && m_Actor;
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
  return m_ShowScalarBar && m_ScalarBarWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkProp3D) VSFilterViewSettings::getActor()
{
  return m_Actor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkDataSetMapper* VSFilterViewSettings::getDataSetMapper()
{
  if(ActorType::DataSet == m_ActorType &&  isValid())
  {
    return dynamic_cast<vtkDataSetMapper*>(m_Mapper.Get());
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkActor* VSFilterViewSettings::getDataSetActor()
{
  if(ActorType::DataSet == m_ActorType && isValid())
  {
    return vtkActor::SafeDownCast(m_Actor);
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkImageSliceMapper* VSFilterViewSettings::getImageMapper()
{
  if(ActorType::Image2D == m_ActorType && isValid())
  {
    return dynamic_cast<vtkImageSliceMapper*>(m_Mapper.Get());
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkImageSlice* VSFilterViewSettings::getImageSliceActor()
{
  if(ActorType::Image2D == m_ActorType && isValid())
  {
    return dynamic_cast<vtkImageSlice*>(m_Actor.Get());
  }

  return nullptr;
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
  vtkDataSetMapper* dataMapper = getDataSetMapper();
  if(nullptr == dataMapper)
  {
    return nullptr;
  }

  vtkDataSet* dataSet = dataMapper->GetInput();
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
  if(nullptr == getDataSetMapper())
  {
    return;
  }

  // Draw a solid color if index is -1
  if(index == -1)
  {
    getDataSetMapper()->SelectColorArray(-1);
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
  vtkDataSetMapper* mapper = getDataSetMapper();
  if(nullptr == mapper)
  {
    return;
  }

  m_ActiveComponent = index;

  VTK_PTR(vtkScalarsToColors) lookupTable = mapper->GetLookupTable();
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
  mapper->ColorByArrayComponent(m_ActiveArray, index);
  mapper->SetScalarModeToUseCellFieldData();
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
  vtkDataSetMapper* mapper = getDataSetMapper();
  if(nullptr == mapper)
  {
    return;
  }

  vtkDataArray* dataArray = getDataArray();
  bool unmapColorArray = isColorArray(dataArray) && (m_ActiveComponent == -1);

  if(m_MapColors && !unmapColorArray)
  {
    mapper->SetColorModeToMapScalars();
  }
  else
  {
    mapper->SetColorModeToDirectScalars();
  }

  mapper->Update();
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
  if(alpha < 0.0)
  {
    alpha = 0.0;
  }
  else if(alpha > 1.0)
  {
    alpha = 1.0;
  }

  m_Alpha = alpha;

  switch(m_ActorType)
  {
  case ActorType::DataSet:
    updateDataSetAlpha();
    break;
  case ActorType::Image2D:
    updateImageAlpha();
    break;
  default:
    return;
  }

  emit alphaChanged(this, m_Alpha);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateDataSetAlpha()
{
  vtkActor* actor = getDataSetActor();
  if(nullptr == actor)
  {
    return;
  }

  vtkProperty* property = actor->GetProperty();
  property->SetOpacity(m_Alpha);
  actor->SetProperty(property);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateImageAlpha()
{
  vtkImageSlice* actor = getImageSliceActor();
  if(nullptr == actor)
  {
    return;
  }

  vtkImageProperty* property = actor->GetProperty();
  property->SetOpacity(m_Alpha);
  actor->SetProperty(property);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::invertScalarBar()
{
  if(false == isValid() || nullptr == m_LookupTable)
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
  if(false == isValid() || nullptr == m_LookupTable)
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

  if(isFlatImage())
  {
    setupImageActors();
  }
  else
  {
    setupDataSetActors();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isFlatImage()
{
  VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();
  if(nullptr == outputData)
  {
    return false;
  }

  vtkImageData* imageData = dynamic_cast<vtkImageData*>(outputData.Get());
  if(nullptr == imageData)
  {
    return false;
  }

  if(imageData->GetPointData()->GetNumberOfArrays() != 1)
  {
    return false;
  }

  // Check extents
  int* extent = imageData->GetExtent();
  for(int i = 0; i < 3; i++)
  {
    if(extent[2 * i + 1] - extent[2 * i] <= 1)
    {
      return true;
    }
  }
  
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupImageActors()
{
  vtkImageSliceMapper* mapper = vtkImageSliceMapper::New();
  mapper->SetInputConnection(m_Filter->getOutputPort());
  mapper->SliceAtFocalPointOn();
  mapper->SliceFacesCameraOff();
  m_Mapper = mapper;

  vtkImageSlice* actor = vtkImageSlice::New();
  actor->SetMapper(mapper);
  m_Actor = actor;

  setMapColors(Qt::Unchecked);
  setScalarBarVisible(false);

  m_ActorType = ActorType::Image2D;

  updateTransform();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupDataSetActors()
{
  VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();

  m_DataSetFilter = VTK_PTR(vtkDataSetSurfaceFilter)::New();
  m_DataSetFilter->SetInputConnection(m_Filter->getTransformedOutputPort());

  m_Mapper = VTK_PTR(vtkDataSetMapper)::New();
  m_Mapper->SetInputConnection(m_DataSetFilter->GetOutputPort());
  vtkDataSetMapper* mapper = dynamic_cast<vtkDataSetMapper*>(m_Mapper.Get());

  m_Actor = VTK_PTR(vtkActor)::New();
  dynamic_cast<vtkActor*>(m_Actor.Get())->SetMapper(mapper);

  m_LookupTable = new VSLookupTableController();
  mapper->SetLookupTable(m_LookupTable->getColorTransferFunction());

  m_ScalarBarActor = VTK_PTR(vtkScalarBarActor)::New();
  m_ScalarBarActor->SetLookupTable(mapper->GetLookupTable());
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

  m_ActorType = ActorType::DataSet;
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

  if(m_DataSetFilter)
  {
    m_DataSetFilter->SetInputConnection(filter->getTransformedOutputPort());
    m_DataSetFilter->Update();
  }
  else
  {
    m_Mapper->SetInputConnection(filter->getOutputPort());
    m_Actor->SetUserTransform(m_Filter->getTransform()->getGlobalTransform());
  }
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateTransform()
{
  if(false == (m_Filter && m_Filter->getTransform() && m_Actor))
  {
    return;
  }

  if(ActorType::Image2D == m_ActorType)
  {
    VSTransform* transform =  m_Filter->getTransform();
    m_Actor->SetPosition(transform->getPosition());
    m_Actor->SetOrientation(transform->getRotation());
    m_Actor->SetScale(transform->getScale());
  }

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
    disconnect(m_Filter, SIGNAL(transformChanged()), this, SLOT(updateTransform()));
  }

  m_Filter = filter;
  if(filter)
  {
    connect(filter, SIGNAL(updatedOutputPort(VSAbstractFilter*)), this, SLOT(updateInputPort(VSAbstractFilter*)));
    connect(filter, SIGNAL(transformChanged()), this, SLOT(updateTransform()));

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
  vtkActor* actor = getDataSetActor();
  if(nullptr == actor)
  {
    return NULL_COLOR;
  }

  return actor->GetProperty()->GetColor();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setSolidColor(double color[3])
{
  vtkActor* actor = getDataSetActor();
  if(nullptr == actor)
  {
    return;
  }

  actor->GetProperty()->SetColor(color);

  emit solidColorChanged(this, color);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Representation VSFilterViewSettings::getRepresentation()
{
  vtkActor* actor = getDataSetActor();
  if(nullptr == actor)
  {
    return Representation::Invalid;
  }

  vtkProperty* property = actor->GetProperty();
  Representation rep = static_cast<Representation>(property->GetRepresentation());
  int edges = property->GetEdgeVisibility();

  if(1 == edges && Representation::Surface == rep)
  {
    return Representation::SurfaceWithEdges;
  }

  return rep;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getRepresentationi()
{
  return static_cast<int>(getRepresentation());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::ActorType VSFilterViewSettings::getActorType()
{
  return m_ActorType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setRepresentation(Representation type)
{
  vtkActor* actor = getDataSetActor();
  if(nullptr == actor)
  {
    return;
  }

  if(type == Representation::SurfaceWithEdges)
  {
    actor->GetProperty()->SetRepresentation(static_cast<int>(Representation::Surface));
    actor->GetProperty()->EdgeVisibilityOn();
  }
  else
  {
    actor->GetProperty()->SetRepresentation(static_cast<int>(type));
    actor->GetProperty()->EdgeVisibilityOff();
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
