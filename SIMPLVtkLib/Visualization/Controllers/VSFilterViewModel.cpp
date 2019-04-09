/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include "VSFilterViewModel.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "vtkActor.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewModel::VSFilterViewModel(QObject* parent)
: QAbstractItemModel(parent)
{
  m_FilterModel = new VSFilterModel(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewModel::VSFilterViewModel(const VSFilterViewModel& model)
: QAbstractItemModel(model.parentObject())
{
  deepCopy(model);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QObject* VSFilterViewModel::parentObject() const
{
  return QObject::parent();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::setFilterModel(VSFilterModel* filterModel)
{
  if(m_FilterModel)
  {
    disconnect(m_FilterModel, &VSFilterModel::columnsAboutToBeInserted, this, &VSFilterViewModel::columnsAboutToBeInserted);
    disconnect(m_FilterModel, &VSFilterModel::columnsAboutToBeMoved, this, &VSFilterViewModel::columnsAboutToBeMoved);
    disconnect(m_FilterModel, &VSFilterModel::columnsAboutToBeRemoved, this, &VSFilterViewModel::columnsAboutToBeRemoved);
    disconnect(m_FilterModel, &VSFilterModel::columnsInserted, this, &VSFilterViewModel::columnsInserted);
    disconnect(m_FilterModel, &VSFilterModel::columnsMoved, this, &VSFilterViewModel::columnsMoved);
    disconnect(m_FilterModel, &VSFilterModel::columnsRemoved, this, &VSFilterViewModel::columnsRemoved);

    disconnect(m_FilterModel, &VSFilterModel::dataChanged, this, &VSFilterViewModel::dataChanged);
    disconnect(m_FilterModel, &VSFilterModel::headerDataChanged, this, &VSFilterViewModel::headerDataChanged);
    disconnect(m_FilterModel, &VSFilterModel::layoutAboutToBeChanged, this, &VSFilterViewModel::layoutAboutToBeChanged);
    disconnect(m_FilterModel, &VSFilterModel::layoutChanged, this, &VSFilterViewModel::layoutChanged);
    disconnect(m_FilterModel, &VSFilterModel::modelAboutToBeReset, this, &VSFilterViewModel::modelAboutToBeReset);
    disconnect(m_FilterModel, &VSFilterModel::modelReset, this, &VSFilterViewModel::modelReset);

    disconnect(m_FilterModel, &VSFilterModel::rowsAboutToBeInserted, this, &VSFilterViewModel::rowsAboutToBeInserted);
    disconnect(m_FilterModel, &VSFilterModel::rowsAboutToBeMoved, this, &VSFilterViewModel::rowsAboutToBeMoved);
    disconnect(m_FilterModel, &VSFilterModel::rowsAboutToBeRemoved, this, &VSFilterViewModel::rowsAboutToBeRemoved);
    disconnect(m_FilterModel, &VSFilterModel::rowsInserted, this, &VSFilterViewModel::rowsInserted);
    disconnect(m_FilterModel, &VSFilterModel::rowsMoved, this, &VSFilterViewModel::rowsMoved);
    disconnect(m_FilterModel, &VSFilterModel::rowsRemoved, this, &VSFilterViewModel::rowsRemoved);

    disconnect(m_FilterModel, &VSFilterModel::filterAdded, this, &VSFilterViewModel::createFilterViewSettings);
    disconnect(m_FilterModel, &VSFilterModel::filterRemoved, this, &VSFilterViewModel::removeFilterViewSettings);
  }

  m_FilterModel = filterModel;

  clearFilterViewSettings();
  if(filterModel)
  {
    connect(filterModel, &VSFilterModel::dataChanged, this, &VSFilterViewModel::dataChanged);
    connect(filterModel, &VSFilterModel::headerDataChanged, this, &VSFilterViewModel::headerDataChanged);
    connect(filterModel, &VSFilterModel::layoutAboutToBeChanged, this, &VSFilterViewModel::layoutAboutToBeChanged);
    connect(filterModel, &VSFilterModel::layoutChanged, this, &VSFilterViewModel::layoutChanged);
    connect(filterModel, &VSFilterModel::modelAboutToBeReset, this, &VSFilterViewModel::modelAboutToBeReset);
    connect(filterModel, &VSFilterModel::modelReset, this, &VSFilterViewModel::modelReset);

    connect(filterModel, &VSFilterModel::rowsInserted, this, &VSFilterViewModel::rowsInserted);
    connect(filterModel, &VSFilterModel::rowsMoved, this, &VSFilterViewModel::rowsMoved);
    connect(filterModel, &VSFilterModel::rowsRemoved, this, &VSFilterViewModel::rowsRemoved);

    connect(filterModel, &VSFilterModel::filterAdded, this, &VSFilterViewModel::createFilterViewSettings);
    connect(filterModel, &VSFilterModel::filterRemoved, this, &VSFilterViewModel::removeFilterViewSettings);

    connect(filterModel, &VSFilterModel::beganInsertingFilter, this, &VSFilterViewModel::beginInsertingFilter);
    connect(filterModel, &VSFilterModel::beganRemovingFilter, this, &VSFilterViewModel::beginRemovingFilter);
    connect(filterModel, &VSFilterModel::finishedInsertingFilter, this, &VSFilterViewModel::finishInsertingFilter);
    connect(filterModel, &VSFilterModel::finishedRemovingFilter, this, &VSFilterViewModel::finishRemovingFilter);

    std::list<VSAbstractFilter*> filterList = filterModel->getAllFilters();
    for(VSAbstractFilter* filter : filterList)
    {
      beginInsertingFilter(filter);
      createFilterViewSettings(filter);
      finishInsertingFilter();
    }
  }

  emit rootChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::beginInsertingFilter(VSAbstractFilter* parentFilter)
{
  if(nullptr == parentFilter || nullptr == m_FilterModel)
  {
    return;
  }

  QModelIndex parentIndex = getIndexFromFilter(parentFilter);
  int position = parentFilter->getChildCount();
  beginInsertRows(parentIndex, position, position);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::beginRemovingFilter(VSAbstractFilter* filter, int row)
{
  if(nullptr == filter || row < 0 || nullptr == m_FilterModel)
  {
    return;
  }

  QModelIndex index = getIndexFromFilter(filter);
  beginRemoveRows(index, row, row);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::finishInsertingFilter()
{
  endInsertRows();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::finishRemovingFilter()
{
  endRemoveRows();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::deepCopy(const VSFilterViewModel& model)
{
  if(nullptr != model.m_FilterModel)
  {
    setFilterModel(model.m_FilterModel);

    std::vector<VSFilterViewSettings*> allFilterViewSettings = model.getAllFilterViewSettings();
    for(VSFilterViewSettings* filterViewSettings : allFilterViewSettings)
    {
      if(m_FilterViewSettings.find(filterViewSettings->getFilter()) == m_FilterViewSettings.end())
      {
        m_FilterViewSettings[filterViewSettings->getFilter()] = new VSFilterViewSettings(*filterViewSettings);
      }
      else
      {
        m_FilterViewSettings[filterViewSettings->getFilter()]->deepCopy(filterViewSettings);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSFilterViewModel::getFilterViewSettings(VSAbstractFilter* filter) const
{
  if(nullptr == filter)
  {
    return nullptr;
  }

  VSFilterViewSettings* settings = m_FilterViewSettings[filter];
  if(nullptr == settings)
  {
    settings = createFilterViewSettings(filter);
  }

  return settings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Collection VSFilterViewModel::getFilterViewSettings(VSAbstractFilter::FilterListType filters) const
{
  VSFilterViewSettings::Collection collection;

  for(VSAbstractFilter* filter : filters)
  {
    VSFilterViewSettings* settings = getFilterViewSettings(filter);
    if(nullptr != settings)
    {
      collection.push_back(settings);
    }
  }

  return collection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSFilterViewModel::getFilterViewSettingsByIndex(const QModelIndex& index) const
{
  VSAbstractFilter* targetFilter = getFilterFromIndex(index);
  return getFilterViewSettings(targetFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Map VSFilterViewModel::getFilterViewSettingsMap() const
{
  return m_FilterViewSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<VSFilterViewSettings*> VSFilterViewModel::getAllFilterViewSettings() const
{
  std::vector<VSFilterViewSettings*> viewSettings(m_FilterViewSettings.size());

  int i = 0;
  for(auto iter = m_FilterViewSettings.begin(); iter != m_FilterViewSettings.end(); iter++, i++)
  {
    viewSettings[i] = iter->second;
  }

  return viewSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSFilterViewModel::createFilterViewSettings(VSAbstractFilter* filter) const
{
  if(nullptr == filter)
  {
    return nullptr;
  }

  // Do not overwrite filter view settings that already exist
  if(nullptr != m_FilterViewSettings[filter])
  {
    return m_FilterViewSettings[filter];
  }

  static int numImages = 0;
  static int imageIndex = 0;

  VSFilterViewSettings* viewSettings;
  if(m_DisplayType == AbstractImportMontageDialog::DisplayType::NotSpecified)
  {
	  viewSettings = new VSFilterViewSettings(filter);
  }
  else
  {
	  VSFilterViewSettings::Representation representation = VSFilterViewSettings::Representation::Outline;
    if(m_DisplayType == AbstractImportMontageDialog::DisplayType::SideBySide || m_DisplayType == AbstractImportMontageDialog::DisplayType::Montage)
    {
		  representation = VSFilterViewSettings::Representation::Surface;
	  }
	  viewSettings = new VSFilterViewSettings(filter, representation, m_DisplayType);
  }

  //connect(filter, &VSAbstractFilter::removeFilter, this, [=] { removeFilterViewSettings(filter); });
  //connect(viewSettings, &VSFilterViewSettings::visibilityChanged, this, [=] { filterVisibilityChanged(); });

  m_FilterViewSettings[filter] = viewSettings;

  if(filter->getParentFilter() && filter->getParentFilter()->getOutput())
  {
    VSFilterViewSettings* parentSettings = getFilterViewSettings(filter->getParentFilter());
    if(parentSettings)
    {
      viewSettings->copySettings(parentSettings);
    }
  }

  emit viewSettingsCreated(viewSettings);
  return viewSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::clearFilterViewSettings()
{
  for(auto iter : m_FilterViewSettings)
  {
    emit viewSettingsRemoved(iter.second);
    iter.second->deleteLater();
  }

  m_FilterViewSettings.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::removeFilterViewSettings(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }
  if(m_FilterViewSettings.find(filter) == m_FilterViewSettings.end())
  {
    return;
  }

  VSFilterViewSettings* viewSettings = m_FilterViewSettings[filter];
  if(viewSettings)
  {
    viewSettings->setVisible(false);

    emit viewSettingsRemoved(viewSettings);
    m_FilterViewSettings.erase(filter);
    viewSettings->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags VSFilterViewModel::flags(const QModelIndex& index) const
{
  VSAbstractFilter* filter = getFilterFromIndex(index);
  if(filter)
  {
    return filter->flags();
  }

  return Qt::ItemFlags();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewModel::rowCount(const QModelIndex& parent) const
{
  if(m_FilterModel)
  {
    return m_FilterModel->rowCount(parent);
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSFilterViewModel::parent(const QModelIndex& index) const
{
  if(m_FilterModel)
  {
    if(!index.isValid())
    {
      return QModelIndex();
    }

    VSAbstractFilter* filter = getFilterFromIndex(index);
    if(!filter || filter == m_FilterModel->getRootFilter())
    {
      return QModelIndex();
    }

    VSAbstractFilter* parentFilter = filter->getParentFilter();
    if(parentFilter) // && parentFilter != m_RootFilter)
    {
      return createIndex(parentFilter->getChildIndex(), 0, parentFilter);
    }
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSFilterViewModel::index(int row, int column, const QModelIndex& parent) const
{
  if(m_FilterModel)
  {
    // column must be 0 for valid indices in this model
    if(column != 0)
    {
      return QModelIndex();
    }
    if(parent.isValid() && parent.column() != 0)
    {
      return QModelIndex();
    }

    // Root index
    if(!parent.isValid() && row == 0)
    {
      return createIndex(row, column, m_FilterModel->getRootFilter());
    }

    // Valid parent
    if(parent.isValid())
    {
      VSAbstractFilter* parentFilter = getFilterFromIndex(parent);
      if(parentFilter && row < parentFilter->getChildren().size())
      {
        return createIndex(row, column, parentFilter->getChild(row));
      }
    }
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSFilterViewModel::getBaseFilters() const
{
  if(m_FilterModel)
  {
    return m_FilterModel->getBaseFilters();
  }

  return VSAbstractFilter::FilterListType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSFilterViewModel::getAllFilters() const
{
  if(m_FilterModel)
  {
    return m_FilterModel->getAllFilters();
  }

  return VSAbstractFilter::FilterListType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterViewModel::getFilterFromIndex(const QModelIndex& index) const
{
  if(m_FilterModel)
  {
    return m_FilterModel->getFilterFromIndex(index);
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSFilterViewModel::getIndexFromFilter(VSAbstractFilter* filter) const
{
  if(filter == nullptr)
  {
    return QModelIndex();
  }

  return createIndex(filter->getChildIndex(), 0, filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSFilterViewModel::getFiltersFromIndexes(const QModelIndexList& indexes) const
{
  VSAbstractFilter::FilterListType filterList;
  for(QModelIndex index : indexes)
  {
    VSAbstractFilter* filter = getFilterFromIndex(index);
    if(filter)
    {
      filterList.push_back(filter);
    }
  }

  return filterList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList VSFilterViewModel::getIndexesFromFilters(VSAbstractFilter::FilterListType filters) const
{
  QModelIndexList indexes;
  for(VSAbstractFilter* filter : filters)
  {
    indexes.push_back(getIndexFromFilter(filter));
  }

  return indexes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant VSFilterViewModel::getFilterCheckState(const QModelIndex& index) const
{
  VSFilterViewSettings* targetSettings = getFilterViewSettingsByIndex(index);
  if(!targetSettings)
  {
    return QVariant();
  }

  return targetSettings->isVisible() ? Qt::Checked : Qt::Unchecked;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewModel::setFilterCheckState(const QModelIndex& index, QVariant value)
{
  Qt::CheckState checkState = value.value<Qt::CheckState>();

  // Get Target Filter
  VSAbstractFilter* targetFilter = getFilterFromIndex(index);
  if(!targetFilter)
  {
    return false;
  }

  // Get View Settings
  VSFilterViewSettings* targetSettings = (m_FilterViewSettings.find(targetFilter))->second;
  if(targetSettings)
  {
    targetSettings->setVisible(checkState == Qt::Checked);
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant VSFilterViewModel::data(const QModelIndex& index, int role) const
{
  if(m_FilterModel)
  {
    if(role == Qt::CheckStateRole)
    {
      return getFilterCheckState(index);
    }
    else
    {
      return m_FilterModel->data(index, role);
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if(m_FilterModel)
  {
    if(role == Qt::CheckStateRole)
    {
      return setFilterCheckState(index, value);
    }

    return m_FilterModel->setData(index, value, role);
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSFilterViewModel::rootIndex() const
{
  if(m_FilterModel)
  {
    return createIndex(0, 0, m_FilterModel->getRootFilter());
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSRootFilter* VSFilterViewModel::getRootFilter() const
{
  if(m_FilterModel)
  {
    return m_FilterModel->getRootFilter();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFilterViewModel::getFilterText(const QModelIndex& index) const
{
  if(m_FilterModel)
  {
    return m_FilterModel->getFilterText(index);
  }

  return "Undefined";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFont VSFilterViewModel::getFilterFont(const QModelIndex& index) const
{
  if(m_FilterModel)
  {
    return m_FilterModel->getFilterFont(index);
  }

  return QFont();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewModel::getFilterCheckable(const QModelIndex& index) const
{
  VSAbstractFilter* targetFilter = getFilterFromIndex(index);
  if(targetFilter)
  {
    return targetFilter->isCheckable();
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::filterVisibilityChanged()
{
  VSFilterViewSettings* viewSettings = dynamic_cast<VSFilterViewSettings*>(sender());
  if(nullptr == viewSettings)
  {
    return;
  }

  QVector<int> roles(Qt::CheckStateRole);
  QModelIndex index = getIndexFromFilter(viewSettings->getFilter());
  emit dataChanged(index, index, roles);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList VSFilterViewModel::convertIndicesToFilterModel(const QModelIndexList& indices) const
{
  QModelIndexList filterModelIndices;
  for(QModelIndex localIndex : indices)
  {
    VSAbstractFilter* filter = getFilterFromIndex(localIndex);
    if(filter)
    {
      QModelIndex targetIndex = m_FilterModel->getIndexFromFilter(filter);
      filterModelIndices.push_back(targetIndex);
    }
  }

  return filterModelIndices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList VSFilterViewModel::convertIndicesFromFilterModel(const QModelIndexList& indices) const
{
  QModelIndexList localModelIndices;
  for(QModelIndex filterIndex : indices)
  {
    VSAbstractFilter* filter = m_FilterModel->getFilterFromIndex(filterIndex);
    if(filter)
    {
      QModelIndex targetIndex = this->getIndexFromFilter(filter);
      localModelIndices.push_back(targetIndex);
    }
  }

  return localModelIndices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewModel::setDisplayType(AbstractImportMontageDialog::DisplayType displayType)
{
	m_DisplayType = displayType;
}