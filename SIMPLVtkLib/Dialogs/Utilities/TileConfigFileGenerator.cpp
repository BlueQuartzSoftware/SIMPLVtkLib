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

#include <math.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>

#include "TileConfigFileGenerator.h"

#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/Utilities/StringOperations.h"

#include <QtCore/qdebug.h>
#include <qimage.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TileConfigFileGenerator::TileConfigFileGenerator()
{
  m_montageType = MontageSettings::MontageType::GridRowByRow;
  m_montageOrder = MontageSettings::MontageOrder::RightAndDown;
  m_gridSizeX = 3;
  m_gridSizeY = 3;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TileConfigFileGenerator::TileConfigFileGenerator(FileListInfo_t fileListInfo, MontageSettings::MontageType montageType, MontageSettings::MontageOrder montageOrder, int gridSizeX, int gridSizeY,
                                                 double tileOverlap, QString outputFilename)
{
  m_fileListInfo = fileListInfo;
  m_montageType = montageType;
  m_montageOrder = montageOrder;
  m_gridSizeX = gridSizeX;
  m_gridSizeY = gridSizeY;
  m_tileOverlap = tileOverlap;
  m_outputFilename = outputFilename;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TileConfigFileGenerator::TileConfigFileGenerator(const TileConfigFileGenerator& model)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QObject* TileConfigFileGenerator::parentObject() const
{
  return QObject::parent();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TileConfigFileGenerator::buildTileConfigFile() const
{
  // Prepare the input files
  bool hasMissingFiles = false;
  bool orderAscending = false;

  if(m_fileListInfo.Ordering == 0)
  {
    orderAscending = true;
  }
  else if(m_fileListInfo.Ordering == 1)
  {
    orderAscending = false;
  }
  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList =
      FilePathGenerator::GenerateFileList(m_fileListInfo.StartIndex, m_fileListInfo.EndIndex, m_fileListInfo.IncrementIndex, hasMissingFiles, orderAscending, m_fileListInfo.InputPath,
                                          m_fileListInfo.FilePrefix, m_fileListInfo.FileSuffix, m_fileListInfo.FileExtension, m_fileListInfo.PaddingDigits);
  if(fileList.empty())
  {
    return;
  }
  size_t availableFileCount = 0;
  float image_width = 100.0;
  float image_height = 100.0;
  bool image_dimensions_determined = false;
  for(QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    QString imageFName = *filepath;
    QFileInfo fi(imageFName);
    if(fi.exists())
    {
      availableFileCount++;
      if(!image_dimensions_determined)
      {
        QImage image(imageFName);
        image_width = image.width() * ((100.0 - m_tileOverlap) / 100.0);
        image_height = image.height() * ((100.0 - m_tileOverlap) / 100.0);
      }
    }
  }

  // Set up the output file
  std::ofstream outputFile;
  std::stringstream ss;
  ss << m_fileListInfo.InputPath.toStdString();
#ifdef _WIN32
  ss << "\\"; // WINDOWS ONLY
#else
  ss << "/";
#endif
  ss << m_outputFilename.toStdString();
  std::string outputFilePath = ss.str();
  outputFile.open(outputFilePath, std::ios_base::out);

  outputFile << "# Define the number of dimensions we are working on\n";
  outputFile << "dim = 2\n\n";
  outputFile << "# Define the image coordinates\n";

  // Snake / Down and right
  if(m_montageType == MontageSettings::MontageType::GridRowByRow || m_montageType == MontageSettings::MontageType::GridSnakeByRows)
  {
    // Boolean flags for whether the montage is a snake type pattern
    // and whether it is right and/or down in order
    bool isSnake = (m_montageType == MontageSettings::MontageType::GridSnakeByRows);
    bool isRight = (m_montageOrder == MontageSettings::MontageOrder::RightAndDown || m_montageOrder == MontageSettings::MontageOrder::RightAndUp);
    bool isDown = (m_montageOrder == MontageSettings::MontageOrder::RightAndDown || m_montageOrder == MontageSettings::MontageOrder::LeftAndDown);

    // Row 1 'start' points
    int i = isDown ? m_fileListInfo.StartIndex : m_gridSizeY - 1;  // Y
    int j = isRight ? m_fileListInfo.StartIndex : m_gridSizeX - 1; // X

    // Increment values
    int delta_i = 1;
    int delta_j = 1;

    for(int y = 0; y < m_gridSizeY; y++)
    {
      isDown ? i = y : i = m_gridSizeY - y;

      // Handle switch for every other row (Snake pattern)
      if(isSnake && y % 2 == 1)
      {
        isRight ? j = m_gridSizeX* y + m_gridSizeX - 1 : j = m_gridSizeX * y;
        isRight ? delta_j = -1 : delta_j = 1;
      }
      else if(isSnake)
      {
        isRight ? j = m_gridSizeX* y : j = m_gridSizeX * y + m_gridSizeX - 1;
        isRight ? delta_j = 1 : delta_j = -1;
      }
      else
      {
        j = m_gridSizeX * y;
      }

      j += m_fileListInfo.StartIndex;

      for(int x = 0; x < m_gridSizeX; x++)
      {
        outputFile << m_fileListInfo.FilePrefix.toStdString();
        outputFile << StringOperations::GeneratePaddedString(j, m_fileListInfo.PaddingDigits, '0').toStdString();
        outputFile << m_fileListInfo.FileSuffix.toStdString();
        outputFile << "." << m_fileListInfo.FileExtension.toStdString();
        outputFile << "; ; (";
        outputFile << float(x * image_width);
        outputFile << ", ";
        outputFile << float(y * image_height);
        outputFile << ")\n";
        j += delta_j;
      }
    }
  }
  else if(m_montageType == MontageSettings::MontageType::GridColByCol || m_montageType == MontageSettings::MontageType::GridSnakeByCols)
  {
    // Boolean flags for whether the montage is a snake type pattern
    // and whether it is right and/or down in order
    bool isSnake = (m_montageType == MontageSettings::MontageType::GridSnakeByCols);
    bool isRight = (m_montageOrder == MontageSettings::MontageOrder::DownAndRight || m_montageOrder == MontageSettings::MontageOrder::UpAndRight);
    bool isDown = (m_montageOrder == MontageSettings::MontageOrder::DownAndRight || m_montageOrder == MontageSettings::MontageOrder::DownAndLeft);

    // Row 1 'start' points
    int i = isDown ? m_fileListInfo.StartIndex : m_gridSizeY - 1;  // Y
    int j = isRight ? m_fileListInfo.StartIndex : m_gridSizeX - 1; // X

    // Increment values
    int delta_i = 1;
    int delta_j = 1;

    for(int x = 0; x < m_gridSizeX; x++)
    {
      isRight ? j = x : j = m_gridSizeX - x;

      // Handle switch for every other col (Snake pattern)
      if(isSnake && x % 2 == 1)
      {
        isDown ? i = m_gridSizeY* x + m_gridSizeY - 1 : i = m_gridSizeY * x;
        isDown ? delta_i = -1 : delta_i = 1;
      }
      else if(isSnake)
      {
        isDown ? i = m_gridSizeY* x : i = m_gridSizeY * x + m_gridSizeY - 1;
        isDown ? delta_i = 1 : delta_i = -1;
      }
      else
      {
        i = m_gridSizeY * x;
      }

      i += m_fileListInfo.StartIndex;

      for(int y = 0; y < m_gridSizeY; y++)
      {
        outputFile << m_fileListInfo.FilePrefix.toStdString();
        outputFile << StringOperations::GeneratePaddedString(i, m_fileListInfo.PaddingDigits, '0').toStdString();
        outputFile << m_fileListInfo.FileSuffix.toStdString();
        outputFile << "." << m_fileListInfo.FileExtension.toStdString();
        outputFile << "; ; (";
        outputFile << float(x * image_width);
        outputFile << ", ";
        outputFile << float(y * image_height);
        outputFile << ")\n";
        i += delta_i;
      }
    }
  }
}