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
#include <vtkTransformFilter.h>
#include <vtkTrivialProducer.h>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QSemaphore>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QFont>

#include <QtGui/QStandardItemModel>

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTransform.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

class VSAbstractFilterValues;
class VSAbstractDataFilter;
class VSFilterModel;

/**
 * @class VSAbstractFilter VSAbstractFilter.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h
 * @brief This is the base class for all visual filters in SIMPLVtkLib.
 * Classes that inherit from this handle various vtk algorithms for filtering
 * out parts of the vtkDataSet input into the top-level VSSIMPLDataContainerFilter.  Filters
 * can be chained together to be more specific about what kind of data should be
 * shown by pushing the output of a filter as the input for each of its child filters.
 */
class SIMPLVtkLib_EXPORT VSAbstractFilter : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString filterName READ getFilterName NOTIFY filterNameChanged)
  Q_PROPERTY(QStringList arrayNames READ getArrayNames NOTIFY arrayNamesChanged)
  Q_PROPERTY(QStringList scalarNames READ getScalarNames NOTIFY scalarNamesChanged)
  Q_PROPERTY(VSTransform transform READ getTransform)
  Q_PROPERTY(bool fullyImported READ isDataImported NOTIFY dataImported)

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

  enum class FilterType : unsigned char
  {
    Placeholder = 0,
    Filter,
    Data,
    File,
    Pipeline
  };

  using FilterListType = std::list<VSAbstractFilter*>;

  SIMPL_INSTANCE_PROPERTY(QJsonObject, LoadingObject)
  SIMPL_BOOL_PROPERTY(Initialized)

  /**
   * @brief Deconstructor
   */
  virtual ~VSAbstractFilter() = default;

  /**
   * @brief Deletes the item and removes it from the model
   */
  Q_INVOKABLE virtual void deleteFilter();

  /**
   * @brief Returns the item flags for the filter
   * @return
   */
  Qt::ItemFlags flags() const;

  /**
   * @brief Returns true if the filter is checkable. Returns false otherwise.
   * @return
   */
  bool isCheckable() const;

  /**
   * @brief Returns true if the data filter has been imported.  Returns false otherwise.
   * @return
   */
  virtual bool isDataImported() const;

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
   * @brief Returns the index of this filter in its parent's child list
   * @return
   */
  int getChildIndex() const;

  /**
   * @brief Returns the previous sibling from its parent's list of children
   * @return
   */
  VSAbstractFilter* getPrevSibling() const;

  /**
   * @brief Returns the next sibling from its parent's list of children
   * @return
   */
  VSAbstractFilter* getNextSibling() const;

  /**
   * @brief Returns the highest-level ancestor
   * @return
   */
  VSAbstractFilter* getAncestor();

  /**
   * @brief Returns a list of all children filters
   * @return
   */
  FilterListType getChildren() const;

  /**
   * @brief Returns the number of children this filter has
   * @return
   */
  int getChildCount() const;

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
  int getIndexOfChild(const VSAbstractFilter* child) const;

  /**
   * @brief Returns a vector of all descendant filters
   * @return
   */
  FilterListType getDescendants() const;

  /**
   * @brief Returns the vtkDataArray with the given name
   * @param arrayName
   * @return
   */
  VTK_PTR(vtkDataArray) getDataArray(QString arrayName) const;

  /**
   * @brief Returns the range of values for the given array
   * @param arrayName
   * @return
   */
  double* getArrayValueRange(QString arrayName) const;

  /**
   * @brief Returns the minimum value for the given array
   * @param arrayName
   * @return
   */
  Q_INVOKABLE double getArrayMinValue(QString arrayName) const;

  /**
   * @brief Returns the maximum value for the given array
   * @param arrayName
   * @return
   */
  Q_INVOKABLE double getArrayMaxValue(QString arrayName) const;

  /**
   * @brief Returns a list of array names
   * @return
   */
  QStringList getArrayNames();

  /**
   * @brief Returns a list of scalar array names
   * @return
   */
  QStringList getScalarNames();

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
  virtual VTK_PTR(vtkDataSet) getOutput() const = 0;

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
  virtual QString getFilterName() const = 0;

  /**
   * @brief Returns the tooltip to use for the filter
   * @return
   */
  virtual QString getToolTip() const;

  /**
   * @brief Save the vtkDataSet output to a file
   * @param fileName
   */
  void saveFile(QString fileName);

  /**
   * @brief Returns the output dataType_t value
   * @return
   */
  virtual dataType_t getOutputType() const;

  /**
   * @brief Returns true if the resulting data should use point data instead of cell data
   * Returns false otherwise.
   * @return
   */
  bool isPointData() const;

  /**
   * @brief Returns true if the input dataType_t is compatible with a given required type
   * @param inputType
   * @param requiredType
   * @return
   */
  static bool CompatibleInput(dataType_t inputType, dataType_t requiredType);

  /**
   * @brief Returns a pointer to the object's transform
   * @return
   */
  VSTransform* getTransform();

  /**
   * @brief Writes values to a json file from the filter
   * @param json
   */
  virtual void writeJson(QJsonObject& json);

  /**
   * @brief Returns the VSFilterModel that this item belongs to
   * @return
   */
  virtual VSFilterModel* getModel() const;

  /**
   * @brief Returns the QModelIndex for this filter
   * @return
   */
  QModelIndex getIndex();

  /**
   * @brief Convenience method for determining what the filter does
   * @return
   */
  virtual FilterType getFilterType() const = 0;

  /**
   * @brief Returns the CheckState for the filter
   * @return
   */
  bool isChecked();

  /**
   * @brief Sets whether or not the filter is checked
   * @param checked
   */
  void setChecked(bool checked);

  /**
   * @brief Convenience method to return the Qt::CheckState from isChecked()
   * @return
   */
  Qt::CheckState checkState();

  /**
   * @brief Returns the text to display for VSFilterModel
   * @return
   */
  virtual QString getText() const;

  /**
   * @brief Sets the text to display for VSFilterModel
   * @param display
   */
  virtual void setText(QString display);

  /**
   * @brief Sets the filter's tooltip
   * @param tooltip
   */
  virtual void setToolTip(QString tooltip);

  /**
   * @brief Returns the filter's display font
   * @return
   */
  QFont font() const;

  /**
   * @brief Sets the filter's display font
   * @param font
   */
  void setFont(QFont font);

  virtual VSAbstractFilterValues* getValues() = 0;

  //////////////////
  // Filter Lists //
  //////////////////
  static bool HasSameDataFilter(VSAbstractFilter::FilterListType filters);
  static bool SameFilterType(VSAbstractFilter::FilterListType filters);
  static bool HasPointData(VSAbstractFilter::FilterListType filters);

signals:
  void updatedOutputPort(VSAbstractFilter* filter);
  void updatedOutput();
  void transformChanged();
  void errorGenerated(const QString& title, const QString& msg, const int& errorCode);
  void removeFilter();
  void arrayNamesChanged();
  void scalarNamesChanged();
  void dataImported();
  void filterNameChanged();

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
   * @brief Constructor
   */
  VSAbstractFilter();

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
   * @return
   */
  VTK_PTR(vtkTransformFilter) getTransformFilter();

  /*
   * @brief Returns a pointer to the VSAbstractDataFilter that stores the input vtkDataSet
   * @return
   */
  const VSAbstractDataFilter* getDataSetFilter() const;

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

  /**
   * @brief Returns true if the filter algorithm is connected. Returns false otherwise.
   * @return
   */
  bool getConnectedInput() const;

  /**
   * @brief Sets whether or not the filter algorithm is connected.
   * @param connected
   */
  void setConnectedInput(bool connected);

  /**
   * @brief Returns the VTK input port
   * @return
   */
  VTK_PTR(vtkAlgorithmOutput) getInputPort();

  /**
   * @brief Sets the VTK input port
   * @param inputPort
   */
  void setInputPort(VTK_PTR(vtkAlgorithmOutput) inputPort);

  /**
   * @Brief Sets whether or not the filter is checkable
   * @param checkable
   */
  void setCheckable(bool checkable);

  /**
   * @brief Sets whether or not the filter is selectable
   * @param selectable
   */
  void setSelectable(bool selectable);

  /**
   * @brief Returns true if the item is editable. Returns false otherwise.
   * @return
   */
  bool isEditable() const;

  /**
   * @brief Sets the ItemIsEditable flag for the filter.
   * @param editable
   */
  void setEditable(bool editable);

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
  mutable QSemaphore m_ChildLock;
  bool m_ConnectedInput = false;
  VTK_PTR(vtkAlgorithmOutput) m_InputPort;

  std::list<VSAbstractFilter*> m_Children;
  bool m_Checked = false;
  QString m_Tooltip;
  QFont m_Font;
  Qt::ItemFlags m_Flags;
  QString m_DisplayText;
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif
