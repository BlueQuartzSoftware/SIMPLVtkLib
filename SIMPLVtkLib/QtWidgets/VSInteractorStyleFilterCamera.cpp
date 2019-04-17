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
#include <vtkMath.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSViewWidget.h"
#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"

vtkStandardNewMacro(VSInteractorStyleFilterCamera);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnLeftButtonDown()
{
  if(GetInteractionMode() == VTKIS_IMAGE2D)
  {
    vtkInteractorStyleImage::OnLeftButtonDown();
  }
  else
  {
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }
  updateLinkedRenderWindows();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnRightButtonDown()
{
  if(GetInteractionMode() == VTKIS_IMAGE2D)
  {
    vtkInteractorStyleImage::OnRightButtonDown();
  }
  else
  {
    vtkInteractorStyleTrackballCamera::OnRightButtonDown();
  }
  updateLinkedRenderWindows();

  determineSubsampling();
  // Cancel the current action
  cancelAction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnDoubleClick()
{
  grabFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnMouseMove()
{
  if(m_ActiveFilter && ActionType::None != m_ActionType && !m_CustomTransform)
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
    case ActionType::ResetTransform:
      resetTransform();
      break;
    case ActionType::None:
      break;
    }
  }
  else
  {
    if(GetInteractionMode() == VTKIS_IMAGE2D)
    {
      vtkInteractorStyleImage::OnMouseMove();
    }
    else
    {
      vtkInteractorStyleTrackballCamera::OnMouseMove();
    }
    updateLinkedRenderWindows();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnMouseWheelForward()
{
  determineSubsampling();
  if(GetInteractionMode() == VTKIS_IMAGE2D)
  {
    vtkInteractorStyleImage::OnMouseWheelForward();
  }
  else
  {
    vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
  }
  updateLinkedRenderWindows();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnMouseWheelBackward()
{
  determineSubsampling();
  if(GetInteractionMode() == VTKIS_IMAGE2D)
  {
    vtkInteractorStyleImage::OnMouseWheelBackward();
  }
  else
  {
    vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
  }
  updateLinkedRenderWindows();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::updateLinkedRenderWindows()
{
  if(nullptr == m_ViewWidget)
  {
    return;
  }
  if(nullptr == m_ViewWidget->getVisualizationWidget())
  {
    return;
  }

  VSVisualizationWidget::LinkedRenderWindowType linkedRenderWindows = m_ViewWidget->getVisualizationWidget()->getLinkedRenderWindows();
  for(VTK_PTR(vtkRenderWindow) renderWindow : linkedRenderWindows)
  {
    renderWindow->Render();
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
	if(shiftKey() || (m_ActionAxis == Axis::None && m_ActionType != ActionType::None && !m_CustomTransform))
	{
	  cancelAction();
	}
	if(m_CustomTransform)
	{
	  m_CustomTransform = false;
	  m_CustomTransformAmount.clear();
	}
	else if(m_ActionAxis != Axis::None)
	{
	  m_ActionAxis = Axis::None;
	}
  }
  else if(keyDown == "a")
  {
    if(ctrlKey() && !shiftKey())
    {
      selectAllFilters();
    }
    else if(ctrlKey() && shiftKey())
    {
      deselectAllFilters();
    }
  }
  else if(keyDown == "x")
  {
    setAxis(Axis::X);
  }
  else if(keyDown == "y")
  {
	if(ctrlKey())
	{
	  redoTransform();
	}
	else
	{
	  setAxis(Axis::Y);
	}
  }
  else if(keyDown == "z")
  {
    if(altKey())
    {
      setActionType(ActionType::ResetTransform);
    }
	else if(ctrlKey())
	{
	  undoTransform();
	}
    else
    {
      setAxis(Axis::Z);
    }
  }
  else if (keyDown == "space")
  {
    if (m_CustomTransform)
    {
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
        case ActionType::ResetTransform:
          resetTransform();
          break;
        case ActionType::None:
          break;
        }
      }
      m_CustomTransform = false;
      m_CustomTransformAmount.clear();
    }
    setActionType(ActionType::None);
  }
  // Key down is a number
  else if(!keyDown.empty() && std::all_of(keyDown.begin(), keyDown.end(), ::isdigit))
  {
    if(m_ActionType != ActionType::None)
    {
      if (!m_CustomTransform)
      {
        m_CustomTransform = true;
      }
      m_CustomTransformAmount.append(keyDown.at(0));
    }
  }
  else if (keyDown == "minus" || keyDown == "plus")
  {
    if(m_ActionType != ActionType::None && !m_CustomTransform)
    {
      m_CustomTransform = true;
      m_CustomTransformAmount.clear();
      if (keyDown == "minus")
      {
        m_CustomTransformAmount.append("-");
      }
      else if (keyDown == "plus")
      {
        m_CustomTransformAmount.append("+");
      }
    }
  }
  else if (keyDown == "period")
  {
    if (m_ActionType == ActionType::None)
    {
      return;
    }
    if (m_CustomTransform)
    {
      if (!m_CustomTransformAmount.contains("."))
      {
        m_CustomTransformAmount.append(".");
      }
    }
    else
    {
      m_CustomTransform = true;
      m_CustomTransformAmount.clear();
      m_CustomTransformAmount.append("0.");
    }
  }
  else if(keyDown == "Up" || keyDown == "Down")
  {
	if(!m_CustomTransform)
	{
	  m_CustomTransform = true;
	}
	QString previousCustomTransformAmount = m_CustomTransformAmount;
	m_CustomTransformAmount.clear();
	if(m_ActiveFilter && ActionType::None != m_ActionType)
	{
	  setAxis(Axis::Y);
	  switch(m_ActionType)
	  {
	  case ActionType::Translate:
		if(keyDown == "Up")
		{
		  m_CustomTransformAmount = "1.0";
		}
		else
		{
		  m_CustomTransformAmount = "-1.0";
		}
		translateFilter();
		break;
	  case ActionType::Rotate:
		setAxis(Axis::X);
		if(keyDown == "Up")
		{
		  m_CustomTransformAmount = "-1.0";
		}
		else
		{
		  m_CustomTransformAmount = "1.0";
		}
		rotateFilter();
		break;
	  case ActionType::Scale:
		if(!previousCustomTransformAmount.isEmpty())
		{
		  if(keyDown == "Up")
		  {
			m_CustomTransformAmount = previousCustomTransformAmount.toDouble() * 1.1;
		  }
		  else
		  {
			m_CustomTransformAmount = previousCustomTransformAmount.toDouble() * 0.9;
		  }
		}
		else
		{
		  if(keyDown == "Up")
		  {
			m_CustomTransformAmount = "1.1";
		  }
		  else
		  {
			m_CustomTransformAmount = "0.9";
		  }
		}
		scaleFilter();
		break;
	  }
	}
	m_CustomTransform = false;
	m_CustomTransformAmount.clear();
	setAxis(Axis::None);
  }
  else if(keyDown == "Left" || keyDown == "Right")
  {
	if(!m_CustomTransform)
	{
	  m_CustomTransform = true;
	}
	QString previousCustomTransformAmount = m_CustomTransformAmount;
	m_CustomTransformAmount.clear();
	if(m_ActiveFilter && ActionType::None != m_ActionType)
	{
	  switch(m_ActionType)
	  {
	  case ActionType::Translate:
		setAxis(Axis::X);
		if(keyDown == "Right")
		{
		  m_CustomTransformAmount = "1.0";
		}
		else
		{
		  m_CustomTransformAmount = "-1.0";
		}
		translateFilter();
		break;
	  case ActionType::Rotate:
		if(keyDown == "Right")
		{
		  m_CustomTransformAmount = "-1.0";
		}
		else
		{
		  m_CustomTransformAmount = "1.0";
		}
		setAxis(Axis::Z);
		rotateFilter();
		break;
	  case ActionType::Scale:
		setAxis(Axis::X);
		if(!previousCustomTransformAmount.isEmpty())
		{
		  if(keyDown == "Right")
		  {
			m_CustomTransformAmount = previousCustomTransformAmount.toDouble() * 1.1;
		  }
		  else
		  {
			m_CustomTransformAmount = previousCustomTransformAmount.toDouble() * 0.9;
		  }
		}
		else
		{
		  if(keyDown == "Right")
		  {
			m_CustomTransformAmount = "1.1";
		  }
		  else
		  {
			m_CustomTransformAmount = "0.9";
		  }
		}
		scaleFilter();
		break;
	  }
	}
	m_CustomTransform = false;
	m_CustomTransformAmount.clear();
	setAxis(Axis::None);
  }
  updateTransformText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::OnChar()
{
  // Do nothing
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
VSInteractorStyleFilterCamera::FilterProp VSInteractorStyleFilterCamera::getFilterFromScreenCoords(int pos[2])
{
  vtkRenderer* renderer = this->GetDefaultRenderer();

  FilterProp filterProp;

  VTK_NEW(vtkPropPicker, picker);
  picker->Pick(pos[0], pos[1], 0, renderer);
  filterProp.first = picker->GetProp3D();
  filterProp.second = m_ViewWidget->getFilterFromProp(filterProp.first);

  renderer->GetRenderWindow()->Render();

  return filterProp;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::grabFilter()
{
  int* clickPos = this->GetInteractor()->GetEventPosition();
  vtkProp3D* prop = nullptr;
  VSAbstractFilter* filter = nullptr;
  std::tie(prop, filter) = getFilterFromScreenCoords(clickPos);

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
    setSelection(filter, prop);
  }
  updateTransformText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::addSelection(VSAbstractFilter* filter, vtkProp3D* prop)
{
  m_ActiveFilter = filter;
  m_ActiveProp = prop;

  m_ViewWidget->selectFilter(m_ActiveFilter, VSAbstractViewWidget::SelectionType::AddSelection);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::setSelection(VSAbstractFilter* filter, vtkProp3D* prop)
{
  m_ActiveFilter = filter;
  m_ActiveProp = prop;

  m_ViewWidget->selectFilter(m_ActiveFilter, VSAbstractViewWidget::SelectionType::Current);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::removeSelection(VSAbstractFilter* filter)
{
  VSAbstractFilter::FilterListType selection = getFilterSelection();
  selection.remove(filter);
  if(m_ActiveFilter == filter)
  {
    if(selection.size() == 0)
    {
      m_ActiveFilter = nullptr;
      m_ActiveProp = nullptr;
    }
    else
    {
      m_ActiveFilter = (*selection.begin());
      m_ActiveProp = m_ViewWidget->getFilterViewSettings(m_ActiveFilter)->getActor();
    }
  }

  m_ViewWidget->selectFilter(filter, VSAbstractViewWidget::SelectionType::RemoveSelection);
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
bool VSInteractorStyleFilterCamera::selectionIncludes(VSAbstractFilter* filter)
{
  VSAbstractFilter::FilterListType selection = getFilterSelection();
  return std::find(selection.begin(), selection.end(), filter) != selection.end();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSInteractorStyleFilterCamera::getFilterSelection() const
{
  return m_ViewWidget->getSelectedFilters();
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
  case ActionType::None:
    m_ActionAxis = Axis::None;
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInteractorStyleFilterCamera::Axis VSInteractorStyleFilterCamera::getAxis()
{
  return m_ActionAxis;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::setAxis(Axis axis)
{
  m_ActionAxis = axis;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::endAction()
{
  m_ActionType = ActionType::None;
  m_ActionAxis = Axis::None;
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
  case ActionType::None:
    break;
  }

  m_ActionType = ActionType::None;
  m_ActionAxis = Axis::None;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::releaseFilter()
{
  endAction();

  m_ActiveFilter = nullptr;
  m_ActiveProp = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::translateFilter()
{
  if(nullptr == m_ActiveFilter || nullptr == m_ActiveProp)
  {
	m_PreviousTransforms.clear();
    return;
  }

  vtkRenderWindowInteractor* iren = this->Interactor;

  // Move filter
  double* obj_center = m_ActiveProp->GetCenter();

  double disp_obj_center[3], new_pick_point[4];
  double old_pick_point[4], deltaPosition[3];

  this->ComputeWorldToDisplay(obj_center[0], obj_center[1], obj_center[2], disp_obj_center);
  this->ComputeDisplayToWorld(iren->GetEventPosition()[0], iren->GetEventPosition()[1], disp_obj_center[2], new_pick_point);
  this->ComputeDisplayToWorld(iren->GetLastEventPosition()[0], iren->GetLastEventPosition()[1], disp_obj_center[2], old_pick_point);

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

  // Check for custom transform
  if (m_CustomTransform)
  {
    double newDelta = m_CustomTransformAmount.toDouble();
    localDelta[0] = newDelta;
    localDelta[1] = newDelta;
    localDelta[2] = newDelta;
  }

  switch(m_ActionAxis)
  {
  case Axis::X:
    localDelta[1] = 0.0;
    localDelta[2] = 0.0;
    break;
  case Axis::Y:
    localDelta[0] = 0.0;
    localDelta[2] = 0.0;
    break;
  case Axis::Z:
    localDelta[0] = 0.0;
    localDelta[1] = 0.0;
    break;
  case Axis::None:
  default:
    break;
  }

  // Translate all filters selected
  VSAbstractFilter::FilterListType selection = getFilterSelection();
  for(VSAbstractFilter* filter : selection)
  {
	VSTransform* previousTransform = new VSTransform(*filter->getTransform());
	previousTransform->setLocalPosition(filter->getTransform()->getLocalPosition());
	previousTransform->setLocalScale(filter->getTransform()->getLocalScale());
	previousTransform->setLocalRotation(filter->getTransform()->getLocalRotation());
	m_PreviousTransforms.insert(std::make_pair(filter, previousTransform));
    filter->getTransform()->translate(localDelta);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginTranslation()
{
  if(m_ActiveFilter && m_ActiveFilter->getTransform())
  {
    m_InitialPosition = m_ActiveFilter->getTransform()->getLocalPosition();
    for(int i = 0; i < 3; i++)
    {
      m_Translation[i] = 0.0;
    }
  }
  m_PreviousTransforms.clear();
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

  VSAbstractFilter::FilterListType selection = getFilterSelection();
  for(VSAbstractFilter* filter : selection)
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
	m_PreviousTransforms.clear();
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

  double rotationAxis[3] = {};
  rotationAxis[0] = m_CameraAxis[0];
  rotationAxis[1] = m_CameraAxis[1];
  rotationAxis[2] = m_CameraAxis[2];

  // Check for custom transform
  if(m_CustomTransform)
  {
    rotateAmt = m_CustomTransformAmount.toDouble();
  }

  switch(m_ActionAxis)
  {
  case Axis::X:
    rotationAxis[0] = 1.0;
    rotationAxis[1] = 0.0;
    rotationAxis[2] = 0.0;
    break;
  case Axis::Y:
    rotationAxis[0] = 0.0;
    rotationAxis[1] = 1.0;
    rotationAxis[2] = 0.0;
    break;
  case Axis::Z:
    rotationAxis[0] = 0.0;
    rotationAxis[1] = 0.0;
    rotationAxis[2] = 1.0;
    break;
  case Axis::None:
  default:
    break;
  }


  // Rotate all selected filters
  VSAbstractFilter::FilterListType selection = getFilterSelection();
  for(VSAbstractFilter* filter : selection)
  {
	VSTransform* previousTransform = new VSTransform(*filter->getTransform());
	previousTransform->setLocalPosition(filter->getTransform()->getLocalPosition());
	previousTransform->setLocalScale(filter->getTransform()->getLocalScale());
	previousTransform->setLocalRotation(filter->getTransform()->getLocalRotation());
	m_PreviousTransforms.insert(std::make_pair(filter, previousTransform));
    filter->getTransform()->rotate(rotateAmt, rotationAxis);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginRotation()
{
  m_PreviousTransforms.clear();
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
  VSAbstractFilter::FilterListType selection = getFilterSelection();
  for(VSAbstractFilter* filter : selection)
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
	m_PreviousTransforms.clear();
    return;
  }

  vtkRenderWindowInteractor* iren = this->Interactor;
  double disp_obj_center[3];
  this->ComputeWorldToDisplay(m_InitialCenter[0], m_InitialCenter[1], m_InitialCenter[2], disp_obj_center);

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

  // Check for custom transform
  if(m_CustomTransform)
  {
    deltaScale = m_CustomTransformAmount.toDouble();
  }

  if(deltaScale == 0.0)
  {
	deltaScale = 1.0;
  }

  double scaleVector[3];
  scaleVector[0] = deltaScale;
  scaleVector[1] = deltaScale;
  scaleVector[2] = deltaScale;

  switch(m_ActionAxis)
  {
  case Axis::X:
    scaleVector[1] = 1.0;
    scaleVector[2] = 1.0;
    break;
  case Axis::Y:
    scaleVector[0] = 1.0;
    scaleVector[2] = 1.0;
    break;
  case Axis::Z:
    scaleVector[0] = 1.0;
    scaleVector[1] = 1.0;
    break;
  case Axis::None:
  default:
    break;
  }

  // Scale all selected filters
  VSAbstractFilter::FilterListType selection = getFilterSelection();
  for(VSAbstractFilter* filter : selection)
  {
	VSTransform* previousTransform = new VSTransform(*filter->getTransform());
	previousTransform->setLocalPosition(filter->getTransform()->getLocalPosition());
	previousTransform->setLocalScale(filter->getTransform()->getLocalScale());
	previousTransform->setLocalRotation(filter->getTransform()->getLocalRotation());
	m_PreviousTransforms.insert(std::make_pair(filter, previousTransform));
    filter->getTransform()->scale(scaleVector);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::beginScaling()
{
  m_PreviousTransforms.clear();
  if(nullptr == m_ActiveFilter || nullptr == m_ActiveProp)
  {
    return;
  }

  if(m_ActiveFilter)
  {
    vtkRenderWindowInteractor* iren = this->Interactor;
    double* obj_center = m_ActiveFilter->getTransform()->getPosition();
    double disp_obj_center[3];
    this->ComputeWorldToDisplay(obj_center[0], obj_center[1], obj_center[2], disp_obj_center);

    int* currentMousePos = iren->GetEventPosition();
    double currentDelta[2];
    for(int i = 0; i < 2; i++)
    {
      currentDelta[i] = currentMousePos[i] - disp_obj_center[i];
    }

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
  VSAbstractFilter::FilterListType selection = getFilterSelection();
  for(VSAbstractFilter* filter : selection)
  {
    filter->getTransform()->scale(1.0 / m_ScaleAmt);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::resetTransform()
{
  VSAbstractFilter::FilterListType selection = getFilterSelection();

  for(VSAbstractFilter* filter : selection)
  {
	VSTransform* defaultTransform = m_ViewWidget->getFilterViewSettings(filter)->getDefaultTransform();
    filter->getTransform()->setLocalPosition(defaultTransform->getLocalPosition());
    filter->getTransform()->setLocalScale(defaultTransform->getLocalScale());
    filter->getTransform()->setLocalRotation(defaultTransform->getLocalRotation());
  }
  setActionType(ActionType::None);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::undoTransform()
{
  VSAbstractFilter::FilterListType selection = getFilterSelection();

  for(VSAbstractFilter* filter : selection)
  {
	VSTransform* previousTransform = m_PreviousTransforms.at(filter);
	if(previousTransform != nullptr)
	{
	  VSTransform* lastUndoneTransform = new VSTransform(filter->getTransform());
	  lastUndoneTransform->setLocalPosition(filter->getTransform()->getLocalPosition());
	  lastUndoneTransform->setLocalScale(filter->getTransform()->getLocalScale());
	  lastUndoneTransform->setLocalRotation(filter->getTransform()->getLocalRotation());
	  m_LastUndoneTransforms.insert(std::make_pair(filter, lastUndoneTransform));
	  filter->getTransform()->setLocalPosition(previousTransform->getLocalPosition());
	  filter->getTransform()->setLocalScale(previousTransform->getLocalScale());
	  filter->getTransform()->setLocalRotation(previousTransform->getLocalRotation());
	}
  }
  setActionType(ActionType::None);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::redoTransform()
{
  VSAbstractFilter::FilterListType selection = getFilterSelection();

  for(VSAbstractFilter* filter : selection)
  {
	VSTransform* lastUndoneTransform = m_LastUndoneTransforms.at(filter);
	if(lastUndoneTransform != nullptr)
	{
	  // Save the lastUndoneTransform as previousTransform
	  VSTransform* previousTransform = new VSTransform(*lastUndoneTransform);
	  previousTransform->setLocalPosition(lastUndoneTransform->getLocalPosition());
	  previousTransform->setLocalScale(lastUndoneTransform->getLocalScale());
	  previousTransform->setLocalRotation(lastUndoneTransform->getLocalRotation());
	  m_PreviousTransforms.insert(std::make_pair(filter, previousTransform));
	  filter->getTransform()->setLocalPosition(lastUndoneTransform->getLocalPosition());
	  filter->getTransform()->setLocalScale(lastUndoneTransform->getLocalScale());
	  filter->getTransform()->setLocalRotation(lastUndoneTransform->getLocalRotation());
	}
  }
  setActionType(ActionType::None);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::selectAllFilters()
{
  VSAbstractFilter::FilterListType allFilters;
  if(m_ActiveFilter != nullptr)
  {
    if(m_ActiveFilter->getParentFilter() != nullptr)
    {
      allFilters = m_ActiveFilter->getParentFilter()->getChildren();
    }
    else
    {
      allFilters = m_ActiveFilter->getChildren();
    }
  }
  else
  {
    allFilters = m_ViewWidget->getFilterViewModel()->getAllFilters();
  }
  for(VSAbstractFilter* filter : allFilters)
  {
    if(filter->getParentFilter() == nullptr)
    {
      continue;
    }
    vtkProp3D* prop = m_ViewWidget->getFilterViewSettings(filter)->getActor();

    if(nullptr == prop || nullptr == filter)
    {
      continue;
    }

    addSelection(filter, prop);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::deselectAllFilters()
{
  for(VSAbstractFilter* filter : getFilterSelection())
  {
    removeSelection(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::updateTransformText()
{
  QString transformText;

  // Add action type
  switch(m_ActionType)
  {
  case ActionType::Translate:
    transformText.append("Translating");
    break;
  case ActionType::Rotate:
    transformText.append("Rotating");
    break;
  case ActionType::Scale:
    transformText.append("Scaling");
    break;
  case ActionType::ResetTransform:
  case ActionType::None:
    break;
  }

  // Add axis
  if(m_ActionType != ActionType::None && m_ActionType != ActionType::ResetTransform)
  {
    bool axisSelected = true;
    switch(m_ActionAxis)
    {
    case Axis::X:
      if(m_ActionType == ActionType::Rotate)
      {
        transformText.append(" about the X axis ");
      }
      else
      {
        transformText.append(" along the X axis ");
      }
      break;
    case Axis::Y:
      if(m_ActionType == ActionType::Rotate)
      {
        transformText.append(" about the Y axis");
      }
      else
      {
        transformText.append(" along the Y axis");
      }
      break;
    case Axis::Z:
      if(m_ActionType == ActionType::Rotate)
      {
        transformText.append(" about the Z axis");
      }
      else
      {
        transformText.append(" along the Z axis");
      }
      break;
    case Axis::None:
      axisSelected = false;
      break;
    }

    if(m_CustomTransformAmount.isEmpty())
    {
      if(axisSelected)
      {
        transformText.append("\nPress Space to save changes | Press Shift-Esc to Cancel | Press Ctrl-Z to undo or Alt-Z to reset the transform");
        transformText.append("\nPress Esc to cancel axis selection");
      }
      else
      {
        transformText.append("\nPress X, Y, or Z to snap transform to axis | Press Space to save changes | Press Esc to Cancel | Press Ctrl-Z to undo or Alt-Z to reset the transform");
      }
      transformText.append(" \nUse numerical keys to enter units/degrees for transform (- for negative values and . for precision");
    }
  }
  else
  {
    transformText.append("Press T to Translate, R to Rotate or S to Scale. Press Alt-Z to reset the transform");
  }
  // Add custom transform amount (if any)
  if(!m_CustomTransformAmount.isEmpty())
  {
    transformText.append(" ");
    transformText.append(m_CustomTransformAmount);
    if(m_ActionType == ActionType::Rotate)
    {
      transformText.append(" degrees");
    }
    else
    {
      transformText.append(" units");
    }
    transformText.append("\nPress Space to save changes | Press Shift-Esc to Cancel | Press Ctrl-Z to undo or Alt-Z to reset the transform");
    transformText.append("\nPress Esc to cancel numerical entry");
  }
  VSViewWidget* viewWidget = dynamic_cast<VSViewWidget*>(m_ViewWidget);
  viewWidget->updateTransformText(transformText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInteractorStyleFilterCamera::determineSubsampling()
{
  VTK_PTR(vtkCamera) camera = m_ViewWidget->getVisualizationWidget()->getRenderer()->GetActiveCamera();
  double cameraPosition[3];
  if(camera)
  {
    camera->GetPosition(cameraPosition);
  }

  // Check the memory available vs memory required for current subsampling rate
  std::map<VSAbstractFilter*, VSFilterViewSettings*> allFilterViewSettings = m_ViewWidget->getAllFilterViewSettings();
  bool isSIMPL = false;
  VSSIMPLDataContainerFilter* simplDataContainerFilter;
  VSDataSetFilter* imageDatasetFilter;
  VSFilterViewSettings::Collection filterViewSettingsCollection;
  double distanceToPlane;
  for(auto iter = allFilterViewSettings.begin(); iter != allFilterViewSettings.end(); iter++)
  {
    filterViewSettingsCollection.push_back(iter->second);
    if(!isSIMPL)
    {
	  try {
		simplDataContainerFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(iter->first);
		imageDatasetFilter = dynamic_cast<VSDataSetFilter*>(iter->first);
	  }
	  catch(const std::bad_cast& badCastException)
	  {
		qDebug() << badCastException.what();
	  }
      isSIMPL = simplDataContainerFilter && iter->second->isFlat();
      if(isSIMPL || imageDatasetFilter)
      {
        double position[3];
        VTK_PTR(vtkProp3D) actor = iter->second->getActor();
        if(actor)
        {
          actor->GetPosition(position);
        }
        VTK_PTR(vtkPlane) plane = VTK_PTR(vtkPlane)::New();
        if(plane)
        {
          plane->SetOrigin(position);

          // Check distance to plane from camera
          distanceToPlane = plane->DistanceToPlane(cameraPosition);
        }
      }
    }
  }

  // Only works if this is a VSSIMPLDataContainerFilter
  if(isSIMPL)
  {
    int subsamplingRate = 1;

    if(distanceToPlane > 10.0)
    {
      subsamplingRate = (distanceToPlane) / 1000;
      if(subsamplingRate > 100)
      {
        subsamplingRate = 100;
      }
    }

    if(subsamplingRate > 1)
    {
      VSFilterViewSettings::SetSubsampling(filterViewSettingsCollection, subsamplingRate);
    }
  }
}
