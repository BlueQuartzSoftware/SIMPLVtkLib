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
#include <QtCore/QUuid>

#include <vtkBMPReader.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkRectilinearGrid.h>
#include <vtkRectilinearGridReader.h>
#include <vtkSTLReader.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTIFFReader.h>
#include <vtkUnstructuredGridReader.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDataSetFilter::VSDataSetFilter(const QString& filePath, VSAbstractFilter* parent)
: VSAbstractDataFilter()
, m_FilePath(filePath)
{
  createFilter();
  setParentFilter(parent);
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
VSDataSetFilter* VSDataSetFilter::Create(const QString& filePath, QJsonObject& json, VSAbstractFilter* parent)
{
  VSDataSetFilter* filter = new VSDataSetFilter(filePath, parent);

  filter->setInitialized(true);
  filter->readTransformJson(json);
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::writeJson(QJsonObject& json)
{
  VSAbstractFilter::writeJson(json);

  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::createFilter()
{
  m_TrivialProducer = VTK_PTR(vtkTrivialProducer)::New();

  readDataSet();

  if(m_DataSet != nullptr)
  {
    updateDisplayName();

    m_DataSet->ComputeBounds();

    emit updatedOutputPort(this);
  }
  else
  {
    setText("Invalid Import File");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::updateDisplayName()
{
  dataType_t outputType = getOutputType();
  switch(outputType)
  {
  case dataType_t::IMAGE_DATA:
    setText("Image Data");
    break;
  case dataType_t::POINT_DATA:
    setText("Point Data");
    break;
  case dataType_t::POLY_DATA:
    setText("Poly Data");
    break;
  case dataType_t::UNSTRUCTURED_GRID:
    setText("Unstructured Grid Data");
    break;
  default:
    setText("Other Data");
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::readDataSet()
{
  QFileInfo fi(m_FilePath);
  QString ext = fi.completeSuffix().toLower();

  QMimeDatabase db;
  QMimeType mimeType = db.mimeTypeForFile(m_FilePath, QMimeDatabase::MatchContent);
  QString mimeName = mimeType.name();

  if(mimeType.name().startsWith("image/"))
  {
    readImage();
  }
  else if(ext == "vtk" || ext == "vti" || ext == "vtp" || ext == "vtr" || ext == "vts" || ext == "vtu")
  {
    readVTKFile();
  }
  else if(ext == "stl")
  {
    readSTLFile();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::reloadData()
{
  readDataSet();

  if(m_DataSet != nullptr)
  {
    updateDisplayName();

    m_DataSet->ComputeBounds();

    emit updatedOutputPort(this);
  }
  else
  {
    setText("Invalid Import File");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::readImage()
{
  QMimeDatabase db;
  QMimeType mimeType = db.mimeTypeForFile(m_FilePath, QMimeDatabase::MatchContent);

  if(mimeType.inherits("image/jpeg"))
  {
    VTK_NEW(vtkJPEGReader, imageReader);
    imageReader->SetFileName(m_FilePath.toLatin1().data());
    imageReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = imageReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
  else if(mimeType.inherits("image/png"))
  {
    VTK_NEW(vtkPNGReader, imageReader);
    imageReader->SetFileName(m_FilePath.toLatin1().data());
    imageReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = imageReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
  else if(mimeType.inherits("image/tiff"))
  {
    VTK_NEW(vtkTIFFReader, imageReader);
    imageReader->SetFileName(m_FilePath.toLatin1().data());
    imageReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = imageReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
  else if(mimeType.inherits("image/bmp"))
  {
    VTK_NEW(vtkBMPReader, imageReader);
    imageReader->SetFileName(m_FilePath.toLatin1().data());
    imageReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = imageReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::readVTKFile()
{
  QFileInfo fi(m_FilePath);
  QString ext = fi.completeSuffix().toLower();

  if(ext == "vtk")
  {
    VTK_NEW(vtkGenericDataObjectReader, vtkReader);
    vtkReader->SetFileName(m_FilePath.toLatin1().data());
    vtkReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = vtkDataSet::SafeDownCast(vtkReader->GetOutput());
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
  else if(ext == "vti")
  {
    VTK_NEW(vtkImageReader2, vtkReader);
    vtkReader->SetFileName(m_FilePath.toLatin1().data());
    vtkReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = vtkReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
  else if(ext == "vtp")
  {
    VTK_NEW(vtkPolyDataReader, vtkReader);
    vtkReader->SetFileName(m_FilePath.toLatin1().data());
    vtkReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = vtkReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
  else if(ext == "vtr")
  {
    VTK_NEW(vtkRectilinearGridReader, vtkReader);
    vtkReader->SetFileName(m_FilePath.toLatin1().data());
    vtkReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = vtkReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
  else if(ext == "vts")
  {
    VTK_NEW(vtkStructuredGridReader, vtkReader);
    vtkReader->SetFileName(m_FilePath.toLatin1().data());
    vtkReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = vtkReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
  else if(ext == "vtu")
  {
    VTK_NEW(vtkStructuredGridReader, vtkReader);
    vtkReader->SetFileName(m_FilePath.toLatin1().data());
    vtkReader->Update();

    VTK_PTR(vtkDataSet) newDataSet = vtkReader->GetOutput();
    m_TrivialProducer->SetOutput(newDataSet);
    m_TrivialProducer->Update();
    m_DataSet = newDataSet;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::readSTLFile()
{
  VTK_NEW(vtkSTLReader, reader);
  reader->SetFileName(m_FilePath.toLatin1().data());
  reader->Update();

  VTK_PTR(vtkDataSet) newDataSet = reader->GetOutput();
  m_TrivialProducer->SetOutput(newDataSet);
  m_TrivialProducer->Update();
  m_DataSet = newDataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSDataSetFilter::GetUuid()
{
  return QUuid("{e6596215-3f53-5e62-8f04-cc123f27d1e3}");
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
  return "DataSet Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSDataSetFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  if(dynamic_cast<VSFileNameFilter*>(filter))
  {
    return true;
  }

  return false;
}
