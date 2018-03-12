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

#include "VSTextFilter.h"

#include <QtCore/QUuid>

#include <vtkAlgorithmOutput.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTextFilter::VSTextFilter(VSAbstractFilter* parent, QString text, QString toolTip)
  : VSAbstractFilter()
{
  setParentFilter(parent);

  setText(text);
  setToolTip(toolTip);
  setItalic();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTextFilter* VSTextFilter::Create(QJsonObject &json, VSAbstractFilter* parent)
{
  QString text = json["Text"].toString();
  QString tooltip = json["Tooltip"].toString();

  VSTextFilter* filter = new VSTextFilter(parent, text, tooltip);

  QFont font = filter->font();
  font.setItalic(json["Italic"].toBool());
  font.setBold(json["Bold"].toBool());
  font.setUnderline(json["Underline"].toBool());
  filter->setFont(font);

  filter->setInitialized(true);
  filter->readTransformJson(json);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSTextFilter::getFilterName()
{
  return text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSTextFilter::getToolTip() const
{
  return toolTip();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTextFilter::setItalic(bool italic)
{
  QFont itemFont = font();
  itemFont.setItalic(italic);
  setFont(itemFont);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTextFilter::setBold(bool bold)
{
  QFont itemFont = font();
  itemFont.setBold(bold);
  setFont(itemFont);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTextFilter::setUnderline(bool underline)
{
  QFont itemFont = font();
  itemFont.setUnderline(underline);
  setFont(itemFont);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSTextFilter::GetUuid()
{
  return QUuid("{c819de20-4110-5ea2-b847-89307b05895c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSTextFilter::getOutputPort()
{
  if(getParentFilter())
  {
    return getParentFilter()->getOutputPort();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) VSTextFilter::getOutput()
{
  if(getParentFilter())
  {
    return getParentFilter()->getOutput();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSTextFilter::getOutputType()
{
  // Return the parent's output type if a parent exists
  if(getParentFilter())
  {
    return getParentFilter()->getOutputType();
  }

  return INVALID_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSTextFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTextFilter::writeJson(QJsonObject &json)
{
  VSAbstractFilter::writeJson(json);

  json["Text"] = text();
  json["Tooltip"] = toolTip();
  json["Italic"] = font().italic();
  json["Bold"] = font().bold();
  json["Underline"] = font().underline();
  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSTextFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTextFilter::createFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTextFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  setInputPort(filter->getOutputPort());

  emit updatedOutputPort(filter);
}
