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

#include "VSQueueModel.h"

#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#include "QtWidgets/VSQueueItem.h"

#include "SIMPLVtkLib/Dialogs/Utilities/ImporterWorker.h"

VSQueueModel* VSQueueModel::self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueModel::VSQueueModel(QObject* parent)
: QAbstractItemModel(parent)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueModel::~VSQueueModel()
{
  delete rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::initialize()
{
  qRegisterMetaType<VSQueueModel::QueueState>();

  rootItem = new VSQueueItem("");

  m_ImportDataWorker = new ImporterWorker();
  m_ImportDataWorker->setQueueModel(this);

  m_ImportDataWorkerThread = new QThread;
  m_ImportDataWorker->moveToThread(m_ImportDataWorkerThread);
  connect(m_ImportDataWorkerThread, SIGNAL(started()), m_ImportDataWorker, SLOT(process()));
  connect(m_ImportDataWorker, SIGNAL(finished()), m_ImportDataWorkerThread, SLOT(quit()));
  connect(m_ImportDataWorkerThread, SIGNAL(finished()), this, SLOT(handleDataQueueFinished()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::startQueue()
{
  setQueueState(VSQueueModel::QueueState::Executing);
  m_ImportDataWorkerThread->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::cancelQueue()
{
  setQueueState(VSQueueModel::QueueState::Canceling);
  m_ImportDataWorker->cancelWorker();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::handleDataQueueFinished()
{
  setQueueState(VSQueueModel::QueueState::Idle);
  emit queueFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::setQueueState(QueueState queueState)
{
  m_QueueState = queueState;
  emit queueStateChanged(queueState);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSQueueModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant VSQueueModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  VSQueueItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    return item->getName();
  }
  if(role == static_cast<int>(Roles::ImporterRole))
  {
    QVariant var;
    var.setValue(item->getImporter());
    return var;
  }
  else if(role == Qt::ForegroundRole)
  {
//    if(item->getHasErrors())
//    {
//      return styles->getQTreeViewItem_error_color();
//    }

//      return styles->getQTreeViewItem_color();
  }
  else if(role == Qt::ToolTipRole)
  {
//    QString path = item->getPath();
//    QFileInfo info(path);
//    if(!path.isEmpty())
//    {
//      if(!info.exists())
//      {
//        QString tooltip = tr("'%1' was not found on the file system.\nYou can either locate the file or delete the entry from the table.").arg(item->getPath());
//        return tooltip;
//      }
//      if(info.suffix().compare("json") == 0)
//      {
//        QString html = JsonFilterParametersReader::HtmlSummaryFromFile(path, nullptr);
//        return html;
//      }
//    }

//    return QVariant();
  }
  else if(role == Qt::DecorationRole)
  {
    VSAbstractImporter::Pointer importer = item->getImporter();
    if (importer)
    {
      if (importer->getState() == VSAbstractImporter::State::Ready)
      {
        return QIcon(":/SIMPL/icons/images/bullet_ball_blue.png");
      }
      else if (importer->getState() == VSAbstractImporter::State::Executing)
      {
        return QIcon(":/SIMPL/icons/images/bullet_ball_yellow.png");
      }
      else if (importer->getState() == VSAbstractImporter::State::Finished)
      {
        return QIcon(":/SIMPL/icons/images/bullet_ball_green.png");
      }
      else if (importer->getState() == VSAbstractImporter::State::Canceled)
      {
        return QIcon(":/SIMPL/icons/images/bullet_ball_red.png");
      }
    }
    return QVariant();
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags VSQueueModel::flags(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return nullptr;
  }

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

  return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueItem* VSQueueModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    VSQueueItem* item = static_cast<VSQueueItem*>(index.internalPointer());
    if(item != nullptr)
    {
      return item;
    }
  }
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSQueueModel::index(int row, int column, const QModelIndex& parent) const
{
  if(parent.isValid() && parent.column() != 0)
  {
    return QModelIndex();
  }

  VSQueueItem* parentItem = getItem(parent);

  VSQueueItem* childItem = parentItem->child(row);
  if(childItem != nullptr)
  {
    return createIndex(row, column, childItem);
  }

    return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQueueModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  VSQueueItem* parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, columnCount());
  endInsertRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQueueModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  VSQueueItem* parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQueueModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
  beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

  VSQueueItem* srcParentItem = getItem(sourceParent);
  VSQueueItem* destParentItem = getItem(destinationParent);

  for(int i = sourceRow; i < sourceRow + count; i++)
  {
    QModelIndex srcIndex = index(i, VSQueueItem::Contents, sourceParent);
    VSQueueItem* srcItem = getItem(srcIndex);

    destParentItem->insertChild(destinationChild, srcItem);
    srcItem->setParent(destParentItem);
    srcParentItem->removeChild(i);
  }

  endMoveRows();

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSQueueModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return QModelIndex();
  }

  VSQueueItem* childItem = getItem(index);
  VSQueueItem* parentItem = childItem->parent();

  if(parentItem == rootItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSQueueModel::rowCount(const QModelIndex& parent) const
{
  VSQueueItem* parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQueueModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  VSQueueItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    item->setName(value.toString());
  }
  else if(role == static_cast<int>(Roles::ImporterRole))
  {
    VSAbstractImporter::Pointer oldImporter = item->getImporter();
    if (oldImporter)
    {
      disconnect(oldImporter.get(), &VSAbstractImporter::notifyMessage, 0, 0);
    }

    VSAbstractImporter::Pointer importer = value.value<VSAbstractImporter::Pointer>();
    connect(importer.get(), &VSAbstractImporter::stateChanged, this, &VSQueueModel::importerStateChanged);
    connect(importer.get(), &VSAbstractImporter::notifyMessage, [=] (const AbstractMessage::Pointer& msg) { emit notifyImporterMessage(importer, msg); });
    item->setImporter(importer);
  }
  else if(role == Qt::ToolTipRole)
  {
    item->setItemTooltip(value.toString());
  }
  else
  {
    return false;
  }

  emit dataChanged(index, index, QVector<int>(1, role));

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::addImporter(const QString &name, VSAbstractImporter::Pointer importer, QIcon icon)
{
  int row = rowCount();
  insertImporter(row, name, importer, icon);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::insertImporter(int row, const QString &name, VSAbstractImporter::Pointer importer, QIcon icon)
{
  insertRow(row);
  QModelIndex index = this->index(row, VSQueueItem::ItemData::Contents);

  setData(index, name, Qt::DisplayRole);

  QVariant var;
  var.setValue(importer);
  setData(index, var, VSQueueModel::Roles::ImporterRole);

  setData(index, QIcon(":/SIMPL/icons/images/bullet_ball_blue.png"), Qt::DecorationRole);

  connect(importer.get(), &VSAbstractImporter::stateChanged, this, [=] {
    QModelIndex newIndex = indexOfImporter(importer);
    emit dataChanged(newIndex, newIndex, QVector<int>(1, VSQueueModel::Roles::ImporterRole));
 });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::removeImporter(VSAbstractImporter::Pointer importer)
{
  for (int i = 0; i < rowCount(); i++)
  {
    QModelIndex index = this->index(i, VSQueueItem::ItemData::Contents);
    VSAbstractImporter::Pointer internalImporter = data(index, Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
    if (internalImporter == importer)
    {
      disconnect(importer.get(), &VSAbstractImporter::stateChanged, 0, 0);
      removeRow(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueModel::clearAll()
{
  while (rowCount() > 0)
  {
    QModelIndex index = this->index(0, VSQueueItem::ItemData::Contents);
    VSQueueItem* item = getItem(index);
    VSAbstractImporter::Pointer importer = item->getImporter();
    if (importer->getState() == VSAbstractImporter::State::Executing)
    {
      importer->cancel();
    }

    removeRow(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSQueueModel::indexOfImporter(VSAbstractImporter::Pointer importer)
{
  for (int i = 0; i < rowCount(); i++)
  {
    QModelIndex index = this->index(i, VSQueueItem::ItemData::Contents);
    VSAbstractImporter::Pointer internalImporter = data(index, Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
    if (internalImporter == importer)
    {
      return index;
    }
  }
  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueItem* VSQueueModel::getRootItem()
{
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQueueModel::isEmpty()
{
  return rowCount(QModelIndex()) <= 0;
}
