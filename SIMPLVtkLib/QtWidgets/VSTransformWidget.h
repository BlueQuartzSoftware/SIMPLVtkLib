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

#pragma once

#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSTransform.h"

/**
* @class VSTransformWidget VSTransformWidget.h SIMPLVtkLib/QtWidgets/VSTransformWidget.h
* @brief This class displays the global transformation values for a VSTransform
* and allows the user to edit the local transformation.
*/
class VSTransformWidget : public QWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  */
  VSTransformWidget(QWidget* parent);

  /**
  * @brief Returns the VSTransform used by the widget
  * @return
  */
  VSTransform* getTransform();

  /**
  * @brief Sets the VSTransform used by the widget
  * @param transform
  */
  void setTransform(VSTransform* transform);
  
protected slots:
  /**
  * @brief Updates the translation spin boxes to match the transformation's local values
  */
  void translationSpinBoxesChanged();

  /**
  * @brief Updates the rotation spin boxes to match the transformation's local values
  */
  void rotationSpinBoxesChanged();

  /**
  * @brief Updates the scale spin boxes to match the transformation's local values
  */
  void scaleSpinBoxesChanged();

  /**
  * @brief Updates the translation labels to match the transformation's global values
  * @param position
  */
  void updateTranslationLabels(double* position);

  /**
  * @brief Updates the rotation labels to match the transformation's global values
  * @param rotation
  */
  void updateRotationLabels(double* rotation);

  /**
  * @brief Updates the scale labels to match the transformation's global values
  * @param scale
  */
  void updateScaleLabels(double* scale);

protected:
  /**
  * @brief Performs any initial setup required for the GUI
  */
  void setupGui();

private:
  class VSInternals;
  VSInternals* m_Internals;

  VSTransform* m_Transform = nullptr;
};
