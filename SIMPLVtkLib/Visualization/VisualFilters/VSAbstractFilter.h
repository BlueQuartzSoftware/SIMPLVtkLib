/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <memory>

#include <vtkAlgorithmOutput.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkOutlineFilter.h>
#include <vtkTransformFilter.h>
#include <vtkTrivialProducer.h>

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtGui/QStandardItemModel>

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTransform.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class VSAbstractDataFilter;

/**
* @class VSAbstractFilter VSAbstractFilter.h 
* SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h
* @brief This is the base class for all visual filters in SIMPLVtkLib.
* Classes that inherit from this handle various vtk algorithms for filtering 
* out parts of the vtkDataSet input into the top-level VSSIMPLDataContainerFilter.  Filters
* can be chained together to be more specific about what kind of data should be 
* shown by pushing the output of a filter as the input for each of its child filters.
*/
class SIMPLVtkLib_EXPORT VSAbstractFilter : public QObject, public QStandardItem
{
  Q_OBJECT

public:
  enum dataType_t
  {
    IMAGE_DATA,
    RECTILINEAR_GRID,
    STRUCTURED_GRID,
    UNSTRUCTURED_GRID,
    POLY_DATA,
    POINT_DATA,
    ANY_DATA_SET,
    INVALID_DATA
  };

  SIMPL_INSTANCE_PROPERTY(QJsonObject, LoadingObject)
  SIMPL_BOOL_PROPERTY(Initialized)

  /**
  * @brief Constructor
  */
  VSAbstractFilter();

  /**
  * @brief Deconstructor
  */
  virtual ~VSAbstractFilter();

  /**
  * @brief Deletes the item and removes it from the model
  */
  virtual void deleteFilter();

  /**
  * @brief Returns the parent visual filter
  * @return
  */
  VSAbstractFilter* getParentFilter() const;

  /**
  * @brief Sets the parent visual filter and sets up the input and output connections
  * @param parent
  */
  void setParentFilter(VSAbstractFilter* parent);
  
  /**
  * @brief Returns the highest-level ancestor
  * @return
  */
  VSAbstractFilter* getAncestor();

  /**
  * @brief Returns a vector of all children filters
  * @return
  */
  QVector<VSAbstractFilter*> getChildren() const;

  /**
  * @brief Returns the child at the given index
  * @param index
  * @return
  */
  VSAbstractFilter* getChild(int index) const;

  /**
  * @brief Returns the index of the given child filter
  * @param child
  * @return
  */
  int getIndexOfChild(VSAbstractFilter* child) const;

  /**
  * @brief Returns a vector of all descendant filters
  * @return
  */
  QVector<VSAbstractFilter*> getDescendants() const;

  /**
  * @brief Returns a list of array names
  * @return
  */
  QStringList getArrayNames();

  /**
  * @brief Returns a list of component names
  * @param arrayName
  * @return
  */
  QStringList getComponentList(QString arrayName);

  /**
  * @brief Returns a list of component names
  * @param arrayIndex
  * @return
  */
  QStringList getComponentList(int arrayIndex);

  /**
  * @brief Returns the bounds for the filter
  */
  virtual double* getBounds() const;

  /**
  * @brief Returns the bounds from the vtkTransformFilter
  * @return
  */
  virtual double* getTransformBounds();

  /**
  * @brief Returns the output port for the filter
  * @return
  */
  virtual vtkAlgorithmOutput* getOutputPort() = 0;

  /**
  * @brief Returns the output data for the filter
  */
  virtual VTK_PTR(vtkDataSet) getOutput() = 0;

  /**
  * @brief Returns the outline data port for the filter
  * @return
  */
  vtkAlgorithmOutput* getOutlinePort();

  /**
  * @brief Returns the output port for the transformed filtered data
  * @return
  */
  virtual vtkAlgorithmOutput* getTransformedOutputPort();

  /**
  * @brief Returns the transformed output data
  * @return
  */
  virtual VTK_PTR(vtkDataSet) getTransformedOutput();

  /**
  * @brief Returns the filter name
  * @return
  */
  virtual const QString getFilterName() = 0;

  /**
  * @brief Returns the tooltip to use for the filter
  * @return
  */
  virtual QString getToolTip() const = 0;

  /**
  * @brief Save the vtkDataSet output to a file
  * @param fileName
  */
  void saveFile(QString fileName);

  /**
  * @brief Returns the output dataType_t value
  * @return
  */
  virtual dataType_t getOutputType();

  /**
  * @brief Returns true if the input dataType_t is compatible with a given required type
  * @param inputType
  * @param requiredType
  * @return
  */
  static bool compatibleInput(dataType_t inputType, dataType_t requiredType);

  /**
  * @brief Returns a pointer to the object's transform
  * @return
  */
  VSTransform* getTransform();

  /**
  * @brief Writes values to a json file from the filter
  * @param json
  */
  virtual void writeJson(QJsonObject &json);

signals:
  void updatedOutputPort(VSAbstractFilter* filter);
  void transformChanged();

protected slots:
  /**
  * @brief Updates input connection to match the given filter.  If this filter is not
  * connected to its own algorithm, it propogates the update signal to its children.
  * @param filter
  */
  void connectToOutput(VSAbstractFilter* filter);

  /**
  * @brief Forms connections with additional output filters
  * @param filter
  */
  void connectAdditionalOutputFilters(VSAbstractFilter* filter);

  /**
  * @brief Updates the transform used by the transform filter
  */
  void updateTransformFilter();

protected:
  /**
  * @brief code to setup the vtkAlgorithm for the filter
  */
  virtual void createFilter() = 0;

  /**
  * @brief Creates the vtkTransformFilter for output
  */
  void createTransformFilter();

  /**
  * @brief Returns the vtkTransformFilter used
  */
  VTK_PTR(vtkTransformFilter) getTransformFilter();

  /*
  * @brief Returns a pointer to the VSAbstractDataFilter that stores the input vtkDataSet
  * @return
  */
  VSAbstractDataFilter* getDataSetFilter();
  
  /**
  * @brief Updates the input connection for the vtkAlgorithm if that was already setup
  * @param filter
  */
  virtual void updateAlgorithmInput(VSAbstractFilter* filter) = 0;

  /**
  * @brief Returns a list of component names for a given vtkAbstractArray
  * @param array
  * @return
  */
  QStringList getComponentList(vtkAbstractArray* array);

  /**
  * @brief Reads the transformation data from json and applies it to the filter
  * @param json
  */
  void readTransformJson(QJsonObject& json);

  bool m_ConnectedInput = false;
  VTK_PTR(vtkAlgorithmOutput) m_InputPort;

private:
  /**
  * @brief Adds a child VSAbstractFilter
  * @param child
  */
  void addChild(VSAbstractFilter* child);

  /**
  * @brief Remove a child VSAbstractFilter
  * @param child
  */
  void removeChild(VSAbstractFilter* child);

  std::shared_ptr<VSTransform> m_Transform;
  VTK_PTR(vtkTransformFilter) m_TransformFilter;
  VTK_PTR(vtkOutlineFilter) m_OutlineFilter;
  //VTK_PTR(vtkTransformFilter) m_OutlineTransformFilter;
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif
