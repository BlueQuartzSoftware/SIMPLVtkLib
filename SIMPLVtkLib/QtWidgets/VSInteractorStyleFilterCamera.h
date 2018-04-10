/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include <vtkInteractorStyleTrackballCamera.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

class VSAbstractViewWidget;

/**
* @class VSInteractorStyleFilterCamera VSInteractorStyleFilterCamera.h SIMPLVtkLib/QtWidgets/VSInteractorStyleFilterCamera.h
* @brief This class handles filter selection and camera controls by extending vtkInteractorStyleTrackballCamera.
* Filter selection is performed when the corresponding modifier key is held down.  The filter can then be 
* dragged around to modify its transformation until the mouse or modifier key is released.
*/
class SIMPLVtkLib_EXPORT VSInteractorStyleFilterCamera : public vtkInteractorStyleTrackballCamera
{
public:
  static VSInteractorStyleFilterCamera* New();
  vtkTypeMacro(VSInteractorStyleFilterCamera, vtkInteractorStyleTrackballCamera);

  /**
  * @brief Handles left mouse button presses
  */
  void OnLeftButtonDown() override;

  /**
  * @brief Cancels any drag operations.
  */
  void OnRightButtonDown() override;

  /**
  * @brief Handles mouse movements
  */
  void OnMouseMove() override;

  /**
  * @brief Checks if the ActionType needs to be set
  */
  void OnKeyDown() override;

  /**
  * @brief Sets the VSAbstractViewWidget for selecting filters from
  * @param viewWidget
  */
  void setViewWidget(VSAbstractViewWidget* viewWidget);

protected:
  enum class ActionType
  {
    None = 0,
    Translate,
    Rotate,
    Scale
  };

  /**
  * @brief Returns the ActionType for the interactor style
  * @return
  */
  ActionType getActionType();

  /**
   * @brief Sets the ActionType for the interactor style
   * @param type
   */
  void setActionType(ActionType type);

  /**
   * @brief Ends the current action
   */
  void endAction();

  /**
   * @brief Cancels the current action and resets the filter to the way it was when the action began
   */
  void cancelAction();

  /**
   * @brief Finds the filter at the current mouse cordinates and grabs it.
   */
  void grabFilter();

  /**
   * @brief Adds the filter and prop to the current selection
   * @param filter
   * @param prop
   */
  void addSelection(VSAbstractFilter* filter, vtkProp3D* prop);

  /**
   * @brief Removes the filter from the current selection
   * @param filter
   */
  void removeSelection(VSAbstractFilter* filter);

  /**
   * @brief Clears the current selection
   */
  void clearSelection();

  /**
   * @brief Toggles the filter into or out of the current selection
   * @param filter
   * @param prop
   */
  void toggleSelection(VSAbstractFilter* filter, vtkProp3D* prop);

  /**
   * @brief Returns true if the current selection includes the given filter.
   * Returns false otherwise.
   * @param filter
   * @return
   */
  bool selectionIncludes(VSAbstractFilter* filter);

  /**
  * @brief Releases the selected filter
  */
  void releaseFilter();

  /**
  * @brief Moves the selected filter
  */
  void translateFilter();

  /**
  * @brief Begins a drag movement
  */
  void beginTranslation();

  /**
  * @brief Cancels a drag movement
  */
  void cancelTranslation();

  /**
  * @brief Rotates the selected filter based on the camera angle
  */
  void rotateFilter();

  /**
  * @brief Sets the initial rotation values
  */
  void beginRotation();

  /**
  * @brief Cancels the filter rotation
  */
  void cancelRotation();

  /**
  * @brief Scales the selected filter based on mouse movements
  */
  void scaleFilter();

  /**
  * @brief Sets the initial rotation values
  */
  void beginScaling();

  /**
  * @brief Resets the selected filter's scaling to the initial value.
  */
  void cancelScaling();

  /**
  * @brief Returns true if the CTRL key is down.  Returns false otherwise.
  * @return
  */
  bool ctrlKey();

  /**
  * @brief Returns true if the Alt key is down.  Returns false otherwise.
  * @return
  */
  bool altKey();

  /**
  * @brief Returns true if the Shift key is down.  Returns false otherwise.
  * @return
  */
  bool shiftKey();

  /**
  * @brief Returns true if the required drag filter key is down.  Returns false otherwise.
  * @return
  */
  bool dragFilterKey();

private:
  VSAbstractFilter* m_ActiveFilter = nullptr;
  vtkProp3D* m_ActiveProp = nullptr;
  std::list<VSAbstractFilter*> m_Selection;
  int m_MousePress = 0;
  ActionType m_ActionType = ActionType::None;
  // Position
  double* m_InitialPosition;
  double m_Translation[3];
  // Rotation
  int* m_InitialMousePos;
  double* m_CameraAxis;
  double m_RotationAmt;
  // Scaling
  double* m_InitialCenter;
  double m_LastDistance;
  double m_ScaleAmt = 1.0;
  

  VSAbstractViewWidget* m_ViewWidget = nullptr;
};
