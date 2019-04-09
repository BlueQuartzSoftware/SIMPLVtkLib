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

#include "SIMPLVtkLib/Dialogs/Utilities/MontageSettings.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLVtkLib/Wizards/AbstractMontageWizard.h"

class SIMPLVtkLib_EXPORT PerformMontageWizard : public AbstractMontageWizard
{
    Q_OBJECT

  public:
    enum WizardPages
    {
      DisplayTypeSelection,
	  SaveFile
    };

	enum DisplayType
	{
		NotSpecified,
		Montage,
		Outline
	};

	enum ImageSource
	{
	  Datasets,
	  DataContainers
	};

    /**
     * @brief PerformMontageWizard
     * @param parent
     */
    PerformMontageWizard(QWidget* parent = nullptr);

    ~PerformMontageWizard() override;
	
  private:
	MontageSettings* m_MontageSettings = nullptr;
  public:
    PerformMontageWizard(const PerformMontageWizard&) = delete; // Copy Constructor Not Implemented
    PerformMontageWizard(PerformMontageWizard&&) = delete;      // Move Constructor Not Implemented
    PerformMontageWizard& operator=(const PerformMontageWizard&) = delete; // Copy Assignment Not Implemented
    PerformMontageWizard& operator=(PerformMontageWizard&&) = delete;      // Move Assignment Not Implemented
};

Q_DECLARE_METATYPE(PerformMontageWizard::ImageSource)
