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

#include "VSSliceFilter.h"

#include <QtCore/QString>

#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCutter.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilter::VSSliceFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_SliceAlgorithm = nullptr;
  setParentFilter(parent);

  //m_SliceWidget = new VSPlaneWidget(sliceFunctionWidget, parent->getBounds(), parent->getInteractor());
  //m_SliceWidget->show();

  //connect(m_SliceWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));

  setFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilter::~VSSliceFilter()
{
  m_SliceAlgorithm = nullptr;
  //delete m_SliceWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }

  //m_SliceWidget->setBounds(bounds);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::setFilter()
{
  m_SliceAlgorithm = vtkSmartPointer<vtkCutter>::New();
  //m_SliceAlgorithm->SetCutFunction(m_SliceWidget->getImplicitFunction());

  m_SliceAlgorithm->SetInputConnection(m_ParentFilter->getOutputPort());

  //calculateOutput();
  m_ConnectedInput = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void VSSliceFilter::calculateOutput()
//{
//  if(!m_ConnectedInput && m_ParentFilter)
//  {
//    m_SliceAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
//    m_ConnectedInput = true;
//
//    m_OutputProducer->SetInputConnection(m_SliceAlgorithm->GetOutputPort());
//  }
//
//  m_SliceAlgorithm->SetCutFunction(m_SliceWidget->getImplicitFunction());
//  m_SliceAlgorithm->Update();
//
//  m_isDirty = false;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSSliceFilter::getFilterName()
{
  return "Slice";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//VSAbstractWidget* VSSliceFilter::getWidget()
//{
//  return m_SliceWidget;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::apply()
{
  //m_SliceWidget->apply();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSSliceFilter::getOutputType()
{
  return POLY_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSSliceFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}
