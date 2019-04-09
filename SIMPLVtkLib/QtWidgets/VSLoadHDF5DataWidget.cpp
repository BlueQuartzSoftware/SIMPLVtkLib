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

#include "VSLoadHDF5DataWidget.h"

#include <QtConcurrent>

#include <QtGui/QMovie>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/Dialogs/Utilities/DREAM3DFileTreeModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSLoadHDF5DataWidget::VSLoadHDF5DataWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSLoadHDF5DataWidget)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSLoadHDF5DataWidget::~VSLoadHDF5DataWidget()
{
  delete m_LoadingMovie;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLoadHDF5DataWidget::setupGui()
{
  DREAM3DFileTreeModel* model = new DREAM3DFileTreeModel();
  connect(model, &DREAM3DFileTreeModel::dataChanged, this, &VSLoadHDF5DataWidget::modelDataChanged);

  connect(m_Ui->selectAllCB, &QCheckBox::stateChanged, this, &VSLoadHDF5DataWidget::selectAllStateChanged);

  m_Ui->treeView->setModel(model);

  m_LoadingMovie = new QMovie(":/gifs/loading.gif");
  m_Ui->loadingLabel->setMovie(m_LoadingMovie);

  connect(&m_ProxyInitWatcher, SIGNAL(finished()), this, SLOT(proxyInitFinished()));

  connect(m_Ui->loadBtn, &QPushButton::clicked, this, &VSLoadHDF5DataWidget::loadBtnClicked);
  connect(m_Ui->cancelBtn, &QPushButton::clicked, this, &VSLoadHDF5DataWidget::cancelBtnClicked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLoadHDF5DataWidget::initialize(const QString &filePath)
{
  QFileInfo fi(filePath);
  QDateTime modified = fi.lastModified();

  // Only load the proxy if the file path is different or the file has been modified since the last time it was loaded
  if (filePath != m_ProxyFilePath || modified > m_ProxyLastModified)
  {
    m_Ui->errLabel->hide();
    m_Ui->loadBtn->setDisabled(true);

    QAbstractItemModel* model = m_Ui->treeView->model();
    model->removeRows(0, model->rowCount());
    m_Proxy = DataContainerArrayProxy();

    m_Ui->loadingLabel->show();
    m_LoadingMovie->start();

    m_ProxyFilePath = filePath;
    m_ProxyLastModified = modified;

    m_LoadingProxy = true;

    QFuture<DataContainerArrayProxy> future = QtConcurrent::run(this, &VSLoadHDF5DataWidget::readDCAProxy, filePath);
    m_ProxyInitWatcher.setFuture(future);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLoadHDF5DataWidget::proxyInitFinished()
{
  QFuture<DataContainerArrayProxy> future = m_ProxyInitWatcher.future();
  DataContainerArrayProxy proxy = future.result();

  setProxy(proxy);

  m_Ui->loadingLabel->hide();
  m_LoadingMovie->stop();

  m_Ui->loadBtn->setEnabled(true);
  m_LoadingProxy = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLoadHDF5DataWidget::setNavigationButtonsVisibility(bool visible)
{
  m_Ui->loadBtn->setVisible(visible);
  m_Ui->cancelBtn->setVisible(visible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy VSLoadHDF5DataWidget::readDCAProxy(const QString& filePath)
{
  QFileInfo fi(filePath);

  SIMPLH5DataReader reader;
  connect(&reader, &SIMPLH5DataReader::errorGenerated, [=] (const QString &title, const QString &msg, const int &code) {
    m_Ui->errLabel->setText(tr("%1").arg(msg));
    m_Ui->errLabel->show();
  });

  bool success = reader.openFile(filePath);
  if(!success)
  {
    return DataContainerArrayProxy();
  }

  int err = 0;
  SIMPLH5DataReaderRequirements req(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
  DataContainerArrayProxy proxy = reader.readDataContainerArrayStructure(&req, err);
  QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();
  if(dataContainers.isEmpty())
  {
    return DataContainerArrayProxy();
  }

  QStringList dcNames = dataContainers.keys();
  for(int i = 0; i < dcNames.size(); i++)
  {
    QString dcName = dcNames[i];
    DataContainerProxy dcProxy = dataContainers[dcName];

    // We want only data containers with geometries displayed
    if(dcProxy.getDCType() == static_cast<unsigned int>(DataContainer::Type::Unknown))
    {
      dataContainers.remove(dcName);
    }
    else
    {
	  QMap<QString, AttributeMatrixProxy>& attributeMatricies = dcProxy.getAttributeMatricies();
      QStringList amNames = attributeMatricies.keys();
      for(int j = 0; j < amNames.size(); j++)
      {
        QString amName = amNames[j];
        AttributeMatrixProxy amProxy = attributeMatricies[amName];

        // We want only cell attribute matrices displayed
        if(amProxy.getAMType() != AttributeMatrix::Type::Cell)
        {
          attributeMatricies.remove(amName);
          dataContainers[dcName] = dcProxy;
        }
      }
    }
  }

  if(dataContainers.size() <= 0)
  {
    m_Ui->errLabel->setText(tr("Failed to load DREAM3D file '%1' because the file does not contain any data containers with a supported geometry.").arg(filePath));
    m_Ui->errLabel->show();
    return DataContainerArrayProxy();
  }

  return proxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLoadHDF5DataWidget::selectAllStateChanged(int state)
{
  DREAM3DFileTreeModel* model = dynamic_cast<DREAM3DFileTreeModel*>(m_Ui->treeView->model());
  if (model == nullptr)
  {
    return;
  }

  Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
  if(checkState == Qt::PartiallyChecked)
  {
    m_Ui->selectAllCB->setCheckState(Qt::Checked);
    return;
  }

  for(int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex dcIndex = model->index(i, DREAM3DFileItem::Name);
    model->setData(dcIndex, checkState, Qt::CheckStateRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLoadHDF5DataWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>())
{
  DREAM3DFileTreeModel* model = dynamic_cast<DREAM3DFileTreeModel*>(m_Ui->treeView->model());
  if (model == nullptr)
  {
    return;
  }

  bool allChecked = true;
  size_t checkCount = 0;
  Qt::CheckState checkState = Qt::Unchecked;
  size_t rowCount = model->rowCount();
  for(int i = 0; i < rowCount; i++)
  {
    QModelIndex dcIndex = model->index(i, DREAM3DFileItem::Name);
    if(model->getCheckState(dcIndex) == Qt::Checked)
    {
      checkState = Qt::PartiallyChecked;
      checkCount++;
    }
    else
    {
      allChecked = false;
    }
  }

  if(allChecked == true)
  {
    checkState = Qt::Checked;
  }

  m_Ui->selectAllCB->blockSignals(true);
  m_Ui->selectAllCB->setCheckState(checkState);
  m_Ui->selectAllCB->blockSignals(false);

  m_Proxy = model->getModelProxy();

  emit proxyChanged(m_Proxy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSLoadHDF5DataWidget::setProxy(DataContainerArrayProxy proxy)
{
  DREAM3DFileTreeModel* model = static_cast<DREAM3DFileTreeModel*>(m_Ui->treeView->model());
  if(model != nullptr)
  {
    model->populateTreeWithProxy(proxy);
    m_Ui->selectAllCB->setChecked(true);

    size_t checkCount = 0;
    size_t rowCount = model->rowCount();
    for(int i = 0; i < rowCount; i++)
    {
      QModelIndex dcIndex = model->index(i, DREAM3DFileItem::Name);
//      m_Ui->treeView->expand(dcIndex);

      if(model->getCheckState(dcIndex) == Qt::Checked)
      {
        checkCount++;
      }
    }

    m_Proxy = proxy;

    emit proxyChanged(proxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy VSLoadHDF5DataWidget::getProxy()
{
  return m_Proxy;
}
