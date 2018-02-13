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

#include "VSDataSetFilter.h"

#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>

#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkTIFFReader.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDataSetFilter::VSDataSetFilter(const QString &filePath)
  : VSAbstractFilter()
  , m_FilePath(filePath)
{
  createFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSDataSetFilter::getBounds() const
{
  return m_DataSet->GetBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSDataSetFilter::getOutputPort()
{
  if(m_TrivialProducer)
  {
    return m_TrivialProducer->GetOutputPort();
  }
  else
  {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) VSDataSetFilter::getOutput()
{
  if(nullptr == m_DataSet)
  {
    return nullptr;
  }

  return m_DataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  // Do nothing
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::createFilter()
{
  QFileInfo fi(m_FilePath);
  QString ext = fi.completeSuffix().toLower();

  QMimeDatabase db;
  QMimeType mimeType = db.mimeTypeForFile(m_FilePath, QMimeDatabase::MatchContent);

  setText(fi.fileName());

  if (mimeType.name().startsWith("image/"))
  {
    readImage();
  }

  if (m_DataSet != nullptr)
  {
    m_DataSet->ComputeBounds();

    m_TrivialProducer = VTK_PTR(vtkTrivialProducer)::New();
    m_TrivialProducer->SetOutput(m_DataSet);

    emit updatedOutputPort(this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::readImage()
{
  QMimeDatabase db;
  QMimeType mimeType = db.mimeTypeForFile(m_FilePath, QMimeDatabase::MatchContent);

  char* filePathPtr = m_FilePath.toLatin1().data();
  if (mimeType.inherits("image/jpeg"))
  {
    VTK_NEW(vtkJPEGReader, imageReader);
    imageReader->SetFileName(filePathPtr);
    imageReader->Update();

    m_DataSet = imageReader->GetOutput();
  }
  else if (mimeType.inherits("image/png"))
  {
    VTK_NEW(vtkPNGReader, imageReader);
    imageReader->SetFileName(filePathPtr);
    imageReader->Update();

    m_DataSet = imageReader->GetOutput();
  }
  else if (mimeType.inherits("image/tiff"))
  {
    VTK_NEW(vtkTIFFReader, imageReader);
    imageReader->SetFileName(filePathPtr);
    imageReader->Update();

    m_DataSet = imageReader->GetOutput();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSDataSetFilter::ContainsValidData(const QString &filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.completeSuffix().toLower();

  QMimeDatabase db;
  QMimeType mimeType = db.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

  if (mimeType.inherits("image/jpeg") || mimeType.inherits("image/png") ||
      mimeType.inherits("image/tiff") || ext == "dream3d" || ext == "vtk" || ext == "stl")
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSDataSetFilter::getFilterName()
{
  return text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSDataSetFilter::getToolTip() const
{
  return "Dataset Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSDataSetFilter::getOutputType()
{
  return IMAGE_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSDataSetFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}

