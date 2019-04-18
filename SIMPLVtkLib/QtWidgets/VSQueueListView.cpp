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

#include "VSQueueListView.h"

#include <QtGui/QMouseEvent>

#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#include "QtWidgets/VSQueueItem.h"
#include "QtWidgets/VSQueueModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueListView::VSQueueListView(QWidget* parent)
: QListView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &VSQueueListView::customContextMenuRequested, this, &VSQueueListView::requestContextMenu);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueListView::~VSQueueListView() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueListView::mousePressEvent(QMouseEvent* event)
{
  clearSelection();
  QListView::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueListView::requestContextMenu(const QPoint& pos)
{
  activateWindow();

  QModelIndex index = indexAt(pos);

  QPoint mapped;
  if(index.isValid())
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    mapped = viewport()->mapToGlobal(pos);
  }
  else
  {
    index = QModelIndex();
    mapped = mapToGlobal(pos);
  }

  QModelIndexList indexList = selectionModel()->selectedRows(VSQueueItem::ItemData::Contents);
  qSort(indexList);

//  VSQueueModel* model = VSQueueModel::Instance();

  QMenu menu;
  if(index.isValid())
  {
    menu.addAction("Stop Import", [=] { stopImporter(index); });

    {
      QAction* separator = new QAction(this);
      separator->setSeparator(true);
      menu.addAction(separator);
    }

    menu.exec(mapped);

//    menu.addAction("Reset Import", [=] { resetImporter(index); });
  }

//  {
//    QAction* separator = new QAction(this);
//    separator->setSeparator(true);
//    menu.addAction(separator);
//  }
//  if(index.isValid())
//  {
//    menu.addAction("Clear Selected Import", [=] { clearImporter(index); });
//  }
//  menu.addAction("Clear All Imports", [=] { handleClearAllImports(); });
//  menu.addAction("Clear Completed Imports", [=] { handleClearCompletedImports(); });

//  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueListView::handleClearCompletedImports()
{
  clearCompletedImports();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueListView::clearCompletedImports()
{
  VSQueueModel* model = getQueueModel();
  for(int row = 0; row < model->rowCount(); row++)
  {
    QPersistentModelIndex index = model->index(row, VSQueueItem::ItemData::Contents);
    VSAbstractImporter::Pointer importer = model->data(index, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
    if(importer->getState() == VSAbstractImporter::State::Finished)
    {
      model->removeImporter(importer);
      row--;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueListView::handleClearAllImports()
{
  clearAllImports();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueListView::clearAllImports()
{
  VSQueueModel* model = getQueueModel();
  for(int row = 0; row < model->rowCount(); row++)
  {
    QPersistentModelIndex index = model->index(row, VSQueueItem::ItemData::Contents);
    if (clearImporter(index))
    {
      row--;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQueueListView::clearImporter(const QModelIndex &index)
{
  VSQueueModel* model = getQueueModel();

  VSAbstractImporter::Pointer importer = model->data(index, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
  if(importer->getState() == VSAbstractImporter::State::Executing)
  {
    QString name = model->data(index, Qt::DisplayRole).toString();
    QMessageBox::StandardButton selection = QMessageBox::warning(this, "Clear Importer", tr("Item '%1' is currently importing.  Are you sure that you want to clear it?").arg(name),
                                                                 QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
    if (selection == QMessageBox::StandardButton::No)
    {
      return false;
    }

    importer->cancel();
  }

  model->removeImporter(importer);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueListView::stopImporter(const QModelIndex &index)
{
  VSQueueModel* model = getQueueModel();

  VSAbstractImporter::Pointer importer = model->data(index, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
  if(importer->getState() == VSAbstractImporter::State::Executing)
  {
//    QString name = model->data(index, Qt::DisplayRole).toString();
//    QMessageBox::StandardButton selection = QMessageBox::warning(this, "Cancel Importer", tr("Item '%1' is currently importing.  Are you sure that you want to reset it?").arg(name),
//                                                                 QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
//    if (selection == QMessageBox::StandardButton::No)
//    {
//      return;
//    }

    importer->cancel();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueListView::resetImporter(const QModelIndex &index)
{
  VSQueueModel* model = getQueueModel();

  VSAbstractImporter::Pointer importer = model->data(index, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
  if(importer->getState() == VSAbstractImporter::State::Executing)
  {
    QString name = model->data(index, Qt::DisplayRole).toString();
    QMessageBox::StandardButton selection = QMessageBox::warning(this, "Reset Importer", tr("Item '%1' is currently importing.  Are you sure that you want to reset it?").arg(name),
                                                                 QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
    if (selection == QMessageBox::StandardButton::No)
    {
      return;
    }
  }

  importer->reset();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueModel* VSQueueListView::getQueueModel()
{
  return dynamic_cast<VSQueueModel*>(model());
}
