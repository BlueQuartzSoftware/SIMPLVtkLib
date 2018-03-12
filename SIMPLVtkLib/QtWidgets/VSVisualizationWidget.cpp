/* ============================================================================
* Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include "VSVisualizationWidget.h"

#include <QtWidgets/QFileDialog>

#include <vtkAutoInit.h>
#include <vtkInteractionStyleModule.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)

#include <vtkAxesActor.h>
#include <vtkBMPWriter.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkFollower.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGWriter.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPNGWriter.h>
#include <vtkPointPicker.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisualizationWidget::VSVisualizationWidget(QWidget* parent, unsigned int numLayers)
: QVTKOpenGLWidget(parent)
, m_NumRenderLayers(numLayers)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::setupGui()
{
  initializeRendererAndAxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::initializeRendererAndAxes()
{
  if(nullptr == m_Renderer.Get())
  {
    m_Renderer = VTK_PTR(vtkRenderer)::New();
    m_Renderer->SetLayer(0);

    double bgColor[3] = {0.3, 0.3, 0.35};
    m_Renderer->SetBackground(bgColor);

    VTK_PTR(vtkPointPicker) pointPicker = VTK_PTR(vtkPointPicker)::New();
    pointPicker->SetTolerance(0.00025);

    vtkGenericOpenGLRenderWindow* ren = vtkGenericOpenGLRenderWindow::New();
    SetRenderWindow(ren);
    ren->Finalize();
    ren->SetMapped(1);
    ren->SetSize(width(), height());
    ren->SetPosition(x(), y());

    QVTKInteractor* iren = QVTKInteractor::New();
    iren->SetUseTDx(false);
    ren->SetInteractor(iren);
    iren->Initialize();

    vtkInteractorStyle* style = vtkInteractorStyleTrackballCamera::New();
    iren->SetInteractorStyle(style);

    iren->Delete();
    style->Delete();

    GetRenderWindow()->GetInteractor()->SetPicker(pointPicker);
    GetRenderWindow()->SetNumberOfLayers(m_NumRenderLayers);
    GetRenderWindow()->AddRenderer(m_Renderer);
  }

  if(nullptr == m_OrientationWidget.Get())
  {
    VTK_NEW(vtkAxesActor, axes);

    m_OrientationWidget = vtkOrientationMarkerWidget::New();
    m_OrientationWidget->SetOutlineColor(0.93, 0.57, 0.13);
    m_OrientationWidget->SetOrientationMarker(axes);
    m_OrientationWidget->SetInteractor(GetRenderWindow()->GetInteractor());
    m_OrientationWidget->SetEnabled(1);
    m_OrientationWidget->InteractiveOff();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::copy(VSVisualizationWidget* other)
{
  // Copy camera values from another visualization widget
  vtkCamera* otherCamera = other->m_Renderer->GetActiveCamera();
  vtkCamera* thisCamera = m_Renderer->GetActiveCamera();

  thisCamera->SetPosition(otherCamera->GetPosition());
  thisCamera->SetFocalPoint(otherCamera->GetFocalPoint());

  // TODO: Copy vtkWidget values from another visualization widget
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkRenderer) VSVisualizationWidget::getRenderer()
{
  return m_Renderer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::render()
{
  if(GetRenderWindow() && GetRenderWindow()->GetInteractor())
  {
    GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::clearRenderWindow()
{
  m_Renderer->RemoveAllViewProps();
  m_Renderer->Render();
  GetRenderWindow()->GetInteractor()->Render();
  m_Renderer->ResetCamera();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::saveScreenshot(QString fileName)
{
  VTK_NEW(vtkWindowToImageFilter, screenshotFilter);
  screenshotFilter->SetInput(m_Renderer->GetRenderWindow());
  screenshotFilter->SetMagnification(1);
  screenshotFilter->SetInputBufferTypeToRGBA();
  screenshotFilter->ReadFrontBufferOff();
  screenshotFilter->Update();

  VTK_PTR(vtkImageWriter) imageWriter;

  if(fileName.endsWith(".png"))
  {
    imageWriter = VTK_PTR(vtkPNGWriter)::New();
  }
  else if(fileName.endsWith(".jpg"))
  {
    imageWriter = VTK_PTR(vtkJPEGWriter)::New();
  }
  else if(fileName.endsWith(".bmp"))
  {
    imageWriter = VTK_PTR(vtkBMPWriter)::New();
  }
  else
  {
    return;
  }

  imageWriter->SetFileName(fileName.toStdString().c_str());
  imageWriter->SetInputConnection(screenshotFilter->GetOutputPort());
  imageWriter->Write();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::saveScreenshotPath()
{
  QString filter = "PNG Image Data (*.png);;JPEG Image Data (*jpg);;Bitmap Image Data(*bmp)";

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Screenshot"), QDir::currentPath(), filter);

  if(fileName.isEmpty())
  {
    return;
  }

  else if(fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".bmp"))
  {
    saveScreenshot(fileName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::resetCamera()
{
  if(m_Renderer)
  {
    m_Renderer->ResetCamera();
    render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::getCameraFocalPointAndDistance(double* focalPoint, double& distance)
{
  if(nullptr == m_Renderer)
  {
    return;
  }

  double* newFocalPoint = m_Renderer->GetActiveCamera()->GetFocalPoint();

  for(int i = 0; i < 3; i++)
  {
    focalPoint[i] = newFocalPoint[i];
  }

  double* position = m_Renderer->GetActiveCamera()->GetPosition();
  distance = sqrt(pow(position[0] - focalPoint[0], 2) + pow(position[1] - focalPoint[1], 2) + pow(position[2] - focalPoint[2], 2));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::camXPlus()
{
  if(nullptr == m_Renderer)
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = m_Renderer->GetActiveCamera();
  double* focalPoint = new double[3];
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0] - distance, focalPoint[1], focalPoint[2]);
  camera->SetViewUp(0.0, 0.0, 1.0);

  delete[] focalPoint;

  if(m_Renderer->GetRenderWindow() && m_Renderer->GetRenderWindow()->GetInteractor())
  {
    m_Renderer->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::camYPlus()
{
  if(nullptr == m_Renderer)
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = m_Renderer->GetActiveCamera();
  double* focalPoint = new double[3];
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0], focalPoint[1] - distance, focalPoint[2]);
  camera->SetViewUp(0.0, 0.0, 1.0);

  delete[] focalPoint;

  if(m_Renderer->GetRenderWindow() && m_Renderer->GetRenderWindow()->GetInteractor())
  {
    m_Renderer->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::camZPlus()
{
  if(nullptr == m_Renderer)
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = m_Renderer->GetActiveCamera();
  double* focalPoint = new double[3];
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] - distance);
  camera->SetViewUp(0.0, 1.0, 0.0);

  delete[] focalPoint;

  if(m_Renderer->GetRenderWindow() && m_Renderer->GetRenderWindow()->GetInteractor())
  {
    m_Renderer->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::camXMinus()
{
  if(nullptr == m_Renderer)
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = m_Renderer->GetActiveCamera();
  double* focalPoint = new double[3];
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0] + distance, focalPoint[1], focalPoint[2]);
  camera->SetViewUp(0.0, 0.0, 1.0);

  delete[] focalPoint;

  if(m_Renderer->GetRenderWindow() && m_Renderer->GetRenderWindow()->GetInteractor())
  {
    m_Renderer->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::camYMinus()
{
  if(nullptr == m_Renderer)
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = m_Renderer->GetActiveCamera();
  double focalPoint[3] = {0.0, 0.0, 0.0};
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0], focalPoint[1] + distance, focalPoint[2]);
  camera->SetViewUp(0.0, 0.0, 1.0);

  if(m_Renderer->GetRenderWindow() && m_Renderer->GetRenderWindow()->GetInteractor())
  {
    m_Renderer->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::camZMinus()
{
  if(nullptr == m_Renderer)
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = m_Renderer->GetActiveCamera();
  double focalPoint[3] = { 0.0, 0.0, 0.0};
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + distance);
  camera->SetViewUp(0.0, 1.0, 0.0);

  if(m_Renderer->GetRenderWindow() && m_Renderer->GetRenderWindow()->GetInteractor())
  {
    m_Renderer->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationWidget::mousePressEvent(QMouseEvent* event)
{
  QVTKOpenGLWidget::mousePressEvent(event);

  emit mousePressed();
}
