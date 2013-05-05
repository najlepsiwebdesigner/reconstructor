/**
 * This file is part of the rgbdemo project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Nicolas Burrus <nicolas@burrus.name>, (C) 2010, 2011
 */

#include "ModelAcquisitionWindow.h"
#include "ui_ModelAcquisitionWindow.h"

#include "GuiController.h"
#include "ModelAcquisitionController.h"

#include <ntk/camera/rgbd_frame_recorder.h>
#include <ntk/camera/rgbd_processor.h>

#include <ntk/utils/time.h>
#include <fstream>


#include <ntk/utils/opencv_utils.h>
#include <QFileDialog>
#include <QMessageBox>

ModelAcquisitionWindow::ModelAcquisitionWindow(GuiController& controller, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModelAcquisitionWindow),
    m_controller(controller),
    m_angle_delta(10),
    m_iteration(0)
{
  ui->setupUi(this);
//  ui->mesh_view->enableLighting();
//   ui->mesh_view->setShowGrid(true);+

}

ModelAcquisitionWindow::~ModelAcquisitionWindow()
{
  delete ui;
}

void ModelAcquisitionWindow::on_resetCamera_clicked()
{
  ui->mesh_view->resetCamera();
  ui->mesh_view->updateGL();
}

void ModelAcquisitionWindow::on_saveMeshButton_clicked()
{
  m_controller.modelAcquisitionController()->modeler().computeSurfaceMesh();
  m_controller.modelAcquisitionController()->modeler()
      .currentMesh().saveToPlyFile("scene_mesh.ply");
}

void ModelAcquisitionWindow::on_saveAsMeshButton_clicked()
{
  QString temp = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Standford Graphics Format Files (*.ply)"));

  m_controller.modelAcquisitionController()->modeler().computeSurfaceMesh();
  m_controller.modelAcquisitionController()->modeler().currentMesh().saveToPlyFile(temp.toUtf8().constData());

}

void ModelAcquisitionWindow::on_startButton_clicked()
{
  m_controller.modelAcquisitionController()->setPaused(false);
}

void ModelAcquisitionWindow::on_stopButton_clicked()
{
  m_controller.modelAcquisitionController()->setPaused(true);
}



void ModelAcquisitionWindow::on_startGrabButton_clicked(
        )
{

  m_controller.setGrabFrames(true);
}

void ModelAcquisitionWindow::on_stopGrabButton_clicked(
        )
{
  m_controller.setGrabFrames(false);
}

void ModelAcquisitionWindow::setOutputDirText(QString text){
    ui->outputDirText->insert(text);
}


void ModelAcquisitionWindow::on_resetButton_clicked()
{
  m_controller.modelAcquisitionController()->reset();
}

void ModelAcquisitionWindow::on_outputDirText_editingFinished()
{
  QString dir = ui->outputDirText->text();
  m_controller.frameRecorder()->setDirectory(dir.toStdString());
}


void ModelAcquisitionWindow::on_grabButton_clicked()
{
  m_controller.frameRecorder()->saveCurrentFrame(m_controller.lastImage());
}


void ModelAcquisitionWindow::on_actionQuit_triggered()
{
  m_controller.quit();
}



void ModelAcquisitionWindow::update(const ntk::RGBDImage& image)
{
   // display color image in this window
   ui->colorView->setImage(image.rgb());

  // display depth image in this window
   if (ui->depthView->isVisible())
   {
     double min_dist = m_controller.rgbdProcessor().minDepth();
     double max_dist = m_controller.rgbdProcessor().maxDepth();
     cv::Mat1f masked_distance; image.depth().copyTo(masked_distance);
     ntk::apply_mask(masked_distance, image.depthMask());
     cv::Mat3b depth_as_color;
     ntk::compute_color_encoded_depth(masked_distance, depth_as_color, &min_dist, &max_dist);
     ui->depthView->setImage(depth_as_color);
   }
    // ui->depthView->setImageAsColor(image.depth(), &min_dist, &max_dist);
    // ui->depthView->setImage(image.depth(), &min_dist, &max_dist);
    //  if (image.amplitude().data && ui->amplitudeView->isVisible())
    //    ui->amplitudeView->setImage(image.amplitude());
    //  if (image.intensity().data && ui->intensityView->isVisible())
    //    ui->intensityView->setImage(image.intensity());

   int x,y;
   ui->depthView->getLastMousePos(x,y);

//   ui->mesh_view->updateDisplayCenter();
//   m_controller.on_depth_mouse_moved(x,y);
}
