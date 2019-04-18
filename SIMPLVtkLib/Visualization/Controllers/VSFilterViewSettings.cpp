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

#include <QtWidgets/QColorDialog>
#include <QtWidgets/QInputDialog>

#include <vtkAbstractArray.h>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetMapper.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageProperty.h>
#include <vtkImageSliceMapper.h>
#include <vtkMapper.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkTexture.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkExtractVOI.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractDataFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"

double* VSFilterViewSettings::NULL_COLOR = new double[3]{0.0, 0.0, 0.0};
QIcon* VSFilterViewSettings::s_SolidColorIcon = nullptr;
QIcon* VSFilterViewSettings::s_CellDataIcon = nullptr;
QIcon* VSFilterViewSettings::s_PointDataIcon = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::VSFilterViewSettings()
: QObject(nullptr)
, m_Filter(nullptr)
, m_ShowFilter(false)
, m_ActiveComponent(-1)
, m_Representation(Representation::Invalid)
{
  SetupStaticIcons();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::VSFilterViewSettings(VSAbstractFilter* filter, Representation representation, AbstractImportMontageDialog::DisplayType displayType)
: QObject(nullptr)
, m_ShowFilter(true)
, m_DisplayType(displayType)
, m_Representation(representation)
{
  SetupStaticIcons();

  connectFilter(filter);
  setupActions();
  bool isSIMPL = dynamic_cast<VSSIMPLDataContainerFilter*>(filter);
  setupActors(isSIMPL);
  if(isSIMPL && (representation == Representation::Surface ||
	 representation == Representation::SurfaceWithEdges))
  {
	  connect(filter, SIGNAL(updatedOutputPort(VSAbstractFilter*)), this, SLOT(inputUpdated(VSAbstractFilter*)));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::VSFilterViewSettings(const VSFilterViewSettings& copy)
: QObject(nullptr)
, m_ShowFilter(copy.m_ShowFilter)
, m_ActiveArrayName(copy.m_ActiveArrayName)
, m_ActiveComponent(copy.m_ActiveComponent)
, m_MapColors(copy.m_MapColors)
, m_Alpha(copy.m_Alpha)
{
  connectFilter(copy.m_Filter);
  setupActions();
  setupActors();
  setActiveArrayName(copy.m_ActiveArrayName);
  setActiveComponentIndex(copy.m_ActiveComponent);
  setScalarBarVisible(copy.isScalarBarVisible());
  setRepresentation(copy.getRepresentation());
  setActiveArrayName(copy.m_ActiveArrayName);
  setActiveComponentIndex(copy.m_ActiveComponent);
  setSolidColor(copy.getSolidColor());
  setPointSize(copy.getPointSize());
  setRepresentation(copy.getRepresentation());
  setIsSelected(copy.m_Selected);

  if(copy.m_LookupTable)
  {
    // Create a new VSLookupTableController if necessary
    if(nullptr == m_LookupTable)
    {
      m_LookupTable = copy.m_LookupTable->deepCopy();
    }
    else
    {
      m_LookupTable->copy(*(copy.m_LookupTable));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::~VSFilterViewSettings()
{
  if(m_LookupTable)
  {
    delete m_LookupTable;
    m_LookupTable = nullptr;
  }

  m_Filter = nullptr;
  m_DefaultTransform = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::deepCopy(VSFilterViewSettings* target)
{
  connectFilter(target->m_Filter);
  setVisible(target->isVisible());
  setScalarBarVisible(target->isScalarBarVisible());
  setScalarBarSetting(target->getScalarBarSetting());
  setRepresentation(target->getRepresentation());
  setActiveArrayName(target->m_ActiveArrayName);
  setActiveComponentIndex(target->m_ActiveComponent);
  setSolidColor(target->getSolidColor());
  setPointSize(target->getPointSize());
  setIsSelected(target->m_Selected);
  setDefaultTransform(target->getDefaultTransform());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetupStaticIcons()
{
  if(nullptr == s_SolidColorIcon)
  {
    s_SolidColorIcon = new QIcon(":icons/pqSolidColor.png");
  }

  if(nullptr == s_CellDataIcon)
  {
    s_CellDataIcon = new QIcon(":icons/pqCellData.png");
  }

  if(nullptr == s_PointDataIcon)
  {
    s_PointDataIcon = new QIcon(":icons/pqPointData.png");
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
QString VSFilterViewSettings::getFilterName() const
{
  if(m_Filter)
  {
    return m_Filter->getFilterName();
  }
  else
  {
    return "[Filter Missing]";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform* VSFilterViewSettings::getTransform() const
{
  if(nullptr == m_Filter)
  {
    return new VSTransform();
  }

  return m_Filter->getTransform();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isValid() const
{
  if(nullptr == m_Filter)
  {
    return false;
  }

  bool valid = m_Mapper && m_Actor;
  return valid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isDataImported() const
{
  if(nullptr == m_Filter)
  {
    return false;
  }

  return m_Filter->isDataImported();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupActions()
{
  m_SetColorAction = new QAction("Set Color", this);
  connect(m_SetColorAction, &QAction::triggered, [=] {
    QColor color = QColorDialog::getColor(getSolidColor());
    if(color.isValid())
    {
      setSolidColor(color);
    }
  });

  m_SetOpacityAction = new QAction("Set Opacity", this);

  // The step variable was introduced somewhere between v5.9 and v5.10.1
  // As the only valid values are between 0.0 and 1.0, step values of 1.0 do not make sense.
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 1)
  connect(m_SetOpacityAction, &QAction::triggered, [=] {
    setAlpha(QInputDialog::getDouble(nullptr, "Set Opacity for '" + getFilter()->getFilterName() + "'", "Opacity", getAlpha(), 0.0, 1.0, 2, nullptr, Qt::WindowFlags(), 0.1));
#else
  connect(m_SetOpacityAction, &QAction::triggered, [=] {
    setAlpha(QInputDialog::getDouble(nullptr, "Set Opacity for '" + getFilter()->getFilterName() + "'", "Opacity", getAlpha(), 0.0, 1.0, 2));
#endif
  });

  m_ToggleScalarBarAction = new QAction("Enable Scalar Bar", this);
  m_ToggleScalarBarAction->setCheckable(true);
  m_ToggleScalarBarAction->setChecked(false);
  connect(m_ToggleScalarBarAction, &QAction::toggled, [=](bool checked) { setScalarBarVisible(checked); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isVisible() const
{
  return m_ShowFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isGridVisible() const
{
  return m_GridVisible;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFilterViewSettings::getActiveArrayName() const
{
  return m_ActiveArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getActiveComponentIndex() const
{
  return m_ActiveComponent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFilterViewSettings::getActiveComponentName() const
{
  if(getActiveArrayName().isEmpty())
  {
    return "Solid Color";
  }

  vtkDataArray* dataArray = getArrayByName(m_ActiveArrayName);
  if(nullptr == dataArray)
  {
    return "No Data";
  }

  int numComponents = dataArray->GetNumberOfComponents();
  bool isColorArray = dataArray->IsA("vtkUnsignedCharArray") && numComponents == 3;
  if(numComponents == 1)
  {
    return m_ActiveArrayName;
  }
  else if(isColorArray && m_ActiveComponent == -1)
  {
    return m_ActiveArrayName;
  }
  else if(m_ActiveComponent == -1)
  {
    return m_ActiveArrayName + " Magnitude";
  }
  else
  {
    return dataArray->GetComponentName(m_ActiveComponent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSFilterViewSettings::getArrayNames() const
{
  if(m_Filter)
  {
    QStringList arrayNames = m_Filter->getArrayNames();
    arrayNames.prepend("Solid Colors");
    return arrayNames;
  }

  return QStringList("Solid Colors");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSFilterViewSettings::getScalarNames() const
{
  if(m_Filter)
  {
    return m_Filter->getScalarNames();
  }

  return QStringList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSFilterViewSettings::getComponentNames()
{
  if(m_Filter)
  {
    return m_Filter->getComponentList(getActiveArrayName());
  }

  return QStringList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSFilterViewSettings::getComponentNames(QString arrayName)
{
  if(m_Filter)
  {
    return m_Filter->getComponentList(arrayName);
  }

  return QStringList();
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

  vtkAbstractArray* array = nullptr;

  if(isPointData())
  {
    vtkPointData* pointData = m_Filter->getOutput()->GetPointData();
    array = pointData->GetAbstractArray(arrayIndex);
  }
  else
  {
    vtkCellData* cellData = m_Filter->getOutput()->GetCellData();
    array = cellData->GetAbstractArray(arrayIndex);
  }

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

  vtkAbstractArray* array = nullptr;

  if(isPointData())
  {
    vtkPointData* pointData = m_Filter->getOutput()->GetPointData();
    array = pointData->GetAbstractArray(qPrintable(arrayName));
  }
  else
  {
    vtkCellData* cellData = m_Filter->getOutput()->GetCellData();
    array = cellData->GetAbstractArray(qPrintable(arrayName));
  }

  if(array)
  {
    return array->GetNumberOfComponents();
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::hasMulipleComponents()
{
  int numComp = getNumberOfComponents(getActiveArrayName());
  return numComp > 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::ColorMapping VSFilterViewSettings::getMapColors()
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
bool VSFilterViewSettings::isScalarBarVisible() const
{
  return m_ScalarBarWidget && m_ToggleScalarBarAction->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::ScalarBarSetting VSFilterViewSettings::getScalarBarSetting() const
{
  return m_ScalarBarSetting;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setScalarBarSetting(ScalarBarSetting setting)
{
  m_ScalarBarSetting = setting;
  emit scalarBarSettingChanged(setting);

  switch(setting)
  {
  case ScalarBarSetting::Always:
    setScalarBarVisible(true);
    break;
  case ScalarBarSetting::OnSelection:
    setScalarBarVisible(m_Selected);
    break;
  case ScalarBarSetting::Never:
    setScalarBarVisible(false);
    break;
  default:
    setScalarBarVisible(false);
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setIsSelected(bool selected)
{
  m_Selected = selected;
  updateScalarBarVisibility();
  vtkActor* actor = getDataSetActor();
  if(nullptr == actor)
  {
    return;
  }
  if(selected && isFlatImage())
  {
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(0.0, 1.0, 0.0);
    actor->SetPosition(actor->GetPosition()[0], actor->GetPosition()[1], actor->GetPosition()[2] + 1.0);
  }
  else
  {
    if(m_Representation != Representation::SurfaceWithEdges || m_Representation != Representation::Outline)
    {
      actor->GetProperty()->EdgeVisibilityOff();
    }
    if(isFlatImage())
    {
      actor->GetProperty()->SetEdgeColor(1.0, 1.0, 1.0);
    }
    actor->SetPosition(actor->GetPosition()[0], actor->GetPosition()[1], actor->GetPosition()[2] - 1.0);
  }
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
vtkDataSetMapper* VSFilterViewSettings::getDataSetMapper() const
{
  if(ActorType::DataSet == m_ActorType && isValid())
  {
    return dynamic_cast<vtkDataSetMapper*>(m_Mapper.Get());
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkActor* VSFilterViewSettings::getDataSetActor() const
{
  if((ActorType::DataSet == m_ActorType || ActorType::Image2D == m_ActorType)
	&& isValid())
  {
    return vtkActor::SafeDownCast(m_Actor);
  }
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkImageSliceMapper* VSFilterViewSettings::getImageMapper() const
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
vtkImageSlice* VSFilterViewSettings::getImageSliceActor() const
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
VTK_PTR(vtkCubeAxesActor) VSFilterViewSettings::getCubeAxesActor()
{
  return m_CubeAxesActor;
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

  emit visibilityChanged(m_ShowFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setGridVisible(bool visible)
{
  if(false == isValid())
  {
    return;
  }

  m_GridVisible = visible;

  emit gridVisibilityChanged(m_GridVisible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFilterViewSettings::getArrayNameByIndex(int index)
{
  if(nullptr == m_Filter || index < 0)
  {
    return QString::null;
  }

  QStringList arrayNames = m_Filter->getArrayNames();
  if(index < arrayNames.size())
  {
    return arrayNames[index];
  }

  return QString::null;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkDataArray* VSFilterViewSettings::getArrayAtIndex(int index)
{
  vtkDataSet* dataSet = m_Filter->getOutput();
  if(nullptr == dataSet)
  {
    return nullptr;
  }

  if(isPointData())
  {
    if(dataSet->GetPointData())
    {
      return dataSet->GetPointData()->GetArray(index);
    }
  }
  else
  {
    if(dataSet->GetCellData())
    {
      return dataSet->GetCellData()->GetArray(index);
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkDataArray* VSFilterViewSettings::getArrayByName(QString name) const
{
  vtkDataSet* dataSet = m_Filter->getOutput();
  if(nullptr == dataSet)
  {
    return nullptr;
  }

  if(isPointData())
  {
    if(dataSet->GetPointData())
    {
      return dataSet->GetPointData()->GetArray(qPrintable(name));
    }
  }
  else
  {
    if(dataSet->GetCellData())
    {
      return dataSet->GetCellData()->GetArray(qPrintable(name));
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setActiveArrayName(QString name)
{
  if(nullptr == getDataSetMapper())
  {
    return;
  }

  // Check for Solid Color
  if(name.isNull())
  {
    getDataSetMapper()->SelectColorArray(-1);
    m_ActiveArrayName = QString::null;

    emit activeArrayNameChanged(m_ActiveArrayName);
    emit componentNamesChanged();
    emit requiresRender();

    updateScalarBarVisibility();
	updateTexture();
    return;
  }

  VTK_PTR(vtkDataArray) dataArray = getArrayByName(name);
  if(nullptr == dataArray)
  {
    return;
  }

  m_ActiveArrayName = name;

  emit activeArrayNameChanged(m_ActiveArrayName);
  emit componentNamesChanged();
  setActiveComponentIndex(-1);
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
  VTK_PTR(vtkDataArray) dataArray = getArrayByName(m_ActiveArrayName);
  if(nullptr == dataArray)
  {
    return;
  }

  // Clamp index with lower bound
  if(index < -1)
  {
    index = -1;
  }

  // Set data type to map
  if(isPointData())
  {
    mapper->SetScalarModeToUsePointFieldData();
  }
  else
  {
    mapper->SetScalarModeToUseCellFieldData();
  }

  // Set array component index in the vtkDataSetMapper
  int numComponents = dataArray->GetNumberOfComponents();
  mapper->ColorByArrayComponent(qPrintable(m_ActiveArrayName), index);
  updateColorMode();

  // Set ScalarBar title
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
	updateTexture();
  }
  else if(index < numComponents)
  {
    double* range = dataArray->GetRange(index);
    m_LookupTable->setRange(range);
    m_ScalarBarActor->SetTitle(dataArray->GetComponentName(index));
  }

  emit activeComponentIndexChanged(m_ActiveComponent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkDataArray* VSFilterViewSettings::getDataArray() const
{
  if(nullptr == m_Filter->getOutput())
  {
    return nullptr;
  }

  return getArrayByName(m_ActiveArrayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isColorArray(vtkDataArray* dataArray) const
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
  bool mapColors = (ColorMapping::NonColors == m_MapColors && !unmapColorArray) || ColorMapping::Always == m_MapColors;

  if(mapColors)
  {
    mapper->SetColorModeToMapScalars();
  }
  else
  {
    mapper->SetColorModeToDirectScalars();
  }

  mapper->Update();
  updateScalarBarVisibility();
  updateTexture();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isMappingColors() const
{
  vtkDataSetMapper* mapper = getDataSetMapper();
  if(nullptr == mapper)
  {
    return false;
  }

  switch(m_MapColors)
  {
  case ColorMapping::Always:
    return true;
  case ColorMapping::None:
    return false;
  case ColorMapping::NonColors:
  {
    vtkDataArray* dataArray = getDataArray();
    bool isColorData = isColorArray(dataArray) && (m_ActiveComponent == -1);
    return !isColorData;
  }
  default:
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setMapColors(ColorMapping mapColors)
{
  if(false == isValid())
  {
    return;
  }

  m_MapColors = mapColors;

  updateColorMode();
  emit mapColorsChanged(m_MapColors);
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

  emit alphaChanged(m_Alpha);
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
void VSFilterViewSettings::hideScalarBarWidget()
{
  if(false == isValid())
  {
    return;
  }

  setScalarBarVisible(false);
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

  m_ToggleScalarBarAction->setChecked(visible);
  emit showScalarBarChanged(visible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateScalarBarVisibility()
{
  if(false == isValid())
  {
    return;
  }

  if(m_ActiveArrayName.isNull() || m_Representation == Representation::Outline)
  {
    setScalarBarVisible(false);
    return;
  }

  if(false == isMappingColors())
  {
    setScalarBarVisible(false);
  }
  else
  {
    switch(m_ScalarBarSetting)
    {
    case ScalarBarSetting::Always:
      setScalarBarVisible(true);
      break;
    case ScalarBarSetting::Never:
      setScalarBarVisible(false);
      break;
    case ScalarBarSetting::OnSelection:
      setScalarBarVisible(m_Selected);
      break;
    default:
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupActors(bool outline)
{
  VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();
  if(nullptr == outputData)
  {
    m_ShowFilter = false;
    return;
  }

  if(isFlatImage() && hasSinglePointArray())
  {
    setupImageActors();
  }
  else
  {
    setupDataSetActors();

    if(isFlatImage())
    {
      setScalarBarSetting(ScalarBarSetting::Never);
    }
    else
    {
      // Refresh ScalarBarSetting
      setScalarBarSetting(m_ScalarBarSetting);
    }
  }

  setupCubeAxesActor();

  //if(outline)
  //{
  //  setRepresentation(Representation::Outline);
  //}
  emit requiresRender();
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

  if(m_Filter->getOutputType() != VSAbstractFilter::dataType_t::IMAGE_DATA)
  {
    return false;
  }

  vtkImageData* imageData = dynamic_cast<vtkImageData*>(outputData.Get());
  if(nullptr == imageData)
  {
    return false;
  }

  // Check dimensions
  int* dims = imageData->GetDimensions();
  for(int i = 0; i < 3; i++)
  {
    if(dims[i] <= 1)
    {
      return true;
    }
  }

  // Check bounds of image data
  double* bounds = imageData->GetBounds();
  if(bounds[4] == 0 && (bounds[5] == 0 || bounds[5] == 1))
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::hasSinglePointArray()
{
  VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();
  if(nullptr == outputData)
  {
    return false;
  }

  if(outputData->GetPointData()->GetNumberOfArrays() != 1)
  {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupImageActors()
{
  VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();
  VTK_PTR(vtkPlaneSource) plane = VTK_PTR(vtkPlaneSource)::New();

  vtkDataSetMapper* mapper;
  vtkActor* actor;
  if(ActorType::DataSet == m_ActorType || nullptr == m_Actor)
  {
    mapper = vtkDataSetMapper::New();

    actor = vtkActor::New();
    actor->SetMapper(mapper);

    setMapColors(ColorMapping::None);
    setScalarBarVisible(false);
  }
  else
  {
    mapper = vtkDataSetMapper::SafeDownCast(m_Mapper);
    actor = vtkActor::SafeDownCast(m_Actor);
  }
  
  mapper->SetInputConnection(plane->GetOutputPort());

  if(ActorType::DataSet == m_ActorType && isVisible())
  {
    emit swappingActors(m_Actor.Get(), actor);
  }

  m_Mapper = mapper;
  m_Actor = actor;
  m_Plane = plane;

  m_ActorType = ActorType::Image2D;
  updateTexture();
  updateTransform();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupDataSetActors()
{
  VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();
  VTK_PTR(vtkPlaneSource) plane = VTK_PTR(vtkPlaneSource)::New();

  vtkDataSetMapper* mapper;
  vtkActor* actor;
  if(ActorType::Image2D == m_ActorType || nullptr == m_Actor)
  {
    m_DataSetFilter = VTK_PTR(vtkDataSetSurfaceFilter)::New();
    m_OutlineFilter = VTK_PTR(vtkOutlineFilter)::New();
    mapper = vtkDataSetMapper::New();
    mapper->ReleaseDataFlagOn();
    actor = vtkActor::New();

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
  }
  else
  {
    mapper = vtkDataSetMapper::SafeDownCast(m_Mapper);
    actor = vtkActor::SafeDownCast(m_Actor);
  }

  m_DataSetFilter->SetInputConnection(m_Filter->getTransformedOutputPort());
  m_OutlineFilter->SetInputConnection(m_Filter->getOutputPort());

  updateTexture();

  if(!isFlatImage())
  {
    mapper->SetInputConnection(m_DataSetFilter->GetOutputPort());
  }
  else if(getRepresentation() == Representation::Outline)
  {
    mapper->SetInputConnection(m_OutlineFilter->GetOutputPort());
  }
  else
  {
    mapper->SetInputConnection(plane->GetOutputPort());
  }
  actor->SetMapper(mapper);

  // Check if there are any arrays to use
  bool hasArrays = false;
  if(isPointData())
  {
    hasArrays = outputData->GetPointData()->GetNumberOfArrays() > 0;
  }
  else
  {
    hasArrays = outputData->GetCellData()->GetNumberOfArrays() > 0;
  }

  // Set Mapper to use the active array and component
  if(hasArrays)
  {
    if(m_HadNoArrays)
    {
      setMapColors(ColorMapping::NonColors);
      setActiveArrayName(getArrayNameByIndex(0));
      m_HadNoArrays = false;
    }
    else
    {
      setActiveArrayName(m_ActiveArrayName);
      setActiveComponentIndex(m_ActiveComponent);
    }
  }
  else
  {
    setMapColors(ColorMapping::None);
    setScalarBarVisible(false);
    m_HadNoArrays = true;
  }

  if(ActorType::Image2D == m_ActorType && isVisible())
  {
    emit swappingActors(m_Actor.Get(), actor);
  }

  m_Mapper = mapper;
  m_Actor = actor;
  m_Plane = plane;

  m_ActorType = ActorType::DataSet;
  if(isFlatImage())
  {
    vtkImageData* imageData = dynamic_cast<vtkImageData*>(outputData.Get());

    double spacing[3];
    imageData->GetSpacing(spacing);

    // Get transform vectors
    VSTransform* transform = m_Filter->getTransform();

    double scaling[3] = {spacing[0], spacing[1], spacing[2]};

    transform->setLocalScale(scaling);

    // Save the initial transform
    VSTransform* defaultTransform = getDefaultTransform();
    defaultTransform->setLocalPosition(m_Filter->getTransform()->getLocalPosition());
    defaultTransform->setLocalRotation(m_Filter->getTransform()->getLocalRotation());
    defaultTransform->setLocalScale(m_Filter->getTransform()->getLocalScale());
  }
  updateTransform();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupCubeAxesActor()
{
  if(nullptr == m_CubeAxesActor)
  {
    m_CubeAxesActor = VTK_PTR(vtkCubeAxesActor)::New();

    m_CubeAxesActor->XAxisMinorTickVisibilityOff();
    m_CubeAxesActor->YAxisMinorTickVisibilityOff();
    m_CubeAxesActor->ZAxisMinorTickVisibilityOff();
  }

  if(m_Filter && m_Filter->getOutput())
  {
    m_CubeAxesActor->SetBounds(m_Filter->getBounds());
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

  if(!m_DataSetFilter)
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

  if(Representation::Outline == m_Representation)
  {
    VSTransform* transform = m_Filter->getTransform();
    m_Actor->SetPosition(transform->getPosition());
    m_Actor->SetOrientation(transform->getRotation());
    m_Actor->SetScale(transform->getScale());
  }
  else if(ActorType::Image2D == m_ActorType || ActorType::DataSet == m_ActorType)
  {
	VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();
	vtkImageData* imageData = dynamic_cast<vtkImageData*>(outputData.Get());
	double bounds[6];
	imageData->GetBounds(bounds);
	int extent[6];
	imageData->GetExtent(extent);
	// Set Zmax to 1.0f for Image2D actor types
	if(ActorType::Image2D == m_ActorType && extent[5] == 0)
	{
	  extent[5] = 1.0f;
	}

	// Get transform vectors
	VSTransform* transform = m_Filter->getTransform();
	double* transformPosition = transform->getPosition();
	double* transformRotation = transform->getRotation();
	double* transformScale = transform->getScale();
	  
	m_Actor->SetPosition(transformPosition);
	  m_Actor->SetOrientation(transformRotation);
	  m_Actor->SetScale(extent[1] * transformScale[0], extent[3] * transformScale[1],
		extent[5] * transformScale[2]);
  }
  else
  {
    m_Actor->SetPosition(0.0, 0.0, 0.0);
    m_Actor->SetOrientation(0.0, 0.0, 0.0);
    m_Actor->SetScale(1.0, 1.0, 1.0);
  }

  if(m_CubeAxesActor && m_Filter->getOutput())
  {
    m_CubeAxesActor->SetBounds(m_Filter->getTransformBounds());
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
    disconnect(m_Filter, SIGNAL(transformChanged()), this, SIGNAL(updateTransform()));
    disconnect(m_Filter, &VSAbstractFilter::removeFilter, this, &VSFilterViewSettings::filterDeleted);
    disconnect(m_Filter, &VSAbstractFilter::arrayNamesChanged, this, &VSFilterViewSettings::arrayNamesChanged);
    disconnect(m_Filter, &VSAbstractFilter::scalarNamesChanged, this, &VSFilterViewSettings::scalarNamesChanged);
    disconnect(m_Filter, &VSAbstractFilter::dataImported, this, &VSFilterViewSettings::dataLoaded);

    if(dynamic_cast<VSAbstractDataFilter*>(m_Filter))
    {
      disconnect(m_Filter, SIGNAL(dataImported()), this, SLOT(importedData()));
      disconnect(m_Filter, SIGNAL(dataReloaded()), this, SLOT(reloadedData()));
    }
  }

  m_Filter = filter;
  if(filter)
  {
    connect(filter, SIGNAL(updatedOutputPort(VSAbstractFilter*)), this, SLOT(updateInputPort(VSAbstractFilter*)));
    connect(filter, SIGNAL(transformChanged()), this, SLOT(updateTransform()));
    connect(filter, &VSAbstractFilter::removeFilter, this, &VSFilterViewSettings::filterDeleted);
    connect(filter, &VSAbstractFilter::arrayNamesChanged, this, &VSFilterViewSettings::arrayNamesChanged);
    connect(filter, &VSAbstractFilter::scalarNamesChanged, this, &VSFilterViewSettings::scalarNamesChanged);
    connect(filter, &VSAbstractFilter::dataImported, this, &VSFilterViewSettings::dataLoaded);

    if(filter->getArrayNames().size() < 1)
    {
      setScalarBarVisible(false);
      setMapColors(ColorMapping::None);
      m_ActiveArrayName = QString::null;
      m_HadNoArrays = true;
    }
    else
    {
      m_HadNoArrays = false;
    }

    if(dynamic_cast<VSAbstractDataFilter*>(filter))
    {
      connect(filter, SIGNAL(dataImported()), this, SLOT(importedData()));
      connect(filter, SIGNAL(dataReloaded()), this, SLOT(reloadedData()));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::filterDeleted()
{
  setVisible(false);
  deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSFilterViewSettings::getSolidColorPtr() const
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
void VSFilterViewSettings::setSolidColorPtr(double* color)
{
  vtkActor* actor = getDataSetActor();
  if(nullptr == actor)
  {
    return;
  }

  actor->GetProperty()->SetColor(color);

  emit solidColorChanged();
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor VSFilterViewSettings::getSolidColor() const
{
  QColor colorOut;
  double* color = getSolidColorPtr();
  colorOut.setRgbF(color[0], color[1], color[2]);
  return colorOut;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setSolidColor(QColor color)
{
  double dColor[3];
  dColor[0] = color.redF();
  dColor[1] = color.greenF();
  dColor[2] = color.blueF();

  setSolidColorPtr(dColor);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Representation VSFilterViewSettings::getRepresentation() const
{
  return m_Representation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getRepresentationi() const
{
  return static_cast<int>(getRepresentation());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::ActorType VSFilterViewSettings::getActorType() const
{
  return m_ActorType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isPointData() const
{
  return m_Filter->isPointData();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isRenderingPoints()
{
  return isPointData() || Representation::Points == getRepresentation();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getPointSize() const
{
  if(ActorType::DataSet == getActorType() && getDataSetActor())
  {
    return getDataSetActor()->GetProperty()->GetPointSize();
  }

  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setPointSize(int pointSize)
{
  if(ActorType::DataSet == getActorType() && getDataSetActor())
  {
    getDataSetActor()->GetProperty()->SetPointSize(pointSize);
    emit pointSizeChanged(pointSize);
    emit requiresRender();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::renderPointsAsSpheres() const
{
  if(ActorType::DataSet == getActorType() && getDataSetActor())
  {
    return getDataSetActor()->GetProperty()->GetRenderPointsAsSpheres();
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setRenderPointsAsSpheres(bool renderSpheres)
{
  if(ActorType::DataSet == getActorType() && getDataSetActor())
  {
    getDataSetActor()->GetProperty()->SetRenderPointsAsSpheres(renderSpheres);
    emit renderPointSpheresChanged(renderSpheres);
    emit requiresRender();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setRepresentation(const Representation& type)
{
  vtkActor* actor = getDataSetActor();
  if(nullptr == actor)
  {
    m_Representation = Representation::Invalid;
    return;
  }

  const Representation prevRep = m_Representation;
  m_Representation = type;
  if(type == Representation::Outline)
  {
    getDataSetMapper()->SetInputConnection(m_OutlineFilter->GetOutputPort());
    actor->GetProperty()->SetRepresentation(static_cast<int>(Representation::Wireframe));
  }
  else
  {
    if(isFlatImage())
    {
      getDataSetMapper()->SetInputConnection(m_Plane->GetOutputPort());
      updateTexture();
    }
    else
    {
      getDataSetMapper()->SetInputConnection(m_DataSetFilter->GetOutputPort());
    }

    if(type == Representation::SurfaceWithEdges)
    {
      actor->GetProperty()->SetRepresentation(static_cast<int>(Representation::Surface));
      actor->GetProperty()->EdgeVisibilityOn();
    }
    else
    {
      actor->GetProperty()->SetRepresentation(static_cast<int>(type) - 1);
      actor->GetProperty()->EdgeVisibilityOff();
    }
  }

  // Emit a signal if the Representation changes to or from Points
  if((prevRep == Representation::Points && type != Representation::Points) || (prevRep != Representation::Points && type == Representation::Points))
  {
    emit pointRenderingChanged();
  }

  updateTransform();
  updateScalarBarVisibility();
  emit representationChanged(type);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::importedData()
{
  if(dynamic_cast<VSAbstractDataFilter*>(getFilter()) && getRepresentation() == Representation::Outline)
  {
    setupActors(false);
    if(getImageMapper() && getDataSetActor())
    {
      getDataSetActor()->VisibilityOff();
      getDataSetMapper()->ScalarVisibilityOff();
    }

    // Force active arrays
    QStringList arrayNames = getFilter()->getArrayNames();
    if(arrayNames.count() > 0)
    {
      setActiveArrayName(arrayNames[0]);
    }
    else
    {
      setActiveArrayName("");
    }

    setRepresentation(Representation::Surface);
  }

  emit requiresRender();
  emit dataLoaded();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::checkDataType()
{
  setupActors(false);
  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::reloadedData()
{
  setupActors(false);
  emit actorsUpdated();
  emit dataLoaded();
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
  if(hasUi && m_ScalarBarWidget)
  {
    vtkRenderWindowInteractor* iren = copy->m_ScalarBarWidget->GetInteractor();
    m_ScalarBarWidget->SetInteractor(iren);
  }

  setVisible(copy->m_ShowFilter);
  setActiveArrayName(copy->m_ActiveArrayName);
  setActiveComponentIndex(copy->m_ActiveComponent);
  setMapColors(copy->m_MapColors);
  setScalarBarVisible(copy->m_ToggleScalarBarAction->isChecked());
  setScalarBarSetting(copy->m_ScalarBarSetting);
  m_Selected = copy->m_Selected;
  setAlpha(copy->m_Alpha);
  setSolidColor(copy->getSolidColor());
  setRepresentation(copy->getRepresentation());
  setPointSize(copy->getPointSize());

  if(hasUi && m_ScalarBarWidget)
  {
    m_LookupTable->copy(*(copy->m_LookupTable));
  }

  emit requiresRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* VSFilterViewSettings::getRepresentationMenu()
{
  QMenu* representationMenu = new QMenu("Representation");

  QAction* outlineAction = representationMenu->addAction("Outline");
  outlineAction->setCheckable(true);
  outlineAction->setChecked(getRepresentation() == VSFilterViewSettings::Representation::Outline);
  connect(outlineAction, &QAction::triggered, [=] { setRepresentation(VSFilterViewSettings::Representation::Outline); });

  QAction* pointsAction = representationMenu->addAction("Points");
  pointsAction->setCheckable(true);
  pointsAction->setChecked(getRepresentation() == VSFilterViewSettings::Representation::Points);
  connect(pointsAction, &QAction::triggered, [=] { setRepresentation(VSFilterViewSettings::Representation::Points); });

  QAction* wireframeAction = representationMenu->addAction("WireFrame");
  wireframeAction->setCheckable(true);
  wireframeAction->setChecked(getRepresentation() == VSFilterViewSettings::Representation::Wireframe);
  connect(wireframeAction, &QAction::triggered, [=] { setRepresentation(VSFilterViewSettings::Representation::Wireframe); });

  QAction* surfaceAction = representationMenu->addAction("Surface");
  surfaceAction->setCheckable(true);
  surfaceAction->setChecked(getRepresentation() == VSFilterViewSettings::Representation::Surface);
  connect(surfaceAction, &QAction::triggered, [=] { setRepresentation(VSFilterViewSettings::Representation::Surface); });

  QAction* surfEdgesAction = representationMenu->addAction("Surface with Edges");
  surfEdgesAction->setCheckable(true);
  surfEdgesAction->setChecked(getRepresentation() == VSFilterViewSettings::Representation::SurfaceWithEdges);
  connect(surfEdgesAction, &QAction::triggered, [=] { setRepresentation(VSFilterViewSettings::Representation::SurfaceWithEdges); });

  return representationMenu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* VSFilterViewSettings::getColorByMenu()
{
  QMenu* arrayMenu = new QMenu("Colory By");
  QStringList arrayNames = getFilter()->getArrayNames();
  int numArrays = arrayNames.size();

  QAction* colorAction = arrayMenu->addAction("Solid Color");
  colorAction->setCheckable(true);
  colorAction->setChecked(getActiveArrayName() == QString::null);
  colorAction->setIcon(GetSolidColorIcon());
  connect(colorAction, &QAction::triggered, [=] { setActiveArrayName(QString::null); });

  QIcon arrayIcon = isPointData() ? GetPointDataIcon() : GetCellDataIcon();
  for(int i = 0; i < numArrays; i++)
  {
    QString arrayName = arrayNames[i];
    QAction* arrayAction = arrayMenu->addAction(arrayName);
    arrayAction->setIcon(arrayIcon);
    arrayAction->setCheckable(true);
    arrayAction->setChecked(getActiveArrayName() == arrayName);
    connect(arrayAction, &QAction::triggered, [=] { setActiveArrayName(arrayName); });
  }

  return arrayMenu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* VSFilterViewSettings::getArrayComponentMenu()
{
  QMenu* componentMenu = new QMenu("Array Component");
  QStringList componentList = m_Filter->getComponentList(getActiveArrayName());
  int count = componentList.size();
  int offset = count > 1 ? 1 : 0;

  // Individual components
  for(int i = 0; i < count; i++)
  {
    QString componentName = componentList[i];
    int index = i - offset; // Account for Magnitude at -1

    QAction* compAction = componentMenu->addAction(componentName);
    compAction->setCheckable(true);
    compAction->setChecked(getActiveComponentIndex() == index);
    connect(compAction, &QAction::triggered, [=] { setActiveComponentIndex(index); });
  }

  return componentMenu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* VSFilterViewSettings::getMapScalarsMenu()
{
  QMenu* mapScalarsMenu = new QMenu("Map Scalars");

  QAction* mapAllAction = mapScalarsMenu->addAction("Always");
  mapAllAction->setCheckable(true);
  mapAllAction->setChecked(getMapColors() == ColorMapping::Always);
  connect(mapAllAction, &QAction::triggered, [=] { setMapColors(ColorMapping::Always); });

  QAction* semiMapAction = mapScalarsMenu->addAction("Non-Color Arrays");
  semiMapAction->setCheckable(true);
  semiMapAction->setChecked(getMapColors() == ColorMapping::NonColors);
  connect(semiMapAction, &QAction::triggered, [=] { setMapColors(ColorMapping::NonColors); });

  QAction* noMapAction = mapScalarsMenu->addAction("Never");
  noMapAction->setCheckable(true);
  noMapAction->setChecked(getMapColors() == ColorMapping::None);
  connect(noMapAction, &QAction::triggered, [=] { setMapColors(ColorMapping::None); });

  return mapScalarsMenu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* VSFilterViewSettings::getSetColorAction()
{
  return m_SetColorAction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* VSFilterViewSettings::getSetOpacityAction()
{
  return m_SetOpacityAction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* VSFilterViewSettings::getToggleScalarBarAction()
{
  return m_ToggleScalarBarAction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon VSFilterViewSettings::GetSolidColorIcon()
{
  if(s_SolidColorIcon)
  {
    return *s_SolidColorIcon;
  }

  return QIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon VSFilterViewSettings::GetCellDataIcon()
{
  if(s_CellDataIcon)
  {
    return *s_CellDataIcon;
  }

  return QIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon VSFilterViewSettings::GetPointDataIcon()
{
  if(s_PointDataIcon)
  {
    return *s_PointDataIcon;
  }

  return QIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::GetPointSize(VSFilterViewSettings::Collection collection)
{
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->getPointSize() != -1)
    {
      return settings->getPointSize();
    }
  }

  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetPointSize(VSFilterViewSettings::Collection collection, int size)
{
  for(VSFilterViewSettings* settings : collection)
  {
    settings->setPointSize(size);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::CheckState VSFilterViewSettings::IsRenderingPoints(VSFilterViewSettings::Collection collection)
{
  bool renderingPoints = false;
  bool valueSet = false;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        renderingPoints = settings->isRenderingPoints();
        valueSet = true;
      }
      else if(settings->isRenderingPoints() != renderingPoints)
      {
        return Qt::CheckState::PartiallyChecked;
      }
    }
  }

  return renderingPoints ? Qt::Checked : Qt::Unchecked;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::CheckState VSFilterViewSettings::IsRenderingPointsAsSpheres(VSFilterViewSettings::Collection collection)
{
  bool renderSpheres = false;
  bool valueSet = false;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        renderSpheres = settings->renderPointsAsSpheres();
        valueSet = true;
      }
      else if(settings->renderPointsAsSpheres() != renderSpheres)
      {
        return Qt::PartiallyChecked;
      }
    }
  }

  return renderSpheres ? Qt::Checked : Qt::Unchecked;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetRenderPointsAsSpheres(VSFilterViewSettings::Collection collection, bool renderSpheres)
{
  for(VSFilterViewSettings* settings : collection)
  {
    settings->setRenderPointsAsSpheres(renderSpheres);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::CheckState VSFilterViewSettings::IsGridVisible(VSFilterViewSettings::Collection collection)
{
  bool gridVisible = false;
  bool valueSet = false;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        gridVisible = settings->isGridVisible();
        valueSet = true;
      }
      else if(settings->isGridVisible() != gridVisible)
      {
        return Qt::PartiallyChecked;
      }
    }
  }

  return gridVisible ? Qt::Checked : Qt::Unchecked;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetGridVisible(VSFilterViewSettings::Collection collection, bool visible)
{
  for(VSFilterViewSettings* settings : collection)
  {
    settings->setGridVisible(visible);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::HasValidSettings(VSFilterViewSettings::Collection collection)
{
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList getMutualArrayNames(QStringList list1, QStringList list2)
{
  QStringList mutualItems;
  for(QString item : list1)
  {
    if(list2.contains(item))
    {
      mutualItems.push_back(item);
    }
  }

  return mutualItems;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSFilterViewSettings::GetArrayNames(VSFilterViewSettings::Collection collection)
{
  bool valueSet = false;
  QStringList arrayNames;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        arrayNames = settings->getFilter()->getArrayNames();
        valueSet = true;
      }
      else
      {
        arrayNames = getMutualArrayNames(arrayNames, settings->getFilter()->getArrayNames());
      }
    }
  }

  return arrayNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSFilterViewSettings::GetComponentNames(VSFilterViewSettings::Collection collection, QString arrayName)
{
  if(collection.size() == 0 || arrayName.isEmpty())
  {
    return QStringList();
  }

  if(CheckComponentNamesCompatible(collection, arrayName))
  {
    for(VSFilterViewSettings* settings : collection)
    {
      if(settings->isValid())
      {
        return settings->getComponentNames(arrayName);
      }
    }
  }

  return QStringList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::ActiveArrayNamesConsistent(VSFilterViewSettings::Collection collection)
{
  bool valueSet = false;
  QString activeArrayName;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        activeArrayName = settings->getActiveArrayName();
      }
      else if(settings->getActiveArrayName().compare(activeArrayName) != 0)
      {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFilterViewSettings::GetActiveArrayName(VSFilterViewSettings::Collection collection)
{
  // Return empty string when there are no values or the value is to use the solid color
  // Reserve null string for multiple values
  if(collection.size() == 0)
  {
    return QString("");
  }

  bool valueSet = false;
  QString activeArray("");
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        valueSet = true;
        activeArray = settings->getActiveArrayName();
        if(activeArray.isEmpty())
        {
          return activeArray;
        }
      }
      else if(settings->getActiveArrayName().compare(activeArray) != 0)
      {
        return QString::Null();
      }
    }
  }

  return activeArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetActiveArrayName(VSFilterViewSettings::Collection collection, QString arrayName)
{
  for(VSFilterViewSettings* settings : collection)
  {
    settings->setActiveArrayName(arrayName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::GetActiveComponentIndex(VSFilterViewSettings::Collection collection)
{
  QString activeArrayName = GetActiveArrayName(collection);
  if(activeArrayName.isEmpty() || false == CheckComponentNamesCompatible(collection, activeArrayName))
  {
    return -2;
  }

  bool valueSet = false;
  int componentIndex = -2;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        componentIndex = settings->getActiveComponentIndex();
        valueSet = true;
      }
      else if(settings->getActiveComponentIndex() != componentIndex)
      {
        return -2;
      }
    }
  }

  return componentIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetActiveComponentIndex(VSFilterViewSettings::Collection collection, int index)
{
  // Do not set the active component index if the components are not compatible
  if(false == CheckComponentNamesCompatible(collection, GetActiveArrayName(collection)))
  {
    return;
  }

  for(VSFilterViewSettings* settings : collection)
  {
    settings->setActiveComponentIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetSubsampling(VSFilterViewSettings::Collection collection, int value)
{
	for(VSFilterViewSettings* settings : collection)
	{
		if(settings->isVisible())
		{
			settings->setSubsampling(value);
		}
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setSubsampling(int value)
{
	m_Subsampling = value;
	updateTexture();
	emit subsamplingChanged(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::GetNumberOfComponents(VSFilterViewSettings::Collection collection, QString arrayName)
{
  if(false == CheckComponentNamesCompatible(collection, arrayName))
  {
    return -1;
  }

  // CheckComponentNamesCompatible should catch any inconsistencies.
  // Return the first valid value
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      return settings->getNumberOfComponents(arrayName);
    }
  }

  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::CheckComponentNamesCompatible(VSFilterViewSettings::Collection collection, QString arrayName)
{
  if(collection.size() == 0)
  {
    return false;
  }

  // Solid colors do not have components
  if(arrayName.isEmpty())
  {
    return false;
  }

  bool listSet = false;
  QStringList componentList;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!listSet)
      {
        componentList = settings->getComponentNames(arrayName);
        if(componentList.size() == 0)
        {
          return false;
        }
        listSet = true;
      }
      else
      {
        // Check if the active array has the same name
        if(false == settings->getArrayNames().contains(arrayName))
        {
          return false;
        }

        // Check the number and name of components
        QStringList currentComponents = settings->getComponentNames(arrayName);
        if(currentComponents.size() != componentList.size())
        {
          return false;
        }

        for(int i = 0; i < currentComponents.size(); i++)
        {
          if(currentComponents[i].compare(componentList[i]) != 0)
          {
            return false;
          }
        }
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFilterViewSettings::GetActiveComponentName(VSFilterViewSettings::Collection collection)
{
  QString arrayName = GetActiveArrayName(collection);
  if(arrayName.isEmpty())
  {
    return QString::Null();
  }

  int componentIndex = GetActiveComponentIndex(collection);
  if(componentIndex < 0)
  {
    return arrayName;
  }
  else
  {
    return arrayName + " [" + GetComponentNames(collection, arrayName)[componentIndex + 1] + " Component]";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::GetRepresentationi(VSFilterViewSettings::Collection collection)
{
  if(collection.size() == 0)
  {
    return static_cast<int>(Representation::Surface);
  }

  bool valueSet = false;
  Representation representation = Representation::Default;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        representation = settings->getRepresentation();
        valueSet = true;
      }
      else if(settings->getRepresentation() != representation)
      {
        return static_cast<int>(Representation::MultiValues);
      }
    }
  }

  return static_cast<int>(representation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetRepresentation(VSFilterViewSettings::Collection collection, Representation rep)
{
  for(VSFilterViewSettings* settings : collection)
  {
    settings->setRepresentation(rep);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::SetSolidColor(VSFilterViewSettings::Collection collection, QColor color)
{
  for(VSFilterViewSettings* settings : collection)
  {
    settings->setSolidColor(color);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor VSFilterViewSettings::GetSolidColor(VSFilterViewSettings::Collection collection)
{
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      QColor color = settings->getSolidColor();
      if(color.isValid())
      {
        return color;
      }
    }
  }

  return QColor();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::ActorType VSFilterViewSettings::GetActorType(VSFilterViewSettings::Collection collection)
{
  bool valueSet = false;
  ActorType value = ActorType::Invalid;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        value = settings->getActorType();
        valueSet = true;
      }
      else if(settings->getActorType() != value)
      {
        return ActorType::Invalid;
      }
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::ColorMapping VSFilterViewSettings::GetColorMapping(VSFilterViewSettings::Collection collection)
{
  if(collection.size() == 0)
  {
    return ColorMapping::NonColors;
  }

  bool valueSet = false;
  ColorMapping value = ColorMapping::MultiValues;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        value = settings->getMapColors();
        valueSet = true;
      }
      else if(value != settings->getMapColors())
      {
        return ColorMapping::MultiValues;
      }
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::ScalarBarSetting VSFilterViewSettings::GetScalarBarSettings(VSFilterViewSettings::Collection collection)
{
  bool valueSet = false;
  ScalarBarSetting value = ScalarBarSetting::OnSelection;
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      if(!valueSet)
      {
        value = settings->getScalarBarSetting();
        valueSet = true;
      }
      else if(settings->getScalarBarSetting() != value)
      {
        return ScalarBarSetting::MultiValues;
      }
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSFilterViewSettings::GetAlpha(VSFilterViewSettings::Collection collection)
{
  // Returns the first valid setting's alpha value
  for(VSFilterViewSettings* settings : collection)
  {
    if(settings->isValid())
    {
      return settings->getAlpha();
    }
  }

  return 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSFilterViewSettings::GetSubsampling(VSFilterViewSettings::Collection collection)
{
	// Returns the first valid setting's subsampling value
	for(VSFilterViewSettings* settings : collection)
	{
		if(settings->isValid())
		{
			return settings->getSubsampling();
		}
	}

	return 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getSubsampling() const
{
	return m_Subsampling;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::updateTexture()
{
  if(!isFlatImage())
  {
    return;
  }
  VTK_PTR(vtkDataSet) outputData = m_Filter->getOutput();
  vtkImageData* imageData = dynamic_cast<vtkImageData*>(outputData.Get());

  VTK_PTR(vtkTexture) texture = VTK_PTR(vtkTexture)::New();

  texture->InterpolateOn();
  texture->SetInputData(imageData);
  if(m_LookupTable != nullptr)
  {
    texture->SetLookupTable(m_LookupTable->getColorTransferFunction());
  }

  if(m_Subsampling > 1)
  {
    VTK_PTR(vtkExtractVOI) subsample = VTK_PTR(vtkExtractVOI)::New();
    int* inputDims = imageData->GetDimensions();
    subsample->SetInputData(imageData);
    subsample->SetSampleRate(m_Subsampling, m_Subsampling, m_Subsampling);
    subsample->Update();

    vtkImageData* extracted = subsample->GetOutput();
    texture->SetInputData(extracted);
  }

  m_Texture = texture;
  vtkActor* actor = getDataSetActor();
  if(nullptr != actor)
  {
    if(!getActiveArrayName().isEmpty() || ActorType::Image2D == m_ActorType)
    {
      actor->SetTexture(m_Texture);
    }
    else
    {
      actor->GetProperty()->RemoveAllTextures();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::inputUpdated(VSAbstractFilter* filter)
{
	if(filter->getOutput() != nullptr)
	{
		VTK_PTR(vtkCellData) cellData = filter->getOutput()->GetCellData();
		if(cellData)
		{
			setActiveArrayName(cellData->GetArrayName(0));
		}
	}
	updateTexture();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setDisplayType(AbstractImportMontageDialog::DisplayType displayType)
{
  m_DisplayType = displayType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractImportMontageDialog::DisplayType VSFilterViewSettings::getDisplayType()
{
  return m_DisplayType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setDefaultTransform(VSTransform* defaultTransform)
{
  m_DefaultTransform = defaultTransform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform* VSFilterViewSettings::getDefaultTransform()
{
  if(m_DefaultTransform == nullptr)
  {
	m_DefaultTransform = new VSTransform(m_Filter->getTransform());
	return m_DefaultTransform;
  }
  else
  {
	return m_DefaultTransform;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isFlat()
{
  return isFlatImage();
}
