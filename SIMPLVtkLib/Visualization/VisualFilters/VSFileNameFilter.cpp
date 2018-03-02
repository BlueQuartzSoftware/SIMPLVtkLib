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

#include "VSFileNameFilter.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUuid>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString fetchFileName(QString filePath)
{
  QFileInfo fi(filePath);
  if(fi.exists())
  {
    return fi.fileName();
  }

  return "File not Found";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFileNameFilter::VSFileNameFilter(QString filePath, VSAbstractFilter* parent)
  : VSTextFilter(nullptr, fetchFileName(filePath), filePath)
  , m_FilePath(filePath)
{
  setCheckState(Qt::Unchecked);
  setCheckable(false);
  setEditable(false);
  setParentFilter(parent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFileNameFilter* VSFileNameFilter::Create(QJsonObject &json, VSAbstractFilter* parent)
{
  QString filePath = json["File Path"].toString("");
  QFileInfo fi(filePath);

  if (fi.exists() == false)
  {
    // The file doesn't exist, so have the user give the new location of the file
    QMessageBox::warning(nullptr, "Filter Creation Warning", tr("The file '%1' is used by a filter and does not exist or has been moved.  Please select the file's new location.").arg(filePath));

    QString filter = tr("All Files (*.*)");
    filePath = QFileDialog::getOpenFileName(nullptr, tr("Locate '%1'").arg(fi.fileName()), "", filter);
    if (filePath.isEmpty())
    {
      return nullptr;
    }
  }

  VSFileNameFilter* newFilter = new VSFileNameFilter(filePath, parent);
  newFilter->setToolTip(json["Tooltip"].toString());

  QFont font = newFilter->font();
  font.setItalic(json["Italic"].toBool());
  font.setBold(json["Bold"].toBool());
  font.setUnderline(json["Underline"].toBool());
  newFilter->setFont(font);

  newFilter->setInitialized(true);
  newFilter->readTransformJson(json);

  return newFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFileNameFilter::writeJson(QJsonObject &json)
{
  VSTextFilter::writeJson(json);

  json["File Path"] = m_FilePath;
  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSFileNameFilter::GetUuid()
{
  return QUuid("{b02e564e-6ef9-58cb-a4a2-9d067f92bc7c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFileNameFilter::getFilePath()
{
  return m_FilePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFileNameFilter::getFileName()
{
  QFileInfo fi(m_FilePath);
  return fi.fileName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFileNameFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return true;
  }

  return false;
}
