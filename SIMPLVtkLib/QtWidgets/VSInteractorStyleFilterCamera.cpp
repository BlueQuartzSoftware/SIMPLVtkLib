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
void VSInteractorStyleFilterCamera::OnLeftButtonUp()
{
  vtkInteractorStyleTrackballCamera::OnLeftButtonUp();

  releaseFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnRightButtonDown()
{
  vtkInteractorStyleTrackballCamera::OnRightButtonDown();

  // Cancel any drag operations and release the filter
  cancelAction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnMouseMove()
{
  m_MousePress = 0;

  if(m_GrabbedFilter && ActionType::None != m_ActionType)
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
  m_GrabbedProp = picker->GetProp3D();

  m_GrabbedFilter = m_ViewWidget->getFilterFromProp(m_GrabbedProp);
  m_ViewWidget->selectFilter(m_GrabbedFilter);
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

  m_GrabbedFilter = nullptr;
  m_GrabbedProp = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::translateFilter()
{
  if(nullptr == m_GrabbedFilter || nullptr == m_GrabbedProp)
  {
    return;
  }

  vtkRenderWindowInteractor* iren = this->Interactor;

  // Move filter
  double* obj_center = m_GrabbedProp->GetCenter();

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

  VSTransform* transform = m_GrabbedFilter->getTransform();
  double* globalPosition = transform->getPosition();
  double localDelta[3];
  for(int i = 0; i < 3; i++)
  {
    localDelta[i] = deltaPosition[i] + globalPosition[i];
  }
  delete[] globalPosition;

  transform->localizePoint(localDelta);
  transform->translate(localDelta);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginTranslation()
{
  if(m_GrabbedFilter)
  {
    m_InitialPosition = m_GrabbedFilter->getTransform()->getLocalPosition();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::cancelTranslation()
{
  if(m_GrabbedFilter)
  {
    m_GrabbedFilter->getTransform()->setLocalPosition(m_InitialPosition);
  }

  m_ActionType = ActionType::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::rotateFilter()
{
  if(nullptr == m_GrabbedFilter || nullptr == m_GrabbedProp)
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
  m_GrabbedFilter->getTransform()->rotate(rotateAmt, m_CameraAxis);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginRotation()
{
  if(nullptr == m_GrabbedFilter || nullptr == m_GrabbedProp)
  {
    return;
  }

  if(m_GrabbedFilter)
  {
    vtkRenderWindowInteractor* iren = this->Interactor;
    m_InitialMousePos = iren->GetEventPosition();
    m_InitialRotation = m_GrabbedFilter->getTransform()->getLocalRotation();
    m_CameraAxis = GetDefaultRenderer()->GetActiveCamera()->GetDirectionOfProjection();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::cancelRotation()
{
  if(m_GrabbedFilter)
  {
    m_GrabbedFilter->getTransform()->setLocalRotation(m_InitialRotation);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::scaleFilter()
{
  if(nullptr == m_GrabbedFilter || nullptr == m_GrabbedProp)
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

  double percentChanged = currentDistance / m_InitialDistance;
  m_ScaleAmt = percentChanged - 1.0;

  double newScale[3];
  for(int i = 0; i < 3; i++)
  {
    newScale[i] = m_InitialScale[i] + m_InitialScale[i] * m_ScaleAmt;
    if(newScale[i] < 0.0)
    {
      newScale[i] = 0.0;
    }
  }
  m_GrabbedFilter->getTransform()->setLocalScale(newScale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginScaling()
{
  if(nullptr == m_GrabbedFilter || nullptr == m_GrabbedProp)
  {
    return;
  }

  if(m_GrabbedFilter)
  {
    vtkRenderWindowInteractor* iren = this->Interactor;
    double* obj_center = m_GrabbedFilter->getTransform()->getPosition();
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
    m_InitialCenter = m_GrabbedFilter->getTransform()->getPosition();
    m_InitialDistance = sqrt((currentDelta[0] * currentDelta[0]) + (currentDelta[1] * currentDelta[1]));
    m_InitialScale = m_GrabbedFilter->getTransform()->getLocalScale();
    m_ScaleAmt = 1.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::cancelScaling()
{
  if(m_GrabbedFilter)
  {
    double* localScale = m_GrabbedFilter->getTransform()->getLocalScale();
    for(int i = 0; i < 3; i++)
    {
      localScale[i] -= m_ScaleAmt;
    }
    m_GrabbedFilter->getTransform()->setLocalScale(localScale);
    //delete[] localScale;
  }
}
