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

#include "VSInteractorStyleFilterCamera.h"

#include <string>

#include <vtkCamera.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindowInteractor.h>

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"

vtkStandardNewMacro(VSInteractorStyleFilterCamera);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnLeftButtonDown()
{
  m_MousePress++;
  endAction();
  if(m_MousePress >= 2)
  {
    grabFilter();
  }
  else
  {
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnRightButtonDown()
{
  vtkInteractorStyleTrackballCamera::OnRightButtonDown();

  // Cancel the current action
  cancelAction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnMouseMove()
{
  m_MousePress = 0;

  if(m_ActiveFilter && ActionType::None != m_ActionType)
  {
    switch(m_ActionType)
    {
    case ActionType::Translate:
      translateFilter();
      break;
    case ActionType::Rotate:
      rotateFilter();
      break;
    case ActionType::Scale:
      scaleFilter();
      break;
    }
  }
  else
  {
    vtkInteractorStyleTrackballCamera::OnMouseMove();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnKeyDown()
{
  std::string keyDown = this->Interactor->GetKeySym();
  if(keyDown == "t")
  {
    setActionType(ActionType::Translate);
  }
  else if(keyDown == "r")
  {
    setActionType(ActionType::Rotate);
  }
  else if(keyDown == "s")
  {
    setActionType(ActionType::Scale);
  }
  else if(keyDown == "Escape")
  {
    cancelAction();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  // End any interactions with the selected filter before changing the VSAbstractViewWidget
  releaseFilter();

  m_ViewWidget = viewWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSInteractorStyleFilterCamera::ctrlKey()
{
  return this->Interactor->GetControlKey();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSInteractorStyleFilterCamera::altKey()
{
  return this->Interactor->GetAltKey();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSInteractorStyleFilterCamera::shiftKey()
{
  return this->Interactor->GetShiftKey();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSInteractorStyleFilterCamera::dragFilterKey()
{
  return shiftKey();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::grabFilter()
{
  int* clickPos = this->GetInteractor()->GetEventPosition();
  vtkRenderer* renderer = this->GetDefaultRenderer();

  VTK_NEW(vtkPropPicker, picker);
  picker->Pick(clickPos[0], clickPos[1], 0, renderer);
  double* pickPos = picker->GetPickPosition();
  vtkProp3D* prop = picker->GetProp3D();
  VSAbstractFilter* filter = m_ViewWidget->getFilterFromProp(prop);

  if(nullptr == prop || nullptr == filter)
  {
    return;
  }

  if(shiftKey())
  {
    addSelection(filter, prop);
  }
  else if(ctrlKey())
  {
    toggleSelection(filter, prop);
  }
  else
  {
    clearSelection();
    addSelection(filter, prop);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::addSelection(VSAbstractFilter* filter, vtkProp3D* prop)
{
  m_ActiveFilter = filter;
  m_ActiveProp = prop;

  if(false == selectionIncludes(filter))
  {
    m_Selection.push_front(filter);
  }

  m_ViewWidget->selectFilter(m_ActiveFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::removeSelection(VSAbstractFilter* filter)
{
  m_Selection.remove(filter);
  if(m_ActiveFilter == filter)
  {
    if(m_Selection.size() == 0)
    {
      m_ActiveFilter = nullptr;
      m_ActiveProp = nullptr;
    }
    else
    {
      m_ActiveFilter = (*m_Selection.begin());
      m_ActiveProp = m_ViewWidget->getFilterViewSettings(m_ActiveFilter)->getActor();
    }
  }

  m_ViewWidget->selectFilter(m_ActiveFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::toggleSelection(VSAbstractFilter* filter, vtkProp3D* prop)
{
  if(selectionIncludes(filter))
  {
    removeSelection(filter);
  }
  else
  {
    addSelection(filter, prop);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::clearSelection()
{
  m_ActiveFilter = nullptr;
  m_ActiveProp = nullptr;
  m_Selection.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSInteractorStyleFilterCamera::selectionIncludes(VSAbstractFilter* filter)
{
  return std::find(m_Selection.begin(), m_Selection.end(), filter) != m_Selection.end();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInteractorStyleFilterCamera::ActionType VSInteractorStyleFilterCamera::getActionType()
{
  return m_ActionType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::setActionType(ActionType type)
{
  if(ActionType::None != m_ActionType)
  {
    endAction();
  }

  m_ActionType = type;

  switch(type)
  {
  case ActionType::Translate:
    beginTranslation();
    break;
  case ActionType::Rotate:
    beginRotation();
    break;
  case ActionType::Scale:
    beginScaling();
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::endAction()
{
  m_ActionType = ActionType::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::cancelAction()
{
  switch(m_ActionType)
  {
  case ActionType::Translate:
    cancelTranslation();
    break;
  case ActionType::Rotate:
    cancelRotation();
    break;
  case ActionType::Scale:
    cancelScaling();
    break;
  }

  m_ActionType = ActionType::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::releaseFilter()
{
  endAction();

  m_ActiveFilter = nullptr;
  m_ActiveProp = nullptr;

  clearSelection();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::translateFilter()
{
  if(nullptr == m_ActiveFilter || nullptr == m_ActiveProp)
  {
    return;
  }

  vtkRenderWindowInteractor* iren = this->Interactor;

  // Move filter
  double* obj_center = m_ActiveProp->GetCenter();

  double disp_obj_center[3], new_pick_point[4];
  double old_pick_point[4], deltaPosition[3];

  this->ComputeWorldToDisplay(obj_center[0], obj_center[1], obj_center[2],
    disp_obj_center);

  this->ComputeDisplayToWorld(iren->GetEventPosition()[0],
    iren->GetEventPosition()[1],
    disp_obj_center[2],
    new_pick_point);

  this->ComputeDisplayToWorld(iren->GetLastEventPosition()[0],
    iren->GetLastEventPosition()[1],
    disp_obj_center[2],
    old_pick_point);

  deltaPosition[0] = new_pick_point[0] - old_pick_point[0];
  deltaPosition[1] = new_pick_point[1] - old_pick_point[1];
  deltaPosition[2] = new_pick_point[2] - old_pick_point[2];

  VSTransform* transform = m_ActiveFilter->getTransform();
  double* globalPosition = transform->getPosition();
  double localDelta[3];
  for(int i = 0; i < 3; i++)
  {
    localDelta[i] = deltaPosition[i] + globalPosition[i];
  }
  delete[] globalPosition;

  transform->localizePoint(localDelta);
  for(int i = 0; i < 3; i++)
  {
    m_Translation[i] += localDelta[i];
  }

  // Translate all filters selected
  for(VSAbstractFilter* filter : m_Selection)
  {
    filter->getTransform()->translate(localDelta);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginTranslation()
{
  if(m_ActiveFilter)
  {
    m_InitialPosition = m_ActiveFilter->getTransform()->getLocalPosition();
    for(int i = 0; i < 3; i++)
    {
      m_Translation[i] = 0.0;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::cancelTranslation()
{
  for(int i = 0; i < 3; i++)
  {
    m_Translation[i] *= -1;
  }

  for(VSAbstractFilter* filter : m_Selection)
  {
    filter->getTransform()->translate(m_Translation);
  }

  m_ActionType = ActionType::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::rotateFilter()
{
  if(nullptr == m_ActiveFilter || nullptr == m_ActiveProp)
  {
    return;
  }

  vtkRenderWindowInteractor* iren = this->Interactor;
  
  int* currentMousePos = iren->GetEventPosition();
  int* prevMousePos = iren->GetLastEventPosition();
  double currentDelta[2];
  for(int i = 0; i < 2; i++)
  {
    currentDelta[i] = currentMousePos[i] - prevMousePos[i];
  }

  const double ROTATION_SPEED = 8.0;
  double rotateAmt = (currentDelta[0] + currentDelta[1]) / ROTATION_SPEED;
  m_RotationAmt += rotateAmt;

  // Rotate all selected filters
  for(VSAbstractFilter* filter : m_Selection)
  {
    filter->getTransform()->rotate(rotateAmt, m_CameraAxis);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginRotation()
{
  if(nullptr == m_ActiveFilter || nullptr == m_ActiveProp)
  {
    return;
  }

  if(m_ActiveFilter)
  {
    vtkRenderWindowInteractor* iren = this->Interactor;
    m_InitialMousePos = iren->GetEventPosition();
    m_CameraAxis = GetDefaultRenderer()->GetActiveCamera()->GetDirectionOfProjection();
    m_RotationAmt = 0.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::cancelRotation()
{
  for(VSAbstractFilter* filter : m_Selection)
  {
    filter->getTransform()->rotate(-1 * m_RotationAmt, m_CameraAxis);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::scaleFilter()
{
  if(nullptr == m_ActiveFilter || nullptr == m_ActiveProp)
  {
    return;
  }

  vtkRenderWindowInteractor* iren = this->Interactor;
  double disp_obj_center[3];

  this->ComputeWorldToDisplay(m_InitialCenter[0], m_InitialCenter[1], m_InitialCenter[2],
    disp_obj_center);

  int* currentMousePos = iren->GetEventPosition();
  double currentDelta[2];
  for(int i = 0; i < 2; i++)
  {
    currentDelta[i] = currentMousePos[i] - disp_obj_center[i];
  }
  double currentDistance = sqrt((currentDelta[0] * currentDelta[0]) + (currentDelta[1] * currentDelta[1]));

  double percentChanged = currentDistance / m_LastDistance;
  m_LastDistance = currentDistance;
  double deltaScale = percentChanged;
  m_ScaleAmt *= deltaScale;

  // Scale all selected filters
  for(VSAbstractFilter* filter : m_Selection)
  {
    filter->getTransform()->scale(deltaScale);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginScaling()
{
  if(nullptr == m_ActiveFilter || nullptr == m_ActiveProp)
  {
    return;
  }

  if(m_ActiveFilter)
  {
    vtkRenderWindowInteractor* iren = this->Interactor;
    double* obj_center = m_ActiveFilter->getTransform()->getPosition();
    double disp_obj_center[3];

    this->ComputeWorldToDisplay(obj_center[0], obj_center[1], obj_center[2],
      disp_obj_center);

    int* currentMousePos = iren->GetEventPosition();
    double currentDelta[2];
    for(int i = 0; i < 2; i++)
    {
      currentDelta[i] = currentMousePos[i] - disp_obj_center[i];
    }

    int* initialMousePos = this->Interactor->GetEventPosition();
    m_InitialCenter = m_ActiveFilter->getTransform()->getPosition();
    m_LastDistance = sqrt((currentDelta[0] * currentDelta[0]) + (currentDelta[1] * currentDelta[1]));
    m_ScaleAmt = 1.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::cancelScaling()
{
  for(VSAbstractFilter* filter : m_Selection)
  {
    filter->getTransform()->scale(1.0 / m_ScaleAmt);
  }
}
