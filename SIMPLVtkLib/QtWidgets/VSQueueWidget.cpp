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

#include "VSQueueWidget.h"

#include <QtWidgets/QColorDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#include "QtWidgets/VSQueueItem.h"
#include "QtWidgets/VSQueueItemDelegate.h"

#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/Messages/FilterErrorMessage.h"
#include "SIMPLib/Messages/FilterWarningMessage.h"
#include "SIMPLib/Messages/FilterStatusMessage.h"
#include "SIMPLib/Messages/FilterProgressMessage.h"
#include "SIMPLib/Messages/PipelineProgressMessage.h"

/**
 * @brief This message handler is used by VSMontageImporter to process filter messages
 */
class ImporterMessageHandler : public AbstractMessageHandler
{
public:
  explicit ImporterMessageHandler(VSAbstractImporter* importer, VSQueueWidget* queueWidget)
  : m_Importer(importer)
  , m_QueueWidget(queueWidget)
  {
  }

  /**
   * @brief Re-emits error messages received from filters
   */
  void processMessage(const FilterErrorMessage* msg) const override
  {
    //  emit notifyErrorMessage(msg, code);
  }

  /**
   * @brief Re-emits error messages received from filters
   */
  void processMessage(const FilterWarningMessage* msg) const override
  {
    //  emit notifyWarningMessage(msg, code);
  }

  /**
   * @brief Re-emits status messages received from filters
   */
  void processMessage(const FilterStatusMessage* msg) const override
  {
    QString str = msg->generateMessageString();
    m_QueueWidget->m_Ui->progressText->setText(str);
  }

  /**
   * @brief Re-emits progress messages received from filters
   */
  void processMessage(const FilterProgressMessage* msg) const override
  {
    m_QueueWidget->m_Ui->progressBar->setValue(msg->getProgressValue());
    m_QueueWidget->m_Ui->progressLabel->setText(QObject::tr("%1%").arg(msg->getProgressValue()));

    QString name = m_Importer->getName();
    QString importText = QObject::tr("Importing '%1'").arg(name);
    m_QueueWidget->m_Ui->statusLabel->setText(importText);
    importText.append(QObject::tr(": %1%").arg(msg->getProgressValue()));

    emit m_QueueWidget->notifyStatusMessage(importText);
  }

  /**
   * @brief Re-emits progress messages received from the pipeline
   */
  void processMessage(const PipelineProgressMessage* msg) const override
  {
    m_QueueWidget->m_Ui->progressBar->setValue(msg->getProgressValue());
    m_QueueWidget->m_Ui->progressLabel->setText(QObject::tr("%1%").arg(msg->getProgressValue()));

    QString name = m_Importer->getName();
    QString importText = QObject::tr("Importing '%1'").arg(name);
    m_QueueWidget->m_Ui->statusLabel->setText(importText);
    importText.append(QObject::tr(": %1%").arg(msg->getProgressValue()));

    emit m_QueueWidget->notifyStatusMessage(importText);
  }

private:
  VSAbstractImporter* m_Importer = nullptr;
  VSQueueWidget* m_QueueWidget = nullptr;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueWidget::VSQueueWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSQueueWidget)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueWidget::~VSQueueWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::setupGui()
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connectSignalsSlots();

  VSQueueItemDelegate* delegate = new VSQueueItemDelegate(this);
  m_Ui->queueListView->setItemDelegate(delegate);

  m_Ui->queueListView->installEventFilter(this);

  m_Ui->statusLabel->hide();
  m_Ui->progressLabel->hide();
  m_Ui->progressText->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::connectSignalsSlots()
{
  connect(m_Ui->startStopBtn, &QPushButton::clicked, this, &VSQueueWidget::startStopButtonClicked);

  connect(m_Ui->clearBtn, &QPushButton::clicked, [=] { m_Ui->queueListView->clearAllImports(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::addDataImporter(const QString& name, VSAbstractImporter::Pointer importer)
{
  if(!importer)
  {
    return;
  }

  VSQueueModel* queueModel = getQueueModel();

  if(queueModel)
  {
    queueModel->addImporter(name, importer, QIcon(":/SIMPL/icons/images/bullet_ball_blue.png"));
    queueModel->startQueue();
    connect(importer.get(), &VSAbstractImporter::stateChanged, [=](VSAbstractImporter* dataImporter, VSAbstractImporter::State state) {
      if(state == VSAbstractImporter::State::Finished)
      {
        removeDataImporter(importer);
      }
    });
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::insertDataImporter(int row, const QString &name, VSAbstractImporter::Pointer importer)
{
  if (!importer)
  {
    return;
  }

  VSQueueModel* queueModel = getQueueModel();

  if (queueModel)
  {
    queueModel->insertImporter(row, name, importer, QIcon(":/SIMPL/icons/images/bullet_ball_blue.png"));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::removeDataImporter(VSAbstractImporter::Pointer importer)
{
  if (!importer)
  {
    return;
  }

  VSQueueModel* queueModel = getQueueModel();

  if (queueModel)
  {
    QPersistentModelIndex index = queueModel->indexOfImporter(importer);
    if (index.isValid())
    {
      queueModel->removeRow(index.row());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::startStopButtonClicked()
{
  VSQueueModel* model = getQueueModel();

  if (model->getQueueState() == VSQueueModel::QueueState::Idle)
  {
    model->startQueue();
  }
  else if (model->getQueueState() == VSQueueModel::QueueState::Executing)
  {
    model->cancelQueue();
  }
  else
  {
    // We should not enter here, because you should not be able to click the button when the queue is in the canceling state
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::setIdleState()
{
  VSQueueModel* model = getQueueModel();
  bool filtersAtReady = false;
  for (int row = 0; row < model->rowCount() && !filtersAtReady; row++)
  {
    QModelIndex index = model->index(row, VSQueueItem::ItemData::Contents);
    VSAbstractImporter::Pointer importer = model->data(index, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
    if (importer)
    {
      if (importer->getState() == VSAbstractImporter::State::Ready)
      {
        filtersAtReady = true;
      }
    }
  }

  m_Ui->startStopBtn->setText("Start");
  m_Ui->startStopBtn->setEnabled(filtersAtReady);
  m_Ui->clearBtn->setEnabled(model->rowCount() > 0);
  m_Ui->statusLabel->hide();
  m_Ui->progressLabel->hide();
  m_Ui->progressText->hide();
  m_Ui->progressBar->setValue(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::setExecutingState()
{
  m_Ui->startStopBtn->setText("Stop");
  m_Ui->startStopBtn->setEnabled(true);
  m_Ui->clearBtn->setDisabled(true);
  m_Ui->statusLabel->show();
  m_Ui->progressLabel->show();
  m_Ui->progressText->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::setCancelingState()
{
  m_Ui->startStopBtn->setText("Stopping...");
  m_Ui->startStopBtn->setDisabled(true);
  m_Ui->clearBtn->setDisabled(true);
  m_Ui->statusLabel->setText("Stopping Import Queue...");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::processImporterMessage(const VSAbstractImporter::Pointer& importer, const AbstractMessage::Pointer& msg)
{
  ImporterMessageHandler msgHandler(importer.get(), this);
  msg->visit(&msgHandler);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::handleQueueStateChanged(VSQueueModel::QueueState queueState)
{
  if (queueState == VSQueueModel::QueueState::Idle)
  {
    setIdleState();
  }
  else if (queueState == VSQueueModel::QueueState::Executing)
  {
    setExecutingState();
  }
  else
  {
    setCancelingState();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueModel* VSQueueWidget::getQueueModel()
{
  return m_Ui->queueListView->getQueueModel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::setQueueModel(VSQueueModel* queueModel)
{
  VSQueueModel* oldModel = getQueueModel();
  if (oldModel)
  {
  }

  m_Ui->queueListView->setModel(queueModel);
  connect(queueModel, &VSQueueModel::queueStateChanged, this, &VSQueueWidget::handleQueueStateChanged);
  connect(queueModel, &VSQueueModel::notifyImporterMessage, this, &VSQueueWidget::processImporterMessage);
  connect(queueModel, &VSQueueModel::queueFinished, [=] { emit notifyStatusMessage(""); });
}
