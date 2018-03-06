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

#include <vector>
#include <map>

#include <QtCore/QObject>
#include <QtGui/QColor>

#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkAbstractMapper3D.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>

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

public:
  using Map = std::map<VSAbstractFilter*, VSFilterViewSettings*>;

  enum class Representation : int
  {
    Invalid = -1,
    Outline,
    Points,
    Wireframe,
    Surface,
    SurfaceWithEdges,
    Default = Outline
  };

  enum class ActorType : int
  {
    Invalid = -1,
    DataSet = 0,
    Image2D = 1
  };

  /**
  * @brief Constructor
  * @param filter
  */
  VSFilterViewSettings(VSAbstractFilter* filter);

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
  * @brief Returns a pointer to the VSAbstractFilter
  * @return
  */
  VSAbstractFilter* getFilter();

  /**
  * @brief Returns true if the VSFilterViewSettings can be displayed and false otherwise
  * @return
  */
  bool isValid();

  /**
  * @brief Returns true if the filter is displayed in for this view.  Returns false otherwise
  * @return
  */
  bool isVisible();

  /**
  * @brief Returns the active array index used to render the filter
  * @return
  */
  int getActiveArrayIndex();

  /**
  * @brief Returns the active array component used to render the filter
  * @return
  */
  int getActiveComponentIndex();

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
  * @brief Returns the check state for mapping filter values to a lookup table.
  * @return
  */
  Qt::CheckState getMapColors();

  /**
  * @brief Returns tha alpha transparency used for the displaying the actor
  */
  double getAlpha();

  /**
  * @brief Returns true if the ScalarBarWidget is visible.  Returns false otherwise
  * @return
  */
  bool isScalarBarVisible();

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
  * @brief Returns the color used when no scalar data exists as a double*
  * @return
  */
  double* getSolidColor();

  /**
  * @brief Returns the actor property representation
  * @return
  */
  Representation getRepresentation();

  /**
  * @brief Returns the actor property representation as an integer
  * @return
  */
  int getRepresentationi();

  /**
  * @brief Returns the current actor type
  * @return
  */
  ActorType getActorType();

  /**
  * @brief Copies another VSFilterViewSettings for everything but the active filter
  * @param filter
  */
  void copySettings(VSFilterViewSettings* other);

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
  * @brief Updates the active array index for this view
  * @param index
  */
  void setActiveArrayIndex(int index);

  /**
  * @brief Updates the active component index for this view
  * @param index
  */
  void setActiveComponentIndex(int index);

  /**
  * @brief Updates whether or not the data values are mapped to the lookup table for this view
  * @param mapColors
  */
  void setMapColors(Qt::CheckState mapColorState);

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
  * @brief Updates whether or not the vtkScalarBarWidget is visible for this view
  * @param visible
  */
  void setScalarBarVisible(bool visible);

  /**
  * @brief Sets the color to use when there are no scalar values to map
  * @param color
  */
  void setSolidColor(double color[3]);

  /**
  * @brief Sets the actor property representation
  * @param type
  */
  void setRepresentation(Representation type);

  void importedData();

  void checkDataType();

  /**
  * @brief Updates the input connection for the vtkMapper
  * @param filter
  */
  void updateInputPort(VSAbstractFilter* filter);

signals:
  void visibilityChanged(VSFilterViewSettings*, bool);
  void representationChanged(VSFilterViewSettings*, Representation);
  void solidColorChanged(VSFilterViewSettings*, double*);
  void activeArrayIndexChanged(VSFilterViewSettings*, int);
  void activeComponentIndexChanged(VSFilterViewSettings*, int);
  void mapColorsChanged(VSFilterViewSettings*, Qt::CheckState);
  void alphaChanged(VSFilterViewSettings*, double);
  void showScalarBarChanged(VSFilterViewSettings*, bool);
  void requiresRender();
  void swappingActors(vtkProp3D* oldProp, vtkProp3D* newProp);

protected:
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
  * @brief Returns the vtkDataSetMapper if the ActorType is DataSet and the settings are valid.
  * Returns nullptr otherwise.
  * @return
  */
  vtkDataSetMapper* getDataSetMapper();

  /**
  * @brief Returns the vtkActor if the ActorType is DataSet and the settings are valid.
  * Returns nullptr otherwise.
  * @return
  */
  vtkActor* getDataSetActor();

  /**
  * @brief Returns the vtkImageSliceMapper if the ActorType is Image2D and the settings are valid.
  * Returns nullptr otherwise.
  * @return
  */
  vtkImageSliceMapper* getImageMapper();

  /**
  * @brief Returns the vtkImageSlice if the ActorType is Image2D and the settings are valid.
  * Returns nullptr otherwise.
  * @return
  */
  vtkImageSlice* getImageSliceActor();

  /**
  * @brief Returns true if data set is a 2D image.  Returns false otherwise.
  * @return
  */
  bool isFlatImage();

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
  * @brief Returns the array at the given index
  * @param index
  * @return
  */
  vtkDataArray* getArrayAtIndex(int index);

  /**
  * @brief Updates the mapper color mode to match variable values
  */
  void updateColorMode();

  /**
  * @brief Returns true if the given array is a 3-Component color array
  * @param array
  * @return
  */
  bool isColorArray(vtkDataArray* array);

  /**
  * @brief Returns the current vtkDataArray specified by the given array index
  */
  vtkDataArray* getDataArray();

private:
  VSAbstractFilter* m_Filter = nullptr;
  ActorType m_ActorType = ActorType::Invalid;
  VTK_PTR(vtkDataSetSurfaceFilter) m_DataSetFilter = nullptr;
  bool m_ShowFilter = true;
  int m_ActiveArray = 0;
  int m_ActiveComponent = -1;
  Qt::CheckState m_MapColors = Qt::Checked;
  Representation m_Representation = Representation::Default;
  VTK_PTR(vtkAbstractMapper3D) m_Mapper = nullptr;
  VTK_PTR(vtkProp3D) m_Actor = nullptr;
  bool m_ShowScalarBar = true;
  VSLookupTableController* m_LookupTable = nullptr;
  double m_Alpha = 1.0;
  VTK_PTR(vtkScalarBarActor) m_ScalarBarActor = nullptr;
  VTK_PTR(vtkScalarBarWidget) m_ScalarBarWidget = nullptr;

  static double* NULL_COLOR;
};
