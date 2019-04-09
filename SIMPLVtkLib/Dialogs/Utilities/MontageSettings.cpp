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

#include "MontageSettings.h"
#include <QtCore/qdebug.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSettings::MontageSettings()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSettings::MontageSettings(const MontageSettings& model)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QObject* MontageSettings::parentObject() const
{
  return QObject::parent();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t MontageSettings::getGridSizeX() const
{
  return m_gridSizeX;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setGridSizeX(uint64_t gridSizeX)
{
  m_gridSizeX = gridSizeX;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t MontageSettings::getGridSizeY() const
{
  return m_gridSizeY;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setGridSizeY(uint64_t gridSizeY)
{
  m_gridSizeY = gridSizeY;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MontageSettings::getTileOverlap() const
{
  return m_tileOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setTileOverlap(double tileOverlap)
{
  m_tileOverlap = tileOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MontageSettings::getIncreaseOverlap() const
{
  return m_increaseOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setIncreaseOverlap(double increaseOverlap)
{
  m_increaseOverlap = increaseOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t MontageSettings::getFirstFileIndex() const
{
  return m_firstFileIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setFirstFileIndex(uint64_t firstFileIndex)
{
  m_firstFileIndex = firstFileIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t MontageSettings::getFrameRangeToCompare() const
{
  return m_frameRangeToCompare;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setFrameRangeToCompare(uint64_t frameRangeToCompare)
{
  m_frameRangeToCompare = frameRangeToCompare;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir MontageSettings::getImagesDirectory() const
{
  return m_imagesDirectory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setImagesDirectory(QDir imagesDirectory)
{
  m_imagesDirectory = imagesDirectory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageSettings::getFileNames() const
{
  return m_fileNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setFileNames(QString fileNames)
{
  m_fileNames = fileNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageSettings::getLayoutFile() const
{
  return m_layoutFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setLayoutFile(QString layoutFile)
{
  m_layoutFile = layoutFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageSettings::getMultiSeriesFile() const
{
  return m_multiSeriesFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setMultiSeriesFile(QString multiSeriesFile)
{
  m_multiSeriesFile = multiSeriesFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageSettings::getOutputFileName() const
{
  return m_outputFileName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setOutputFileName(QString outputFileName)
{
  m_outputFileName = outputFileName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfo_t MontageSettings::getFileListInfo() const
{
  return m_fileListInfo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setFileListInfo(FileListInfo_t fileListInfo)
{
  m_fileListInfo = fileListInfo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MontageSettings::getRegressionThreshold() const
{
  return m_regressionThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setRegressionThreshold(double regressionThreshold)
{
  m_regressionThreshold = regressionThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MontageSettings::getMaxDisplacementThreshold() const
{
  return m_maxDisplacementThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setMaxDisplacementThreshold(double maxDisplacementThreshold)
{
  m_maxDisplacementThreshold = maxDisplacementThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MontageSettings::getAbsoluteDisplacementThreshold() const
{
  return m_absoluteDisplacementThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setAbsoluteDisplacementThreshold(double absoluteDisplacementThreshold)
{
  m_absoluteDisplacementThreshold = absoluteDisplacementThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isAddingTilesAsROI() const
{
  return m_addTilesAsROI;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setAddingTilesAsROI(bool addTilesAsROI)
{
  m_addTilesAsROI = addTilesAsROI;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isComputingOverlap() const
{
  return m_computeOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setComputingOverlap(bool computeOverlap)
{
  m_computeOverlap = computeOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isInvertingXCoords() const
{
  return m_invertXCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setInvertingXCoords(bool invertXCoords)
{
  m_invertXCoords = invertXCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isInvertingYCoords() const
{
  return m_invertYCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setInvertingYCoords(bool invertYCoords)
{
  m_invertYCoords = invertYCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isIgnoringZStatePos() const
{
  return m_ignoreZStatePos;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setIgnoringZStatePos(bool ignoreZStatePos)
{
  m_ignoreZStatePos = ignoreZStatePos;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isSubpixelAccuracy() const
{
  return m_subpixelAccuracy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setSubpixelAccuracy(bool subpixelAccuracy)
{
  m_subpixelAccuracy = subpixelAccuracy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isDownsamplingTiles() const
{
  return m_downsampleTiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setDownsamplingTiles(bool downsampleTiles)
{
  m_downsampleTiles = downsampleTiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isDisplayingFusion() const
{
  return m_displayFusion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setDisplayingFusion(bool displayFusion)
{
  m_displayFusion = displayFusion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isUsingVirtualInputImages() const
{
  return m_useVirtualInputImages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setUsingVirtualInputImages(bool useVirtualInputImages)
{
  m_useVirtualInputImages = useVirtualInputImages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isConfirmingFiles() const
{
  return m_confirmFiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setConfirmingFiles(bool confirmFiles)
{
  m_confirmFiles = confirmFiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MontageSettings::isIgnoringCalibration() const
{
  return m_ignoreCalibration;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setIgnoringCalibration(bool ignoreCalibration)
{
  m_ignoreCalibration = ignoreCalibration;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSettings::MontageType MontageSettings::getMontageType() const
{
  return m_montageType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setMontageType(MontageSettings::MontageType montageType)
{
  m_montageType = montageType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSettings::MontageOrder MontageSettings::getMontageOrder() const
{
  return m_montageOrder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setMontageOrder(MontageSettings::MontageOrder montageOrder)
{
  m_montageOrder = montageOrder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSettings::FusionMethod MontageSettings::getFusionMethod() const
{
  return m_fusionMethod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setFusionMethod(MontageSettings::FusionMethod fusionMethod)
{
  m_fusionMethod = fusionMethod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSettings::ComputationParameter MontageSettings::getComputationParameters() const
{
  return m_computationParameters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setComputationParameters(MontageSettings::ComputationParameter computationParameters)
{
  m_computationParameters = computationParameters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageSettings::ImageOutput MontageSettings::getImageOutputMethod() const
{
  return m_imageOutputMethod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageSettings::setImageOutputMethod(MontageSettings::ImageOutput imageOutputMethod)
{
  m_imageOutputMethod = imageOutputMethod;
}
