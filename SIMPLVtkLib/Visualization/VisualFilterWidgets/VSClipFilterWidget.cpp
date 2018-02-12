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

#include "VSClipFilterWidget.h"

#include <QApplication>
#include <QString>

#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTableBasedClipDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "SIMPLVtkLib/QtWidgets/VSMainWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

#include "ui_VSClipFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VSClipFilterWidget::vsInternals : public Ui::VSClipFilterWidget
{
public:
  vsInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilterWidget::VSClipFilterWidget(VSClipFilter* filter, QVTKInteractor* interactor, QWidget* widget)
: VSAbstractFilterWidget(widget)
, m_Internals(new vsInternals())
, m_ClipFilter(filter)
{
  m_Internals->setupUi(this);

  m_PlaneWidget = new VSPlaneWidget(nullptr, m_ClipFilter->getBounds(), interactor);
  m_BoxWidget = new VSBoxWidget(nullptr, m_ClipFilter->getBounds(), interactor);

  QStringList clipTypes = { VSClipFilter::PlaneClipTypeString, VSClipFilter::BoxClipTypeString };
  m_Internals->clipTypeComboBox->insertItems(0, clipTypes);

  changeClipType(VSClipFilter::PlaneClipTypeString);

  connect(m_Internals->clipTypeComboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(changeClipType(const QString &)));

//  connect(m_PlaneWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));
//  connect(m_BoxWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilterWidget::~VSClipFilterWidget()
{
  delete m_PlaneWidget;
  delete m_BoxWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }

  m_PlaneWidget->setBounds(bounds);
  m_BoxWidget->setBounds(bounds);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::changeClipType(const QString &clipType)
{
  if (clipType == VSClipFilter::PlaneClipTypeString)
  {
    m_Internals->gridLayout->removeWidget(m_BoxWidget);
    m_BoxWidget->disable();
    m_BoxWidget->setParent(nullptr);

    m_PlaneWidget->enable();
    m_Internals->gridLayout->addWidget(m_PlaneWidget);
    m_PlaneWidget->updatePlaneWidget();
  }
  else
  {
    m_Internals->gridLayout->removeWidget(m_PlaneWidget);
    m_PlaneWidget->disable();
    m_PlaneWidget->setParent(nullptr);

    m_BoxWidget->enable();
    m_Internals->gridLayout->addWidget(m_BoxWidget);
    m_BoxWidget->updateBoxWidget();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::apply()
{
  if (m_Internals->clipTypeComboBox->currentText() == VSClipFilter::PlaneClipTypeString)
  {
    double normals[3];
    double origin[3];
    m_PlaneWidget->getNormals(normals);
    m_PlaneWidget->getOrigin(origin);

    m_ClipFilter->apply(origin, normals, m_Internals->insideOutCheckBox->isChecked());
  }
  else if (m_Internals->clipTypeComboBox->currentText() == VSClipFilter::BoxClipTypeString)
  {
    VTK_PTR(vtkTransform) transform = m_BoxWidget->getTransform();
    VTK_PTR(vtkPlanes) planes = m_BoxWidget->getPlanes();
    m_ClipFilter->apply(planes, transform, m_Internals->insideOutCheckBox->isChecked());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::reset()
{
  QString clipTypeString = m_ClipFilter->getLastClipTypeString();
  m_Internals->clipTypeComboBox->setCurrentText(clipTypeString);

  if (m_Internals->clipTypeComboBox->currentText() == VSClipFilter::PlaneClipTypeString)
  {
    double* origin = m_ClipFilter->getLastPlaneOrigin();
    double* normals = m_ClipFilter->getLastPlaneNormal();
    bool inverted = m_ClipFilter->getLastPlaneInverted();

    m_PlaneWidget->setNormals(normals);
    m_PlaneWidget->setOrigin(origin);
    m_Internals->insideOutCheckBox->setChecked(inverted);
    m_PlaneWidget->updatePlaneWidget();
  }
  else if (m_Internals->clipTypeComboBox->currentText() == VSClipFilter::BoxClipTypeString)
  {
    bool inverted = m_ClipFilter->getLastBoxInverted();
    VTK_PTR(vtkTransform) transform = m_ClipFilter->getLastBoxTransform();

    m_BoxWidget->setTransform(transform);
    m_Internals->insideOutCheckBox->setChecked(inverted);
    m_BoxWidget->updateBoxWidget();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::setInteractor(QVTKInteractor* interactor)
{
  if (m_PlaneWidget)
  {
    m_PlaneWidget->setInteractor(interactor);
  }

  if (m_BoxWidget)
  {
    m_BoxWidget->setInteractor(interactor);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilterWidget::setDrawingEnabled(bool enabled)
{
  if (m_Internals->clipTypeComboBox->currentText() == VSClipFilter::PlaneClipTypeString)
  {
    (enabled) ? m_PlaneWidget->enable() : m_PlaneWidget->disable();
  }
  else
  {
    (enabled) ? m_BoxWidget->enable() : m_BoxWidget->disable();
  }
}
