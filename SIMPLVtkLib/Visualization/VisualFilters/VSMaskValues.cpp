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

#include "VSMaskValues.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "ui_VSMaskFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskValues::VSMaskValues(VSMaskFilter* filter)
: VSAbstractFilterValues(filter)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskValues::VSMaskValues(const VSMaskValues& values)
: VSAbstractFilterValues(values.getFilter())
, m_LastArrayName(values.m_LastArrayName)
, m_MaskArrayName(values.m_MaskArrayName)
, m_FreshFilter(values.m_FreshFilter)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskValues::applyValues()
{
  VSAbstractFilter::FilterListType filters = getSelection();
  for(VSAbstractFilter* filter : filters)
  {
    // Make sure this is the appropriate filter type first
    FilterType* filterType = dynamic_cast<FilterType*>(filter);
    if(filterType)
    {
      filterType->applyValues(this);
    }
  }

  m_FreshFilter = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskValues::resetValues()
{
  setMaskName(m_LastArrayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSMaskValues::hasChanges() const
{
  if(m_FreshFilter)
  {
    return true;
  }

  if(getSelection().size() > 1)
  {
    return true;
  }

  return m_MaskArrayName != getLastArrayName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSMaskValues::getMaskName() const
{
  return m_MaskArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskValues::setMaskName(QString maskName)
{
  m_MaskArrayName = maskName;
  emit maskNameChanged(maskName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSMaskValues::getLastArrayName() const
{
  return m_LastArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskValues::setLastArrayName(QString lastArrayName)
{
  m_LastArrayName = lastArrayName;

  emit lastArrayNameChanged(lastArrayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VSMaskValues::createFilterWidget()
{
  Ui::VSMaskFilterWidget* ui = new Ui::VSMaskFilterWidget;
  QWidget* filterWidget = new QWidget();
  ui->setupUi(filterWidget);
  VSMaskFilter* filter = dynamic_cast<VSMaskFilter*>(getFilter());

  ui->maskComboBox->addItems(getFilter()->getScalarNames());
  ui->maskComboBox->setCurrentText(getLastArrayName());

  connect(ui->maskComboBox, &QComboBox::currentTextChanged, [=](QString text) { m_MaskArrayName = text; });
  connect(getFilter(), &VSAbstractFilter::arrayNamesChanged, this, [=] {
    QStringList scalarNames = getFilter()->getScalarNames();
    ui->maskComboBox->blockSignals(true);
    ui->maskComboBox->clear();
    ui->maskComboBox->addItems(scalarNames);
    ui->maskComboBox->blockSignals(false);
    if(scalarNames.contains(m_MaskArrayName))
    {
      ui->maskComboBox->setCurrentText(m_MaskArrayName);
    }
    else
    {
      if(scalarNames.size() > 0)
      {
        setMaskName(scalarNames.at(0));
      }
      else
      {
        setMaskName(QString::Null());
      }
    }
  });

  connect(this, &VSMaskValues::maskNameChanged, [=](QString maskName) {
    ui->maskComboBox->blockSignals(true);
    ui->maskComboBox->setCurrentText(maskName);
    ui->maskComboBox->blockSignals(false);
  });

  return filterWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskValues::loadJson(QJsonObject& json)
{
  m_LastArrayName = json["Last Array Name"].toString();
  m_MaskArrayName = json["Mask Name"].toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskValues::writeJson(QJsonObject& json)
{
  json["Last Array Name"] = m_LastArrayName;
  json["Mask Name"] = m_MaskArrayName;
}
