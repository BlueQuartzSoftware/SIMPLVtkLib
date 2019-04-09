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

#include <map>
#include <vector>

#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

#include <vtkAbstractMapper3D.h>
#include <vtkActor.h>
#include <vtkCubeAxesActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkOutlineFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>

#include "SIMPLVtkLib/Dialogs/AbstractImportMontageDialog.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkDataSetMapper;
class vtkImageSliceMapper;
class vtkImageSlice;

/**
 * @class VSFilterViewSettings VSFilterViewSettings.h
 * SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h
 * @brief This class handles the visibility settings regarding a given VSVisualFilter
 * for a single VSViewController. This includes setting the vtkDataArray and component
 * to color by, whether or not to map values to a lookup table, and the visibility of
 * both the vtkActor and vtkScalarBarWidget.
 */
class SIMPLVtkLib_EXPORT VSFilterViewSettings : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString filterName READ getFilterName)
  Q_PROPERTY(bool visibility READ isVisible WRITE setVisible NOTIFY visibilityChanged)
  Q_PROPERTY(bool gridVisibility READ isGridVisible WRITE setGridVisible NOTIFY gridVisibilityChanged)
  Q_PROPERTY(QString activeArrayName READ getActiveArrayName WRITE setActiveArrayName NOTIFY activeArrayNameChanged)
  Q_PROPERTY(int activeComponentIndex READ getActiveComponentIndex WRITE setActiveComponentIndex NOTIFY activeComponentIndexChanged)
  Q_PROPERTY(bool pointRendering READ isRenderingPoints NOTIFY pointRenderingChanged)
  Q_PROPERTY(int pointSize READ getPointSize WRITE setPointSize NOTIFY pointSizeChanged)
  Q_PROPERTY(double alpha READ getAlpha WRITE setAlpha NOTIFY alphaChanged)
  Q_PROPERTY(bool showScalarBar READ isScalarBarVisible WRITE setScalarBarVisible NOTIFY showScalarBarChanged)
  Q_PROPERTY(QStringList arrayNames READ getArrayNames NOTIFY arrayNamesChanged)
  Q_PROPERTY(QStringList scalarNames READ getScalarNames NOTIFY scalarNamesChanged)
  Q_PROPERTY(QStringList componentNames READ getComponentNames NOTIFY componentNamesChanged)
  Q_PROPERTY(QColor solidColor READ getSolidColor WRITE setSolidColor NOTIFY solidColorChanged)
  Q_PROPERTY(VSTransform transform READ getTransform)
  Q_PROPERTY(bool fullyImported READ isDataImported NOTIFY dataLoaded)

public:
  using Map = std::map<VSAbstractFilter*, VSFilterViewSettings*>;
  using Collection = std::list<VSFilterViewSettings*>;

  enum class Representation : int
  {
    MultiValues = -2,
    Invalid = -1,
    Outline,
    Points,
    Wireframe,
    Surface,
    SurfaceWithEdges,
    Default = Surface
  };

  enum class ActorType : int
  {
    Invalid = -1,
    DataSet = 0,
    Image2D = 1
  };

  enum class ColorMapping : int
  {
    MultiValues = -1,
    Always = 0,
    NonColors,
    None
  };

  enum class ScalarBarSetting : int
  {
    MultiValues = -1,
    Always = 0,
    OnSelection,
    Never
  };

  Q_ENUMS(Representation)
  Q_ENUMS(ActorType)
  Q_ENUMS(ColorMapping)
  Q_ENUMS(ScalarBarSetting)

  Q_PROPERTY(Representation representation READ getRepresentation WRITE setRepresentation NOTIFY representationChanged)
  Q_PROPERTY(ColorMapping mapColors READ getMapColors WRITE setMapColors NOTIFY mapColorsChanged)
  Q_PROPERTY(ScalarBarSetting scalarBarSetting READ getScalarBarSetting WRITE setScalarBarSetting NOTIFY scalarBarSettingChanged)

  VSFilterViewSettings();

  /**
   * @brief Constructor
   * @param filter
   */
  VSFilterViewSettings(VSAbstractFilter* filter, Representation representation = Representation::Default,
                       AbstractImportMontageDialog::DisplayType displayType = AbstractImportMontageDialog::DisplayType::NotSpecified);

  /**
   * @brief Copy constructor
   * @param copy
   */
  VSFilterViewSettings(const VSFilterViewSettings& copy);

  /**
   * @brief Deconstructor
   */
  virtual ~VSFilterViewSettings();

  /**
   * @brief Deep copies values from another VSFilterViewSettings
   * @param target
   */
  void deepCopy(VSFilterViewSettings* target);

  /**
   * @brief Returns a pointer to the VSAbstractFilter
   * @return
   */
  VSAbstractFilter* getFilter();

  /**
   * @brief getFilterName required for filterName property.
   * Retrieves the value from the current filter.
   * @return
   */
  QString getFilterName() const;

  /**
   * @brief getTransform required for transform property
   * Retrieves the VSTransform from the current filter
   * @return
   */
  VSTransform* getTransform() const;

  /**
   * @brief Returns true if the VSFilterViewSettings can be displayed and false otherwise
   * @return
   */
  bool isValid() const;

  /**
   * @brief Returns true if the data has been imported for the current filter. Returns false otherwise.
   * @return
   */
  bool isDataImported() const;

  /**
   * @brief Returns true if the filter is displayed for this view.  Returns false otherwise
   * @return
   */
  bool isVisible() const;

  /**
   * @brief Returns true if the axes grid is displayed for this view.  Returns false otherwise.
   */
  bool isGridVisible() const;

  /**
   * @brief Returns the active array name used to render the filter
   * @return
   */
  QString getActiveArrayName() const;

  /**
   * @brief Returns the active array component used to render the filter
   * @return
   */
  int getActiveComponentIndex() const;

  /**
   * @brief Return the name for the active component
   * @return
   */
  QString getActiveComponentName() const;

  /**
   * @brief Returns the array names that can be used to color the data
   * @return
   */
  QStringList getArrayNames() const;

  /**
   * @brief Returns the array names that can be used to color the data
   * @return
   */
  QStringList getScalarNames() const;

  /**
   * @brief Returns the component names available for the active array
   * @return
   */
  QStringList getComponentNames();

  /**
   * @brief Returns the component names available for the given array
   * @return
   */
  QStringList getComponentNames(QString arrayName);

  /**
   * @brief Returns the number of components for the given array
   * @param arrayIndex
   * @return
   */
  int getNumberOfComponents(int arrayIndex);

  /**
   * @brief Returns the number of components for the given array
   * @param arrayIndex
   * @return
   */
  int getNumberOfComponents(QString name);

  /**
   * @brief Returns true if the active array has multiple components
   * @return
   */
  bool hasMulipleComponents();

  /**
   * @brief Returns the ColorMapping value for mapping filter values to a lookup table.
   * @return
   */
  ColorMapping getMapColors();

  /**
   * @brief Returns tha alpha transparency used for the displaying the actor
   */
  double getAlpha();

  /**
   * @brief Returns true if the ScalarBarWidget is visible.  Returns false otherwise
   * @return
   */
  bool isScalarBarVisible() const;

  /**
   * @brief Returns the ScalarBarSetting enum value
   * @return
   */
  ScalarBarSetting getScalarBarSetting() const;

  /**
   * @brief Sets the ScalarBarSetting
   * @param setting
   */
  void setScalarBarSetting(ScalarBarSetting setting);

  /**
   * @brief Sets the selection state
   * @param selected
   */
  void setIsSelected(bool selected);

  /**
   * @brief Returns the vtkActor used to render the filter
   * @return
   */
  VTK_PTR(vtkProp3D) getActor();

  /**
   * @brief Returns the vtkScalarBarWidget used for the filter
   * @return
   */
  VTK_PTR(vtkScalarBarWidget) getScalarBarWidget();

  /**
   * @brief Returns the vtkCubeAxesActor used for showing the axes grid
   * @return
   */
  VTK_PTR(vtkCubeAxesActor) getCubeAxesActor();

  /**
   * @brief Returns the color used when no scalar data exists as a double*
   * @return
   */
  double* getSolidColorPtr() const;

  /**
   * @brief Returns the color used when no scalar data exists as a QColor
   * @return
   */
  QColor getSolidColor() const;

  /**
   * @brief Returns the actor property representation
   * @return
   */
  Representation getRepresentation() const;

  /**
   * @brief Returns the actor property representation as an integer
   * @return
   */
  int getRepresentationi() const;

  /**
   * @brief Returns the actor property representation as an enum
   * @return
   */
  ActorType getActorType() const;

  /**
   * @brief Returns true if the actor should be using point data instead of cell data.
   * Return false otherwise.
   * @return
   */
  bool isPointData() const;

  /**
   * @brief Returns true if the current actor is set to render points.
   * @return
   */
  bool isRenderingPoints();

  /**
   * @brief Returns the point size from the actor.
   * If ActorType is Image2D or Invalid, returns -1.
   * @return
   */
  int getPointSize() const;

  /**
   * @brief Sets the point size for the vtkActor. Does nothing for Image2D ActorTypes.
   * @param pointSize
   */
  void setPointSize(int pointSize);

  /**
   * @brief Returns true if the current actor renders points as spheres. Returns false otherwise.
   * @return
   */
  bool renderPointsAsSpheres() const;

  /**
   * @brief Sets whether or not points are rendered as spheres
   * @param renderSpheres
   */
  void setRenderPointsAsSpheres(bool renderSpheres);

  /**
   * @brief Copies another VSFilterViewSettings for everything but the active filter
   * @param filter
   */
  void copySettings(VSFilterViewSettings* other);

  /**
   * @brief Returns the data representation menu
   * @return
   */
  QMenu* getRepresentationMenu();

  /**
   * @brief Returns the ColorBy menu
   * @return
   */
  QMenu* getColorByMenu();

  /**
   * @brief Returns the ArrayComponent menu
   * @return
   */
  QMenu* getArrayComponentMenu();

  /**
   * @brief Returns the scalar mapping menu
   * @return
   */
  QMenu* getMapScalarsMenu();

  /**
   * @brief Returns an action allowing the user to pick the solid color.
   * @return
   */
  QAction* getSetColorAction();

  /**
   * @brief Returns an action allowing the user to set the object's opacity
   * @return
   */
  QAction* getSetOpacityAction();

  /**
   * @brief Returns an action allowing the user to turn off the scalar bar widget
   * @return
   */
  QAction* getToggleScalarBarAction();
  
  /**
   * @brief Returns the subsampling rate.
   * @return
   */
  int getSubsampling() const;

  /**
   * @brief Set the display type
   * @param displayType
   */
  void setDisplayType(AbstractImportMontageDialog::DisplayType displayType);

  /**
   * @brief Get the display type
   * @return
   */
  AbstractImportMontageDialog::DisplayType getDisplayType();

  /**
   * @brief Set the default transform
   * @param defaultTransform
   */
  void setDefaultTransform(VSTransform* defaultTransform);

  /**
   * @brief Get the display type
   * @return
   */
  VSTransform* getDefaultTransform();

  /**
   * @brief Returns the QIcon used for solid colors
   * @return
   */
  static QIcon GetSolidColorIcon();

  /**
   * @brief Returns the QIcon used for cell data
   * @return
   */
  static QIcon GetCellDataIcon();

  /**
   * @brief Returns the QIcon used for point data
   * @return
   */
  static QIcon GetPointDataIcon();

  /////////////////
  // Collections //
  /////////////////
  /**
   * @brief Returns a QStringList of all shared array names in the given collection
   * @param collection
   * @return
   */
  static QStringList GetArrayNames(VSFilterViewSettings::Collection collection);

  /**
   * @brief Returns a QStringList of the component names for the given array name in the collection.
   * Returns an empty list if there arrays are not compatible.
   * @param collection
   * @param arrayName
   */
  static QStringList GetComponentNames(VSFilterViewSettings::Collection collection, QString arrayName);

  /**
   * @brief Returns the first valid point size in the collection.  Returns -1 if none of the settings render as points.
   * @param collection
   * @return
   */
  static int GetPointSize(VSFilterViewSettings::Collection collection);

  /**
   * @brief Sets the point size for all items in the collection.
   * @param collection
   * @param size
   */
  static void SetPointSize(VSFilterViewSettings::Collection collection, int size);

  /**
   * @brief Returns whether or not items in the collection are rendering as points.
   * Returns Qt::Checked if all valid settings render as points, Qt::Unchecked if none do,
   * and Qt::PartiallyChecked if there's a mix of point and non-point rendering.
   * @param collection
   * @return
   */
  static Qt::CheckState IsRenderingPoints(VSFilterViewSettings::Collection collection);

  /**
   * @brief Returns whether or not items in the collection are rendering points as spheres.
   * Returns Qt::Checked if all valid settings render points as point, Qt::Unchecked if none do,
   * and Qt::PartiallyChecked if there's a mix of points as spheres and non-points spheres being rendered.
   * @param collection
   * @return
   */
  static Qt::CheckState IsRenderingPointsAsSpheres(VSFilterViewSettings::Collection collection);

  /**
   * @brief Sets whether or not items in the collection should render as points as spheres.
   * @param collection
   * @param renderSpheres
   */
  static void SetRenderPointsAsSpheres(VSFilterViewSettings::Collection collection, bool renderSpheres);

  /**
   * @brief Returns whether or not the grid is visible for all items in the collection.
   * Returns Qt::Checked if all valid items have visible grids, Qt::Unchecked if none do,
   * and Qt::PartiallyChecked if some do and some do not.
   * @param collection
   * @return
   */
  static Qt::CheckState IsGridVisible(VSFilterViewSettings::Collection collection);

  /**
   * @brief Sets whether or not the grid is visible for all items in the collection
   * @param collection
   * @param visible
   */
  static void SetGridVisible(VSFilterViewSettings::Collection collection, bool visible);

  /**
   * @brief Returns true if the collection includes valid VSFilterViewSettings.  Returns false otherwise.
   * @param collection
   * @return
   */
  static bool HasValidSettings(VSFilterViewSettings::Collection collection);

  /**
   * @brief Returns true if all valid items in the collection have the same active array name.  Returns false otherwise.
   * @param collection
   * @return
   */
  static bool ActiveArrayNamesConsistent(VSFilterViewSettings::Collection collection);

  /**
   * @brief Returns the active array name for the collection.  Returns a null QString if there are multiple values
   * @param collection
   * @return
   */
  static QString GetActiveArrayName(VSFilterViewSettings::Collection collection);

  /**
   * @brief Sets the active array name for items in the collection.
   * @param collection
   * @param arrayName
   */
  static void SetActiveArrayName(VSFilterViewSettings::Collection collection, QString arrayName);

  /**
   * @brief Returns the active component index for items in the collection.  If multiple indices are found, -2 is returned instead.
   * @param collection
   * @return
   */
  static int GetActiveComponentIndex(VSFilterViewSettings::Collection collection);

  /**
   * @brief Sets the active component index for items in the collection
   * @param collection
   * @param index
   */
  static void SetActiveComponentIndex(VSFilterViewSettings::Collection collection, int index);

  /**
   * @brief Sets the subsampling for items in the collection
   * @param collection
   * @param index
   */
  static void SetSubsampling(VSFilterViewSettings::Collection collection, int value);

  /**
   * @brief Returns the number of components for the given arrayName in the collection.
   * @param collection
   * @param arrayName
   */
  static int GetNumberOfComponents(VSFilterViewSettings::Collection collection, QString arrayName);

  /**
   * @brief Returns true if the given array for items in the collection are compatible
   * @param collection
   * @param arrayName
   * @return
   */
  static bool CheckComponentNamesCompatible(VSFilterViewSettings::Collection collection, QString arrayName);

  /**
   * @brief Returns the active component name as a combination of the array and component name.
   * @param collection
   * @return
   */
  static QString GetActiveComponentName(VSFilterViewSettings::Collection collection);

  /**
   * @brief Returns the representation for items in the collection as an integer.  Returns -2 if there are multiple representations.
   * @param collection
   * @return
   */
  static int GetRepresentationi(VSFilterViewSettings::Collection collection);

  /**
   * @brief Sets the representation for items in the collection
   * @param collection
   * @param rep
   */
  static void SetRepresentation(VSFilterViewSettings::Collection collection, Representation rep);

  /**
   * @brief Returns the first valid color for items in the collection.
   * @param collection
   * @return
   */
  static QColor GetSolidColor(VSFilterViewSettings::Collection collection);

  /**
   * @brief Sets the solid color to use for all items in the collection.
   * @param collection
   * @param color
   */
  static void SetSolidColor(VSFilterViewSettings::Collection collection, QColor color);

  /**
   * @brief Returns the ActorType for all valid items in the collection.  Returns Invalid if multiple types are found.
   * @param collection
   * @return
   */
  static ActorType GetActorType(VSFilterViewSettings::Collection collection);

  /**
   * @brief Returns the ColorMapping value for all valid items in the collection.  If multiple values are detected, return MultiValues.
   * @param collection
   * @return
   */
  static ColorMapping GetColorMapping(VSFilterViewSettings::Collection collection);

  /**
   * @param Returns the ScalarBarSetting used by valid items in the collection.  If multiple values are detected, return MultiValues.
   * @param collection
   * @return
   */
  static ScalarBarSetting GetScalarBarSettings(VSFilterViewSettings::Collection collection);

  /**
   * @brief Returns the alpha to display in the user interface for the collection.
   * @return
   */
  static double GetAlpha(VSFilterViewSettings::Collection collection);

  /**
   * @brief Returns the subsampling to display in the user interface for the collection.
   * @return
   */
  static double GetSubsampling(VSFilterViewSettings::Collection collection);


public slots:
  /**
   * @brief Displays the vtkActor for this view
   */
  void show();

  /**
   * @brief Hides the vtkActor for this view
   */
  void hide();

  /**
   * @brief Sets the vtkActor's visibility for this view
   * @param visible
   */
  void setVisible(bool visible);

  /**
   * @brief Sets the vtkCubeAxesActor's visibility for this view
   * @param visible
   */
  void setGridVisible(bool visible);

  /**
   * @brief Updates the active array name for this view
   * @param name
   */
  void setActiveArrayName(QString name);

  /**
   * @brief Updates the active component index for this view
   * @param index
   */
  void setActiveComponentIndex(int index);

  /**
   * @brief Updates whether or not the data values are mapped to the lookup table for this view
   * @param mapColors
   */
  void setMapColors(ColorMapping mapColors);

  /**
   * @brief Sets the object's alpha transparency
   * @param alpha
   */
  void setAlpha(double alpha);

  /**
   * @brief Inverts the scalar bar so that colors at the higher end of the scale
   * are moved to the bottom and colors at the bottom are moved to the top.
   */
  void invertScalarBar();

  /**
   * @brief Set the preset colors for the VSLookupTableController
   * @param json
   */
  void loadPresetColors(const QJsonObject& json);

  /**
   * @brief Helper method to disable the ScalarBarWidget
   */
  void hideScalarBarWidget();

  /**
   * @brief Sets the color to use when there are no scalar values to map
   * @param color
   */
  void setSolidColorPtr(double* color);

  /**
   * @brief Sets the color to use when there are no scalar values to map
   * @param color
   */
  void setSolidColor(QColor color);

  /**
   * @brief Sets the actor property representation
   * @param type
   */
  void setRepresentation(const Representation& type);

  /**
   * @brief Checks the imported data type, changes the representation, and renders the changes
   */
  void importedData();

  /**
   * @brief Updates the actors for the newly reloaded data type, resets the camera, and renders the changes
   */
  void reloadedData();

  /**
   * @brief Updates the actors for the current data type and renders the changes
   */
  void checkDataType();

  /**
   * @brief Updates the input connection for the vtkMapper
   * @param filter
   */
  void updateInputPort(VSAbstractFilter* filter);

  /**
   * @brief Updates the transformation for 2D image data
   */
  void updateTransform();

  /**
   * @brief Handles updated input to show textured quads
   * @param filter
   */
  void inputUpdated(VSAbstractFilter* filter);

signals:
  void visibilityChanged(const bool&);
  void gridVisibilityChanged(const bool&);
  void activeArrayNameChanged(const QString&);
  void activeComponentIndexChanged(const int&);
  void pointRenderingChanged();
  void pointSizeChanged(const int&);
  void alphaChanged(const double&);
  void showScalarBarChanged(const bool&);
  void scalarBarSettingChanged(const ScalarBarSetting&);
  void arrayNamesChanged();
  void scalarNamesChanged();
  void componentNamesChanged();
  void subsamplingChanged(const int&);

  void representationChanged(const Representation&);
  void mapColorsChanged(const ColorMapping&);

  void renderPointSpheresChanged(const bool&);
  void solidColorChanged();

  void requiresRender();
  void actorsUpdated();
  void dataLoaded();
  void swappingActors(vtkProp3D* oldProp, vtkProp3D* newProp);

protected:
  /**
   * @brief Creates the static icons used used.
   */
  static void SetupStaticIcons();

  /**
   * @brief Performs initial setup commands for any actors used in the view settings
   */
  void setupActors(bool outline = true);

  /**
   * @brief Creates a vtkImageSliceMapper and vtkImageSlice for displaying 2D Image data
   */
  void setupImageActors();

  /**
   * @brief Creates a vtkDataSetMapper and vtkActor for displaying generic vtkDataSets
   */
  void setupDataSetActors();

  /**
   * @brief Creates a vtkAxisActor for displaying the axis grid
   */
  void setupCubeAxesActor();

  /**
   * @brief Creates and initializes QActions
   */
  void setupActions();

  /**
   * @brief Returns the vtkDataSetMapper if the ActorType is DataSet and the settings are valid.
   * Returns nullptr otherwise.
   * @return
   */
  vtkDataSetMapper* getDataSetMapper() const;

  /**
   * @brief Returns the vtkActor if the ActorType is DataSet and the settings are valid.
   * Returns nullptr otherwise.
   * @return
   */
  vtkActor* getDataSetActor() const;

  /**
   * @brief Returns the vtkImageSliceMapper if the ActorType is Image2D and the settings are valid.
   * Returns nullptr otherwise.
   * @return
   */
  vtkImageSliceMapper* getImageMapper() const;

  /**
   * @brief Returns the vtkImageSlice if the ActorType is Image2D and the settings are valid.
   * Returns nullptr otherwise.
   * @return
   */
  vtkImageSlice* getImageSliceActor() const;

  /**
   * @brief Returns true if data set is a 2D image.  Returns false otherwise.
   * @return
   */
  bool isFlatImage();

  /**
   * @brief Returns true if there is only a single array in the filter output's point data.
   * Returns false if there are no arrays or more than one.
   */
  bool hasSinglePointArray();

  /**
   * @brief Updates the alpha for DataSet actors
   */
  void updateDataSetAlpha();

  /**
   * @breif Updates the alpha for 2D Image actors
   */
  void updateImageAlpha();

  /**
   * @brief Sets the visual filter makes any Qt connections required
   * @param filter
   */
  void connectFilter(VSAbstractFilter* filter);

  /**
   * @brief Handles the target filter being deleted.
   */
  void filterDeleted();

  /**
   * @brief Returns the array name based on the given index
   * @param index
   * @return
   */
  QString getArrayNameByIndex(int index);

  /**
   * @brief Returns the array at the given index
   * @param index
   * @return
   */
  vtkDataArray* getArrayAtIndex(int index);

  /**
   * @brief Returns the array with the given name
   * @param name
   * @return
   */
  vtkDataArray* getArrayByName(QString name) const;

  /**
   * @brief Updates the mapper color mode to match variable values
   */
  void updateColorMode();

  /**
   * @brief Returns true if colors are mapped. Returns false otherwise.
   * @return
   */
  bool isMappingColors() const;

  /**
   * @brief Returns true if the given array is a 3-Component color array
   * @param array
   * @return
   */
  bool isColorArray(vtkDataArray* array) const;

  /**
   * @brief Returns the current vtkDataArray specified by the given array index
   */
  vtkDataArray* getDataArray() const;

  /**
   * @brief Updates whether or not the vtkScalarBarWidget is visible for this view
   * @param visible
   */
  void setScalarBarVisible(bool visible);

  /**
   * @brief Checks if the vtkScalarBarWidget should be visible and updates accordingly
   */
  void updateScalarBarVisibility();

  /**
   * @brief Update the subsampling for the texture image
   */
  void setSubsampling(int value);

  /**
   * @brief Update the texture
   */
  void updateTexture();

private:
  VSAbstractFilter* m_Filter = nullptr;
  ActorType m_ActorType = ActorType::Invalid;
  VTK_PTR(vtkDataSetSurfaceFilter) m_DataSetFilter = nullptr;
  bool m_ShowFilter = true;
  QString m_ActiveArrayName;
  int m_ActiveComponent = -1;
  int m_Subsampling = 1;
  ColorMapping m_MapColors = ColorMapping::NonColors;
  Representation m_Representation = Representation::Default;
  AbstractImportMontageDialog::DisplayType m_DisplayType = AbstractImportMontageDialog::DisplayType::NotSpecified;
  VSTransform* m_DefaultTransform = nullptr;
  VTK_PTR(vtkAbstractMapper3D) m_Mapper = nullptr;
  VTK_PTR(vtkProp3D) m_Actor = nullptr;
  VTK_PTR(vtkProp3D) m_OutlineActor = nullptr;
  VTK_PTR(vtkTexture) m_Texture = nullptr;
  VTK_PTR(vtkOutlineFilter) m_OutlineFilter = nullptr;
  VTK_PTR(vtkPlaneSource) m_Plane = nullptr;
  VSLookupTableController* m_LookupTable = nullptr;
  ScalarBarSetting m_ScalarBarSetting = ScalarBarSetting::Never;
  double m_Alpha = 1.0;
  VTK_PTR(vtkScalarBarActor) m_ScalarBarActor = nullptr;
  VTK_PTR(vtkScalarBarWidget) m_ScalarBarWidget = nullptr;
  bool m_HadNoArrays = false;
  VTK_PTR(vtkCubeAxesActor) m_CubeAxesActor = nullptr;
  bool m_GridVisible = false;
  bool m_Selected = false;

  QAction* m_SetColorAction = nullptr;
  QAction* m_SetOpacityAction = nullptr;
  QAction* m_ToggleScalarBarAction = nullptr;
  static QIcon* s_SolidColorIcon;
  static QIcon* s_CellDataIcon;
  static QIcon* s_PointDataIcon;

  static double* NULL_COLOR;
};

Q_DECLARE_METATYPE(VSFilterViewSettings)
