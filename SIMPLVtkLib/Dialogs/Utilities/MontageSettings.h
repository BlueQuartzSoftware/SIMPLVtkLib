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

#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDir>
#include <QtCore/QSemaphore>

#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"

class VSRootFilter;

/**
 * @class MontageSettings MontageSettings.h SIMPLVtkLib/QtWidgets/MontageSettings.h
 * @brief This class handles the montage settings for the Generic Montage wizard.
 */
class SIMPLVtkLib_EXPORT MontageSettings : public QObject
{
  Q_OBJECT

  Q_PROPERTY(uint64_t gridSizeX READ getGridSizeX WRITE setGridSizeX)
  Q_PROPERTY(uint64_t gridSizeY READ getGridSizeY WRITE setGridSizeY)
  Q_PROPERTY(double tileOverlap READ getTileOverlap WRITE setTileOverlap)
  Q_PROPERTY(double increaseOverlap READ getIncreaseOverlap WRITE setIncreaseOverlap)
  Q_PROPERTY(uint64_t firstFileIndex READ getFirstFileIndex WRITE setFirstFileIndex)
  Q_PROPERTY(uint64_t frameRangeToCompare READ getFrameRangeToCompare WRITE setFrameRangeToCompare)
  Q_PROPERTY(QDir imagesDirectory READ getImagesDirectory WRITE setImagesDirectory)
  Q_PROPERTY(QString fileNames READ getFileNames WRITE setFileNames)
  Q_PROPERTY(QString layoutFile READ getLayoutFile WRITE setLayoutFile)
  Q_PROPERTY(QString multiSeriesFile READ getMultiSeriesFile WRITE setMultiSeriesFile)
  Q_PROPERTY(QString outputFileName READ getOutputFileName WRITE setOutputFileName)
  Q_PROPERTY(double regressionThreshold READ getRegressionThreshold WRITE setRegressionThreshold)
  Q_PROPERTY(double maxDisplacementThreshold READ getMaxDisplacementThreshold WRITE setMaxDisplacementThreshold)
  Q_PROPERTY(double absoluteDisplacementThreshold READ getAbsoluteDisplacementThreshold WRITE setAbsoluteDisplacementThreshold)
  Q_PROPERTY(bool addTilesAsROI READ isAddingTilesAsROI WRITE setAddingTilesAsROI)
  Q_PROPERTY(bool computeOverlap READ isComputingOverlap WRITE setComputingOverlap)
  Q_PROPERTY(bool invertXCoords READ isInvertingXCoords WRITE setInvertingXCoords)
  Q_PROPERTY(bool invertYCoords READ isInvertingYCoords WRITE setInvertingYCoords)
  Q_PROPERTY(bool ignoreZStatePos READ isIgnoringZStatePos WRITE setIgnoringZStatePos)
  Q_PROPERTY(bool subpixelAccuracy READ isSubpixelAccuracy WRITE setSubpixelAccuracy)
  Q_PROPERTY(bool downsampleTiles READ isDownsamplingTiles WRITE setDownsamplingTiles)
  Q_PROPERTY(bool displayFusion READ isDisplayingFusion WRITE setDisplayingFusion)
  Q_PROPERTY(bool useVirtualInputImages READ isUsingVirtualInputImages WRITE setUsingVirtualInputImages)
  Q_PROPERTY(bool confirmFiles READ isConfirmingFiles WRITE setConfirmingFiles)
  Q_PROPERTY(bool ignoreCalibration READ isIgnoringCalibration WRITE setIgnoringCalibration)
  Q_PROPERTY(FileListInfo_t fileListInfo READ getFileListInfo WRITE setFileListInfo)

public:
  enum MontageType : int
  {
    GridRowByRow,
    GridColByCol,
    GridSnakeByRows,
    GridSnakeByCols,
    FilenameDefined,
    Unknown,
    PositionsFromFile,
    SequentialImages
  };

  enum MontageOrder : int
  {
    RightAndDown,
    LeftAndDown,
    RightAndUp,
    LeftAndUp,
    DownAndRight,
    DownAndLeft,
    UpAndRight,
    UpAndLeft,
    DefinedByTileConfiguration,
    DefinedByImageMetadata
  };

  enum FusionMethod : int
  {
    LinearBlending,
    Average,
    Median,
    MaxIntensity,
    MinIntensity,
    IntensityOfRandomInputFile,
    DoNotFuseImages
  };

  enum ComputationParameter : int
  {
    SaveMemory,
    SaveComputationTime
  };

  enum ImageOutput : int
  {
    FuseAndDisplay,
    WriteToDisk
  };

  Q_ENUMS(MontageType)
  Q_ENUMS(MontageOrder)
  Q_ENUMS(FusionMethod)
  Q_ENUMS(ComputationParameter)
  Q_ENUMS(ImageOutput)

  Q_PROPERTY(MontageType montageType READ getMontageType WRITE setMontageType)
  Q_PROPERTY(MontageOrder montageOrder READ getMontageOrder WRITE setMontageOrder)
  Q_PROPERTY(FusionMethod fusionMethod READ getFusionMethod WRITE setFusionMethod)
  Q_PROPERTY(ComputationParameter computationParameters READ getComputationParameters WRITE setComputationParameters)
  Q_PROPERTY(ImageOutput imageOutputMethod READ getImageOutputMethod WRITE setImageOutputMethod)

  /**
   * @brief Constructor
   * @param parent
   */
  MontageSettings();

  /**
   * @brief Copy constructor
   * @param model
   */
  MontageSettings(const MontageSettings& model);

  /**
   * @brief Deconstructor
   */
  virtual ~MontageSettings() = default;

  /**
   * @brief Returns the QObject parent since the parent() method is overloaded
   * @return
   */
  QObject* parentObject() const;

  // Getters and Setters

  /**
   * @brief Returns the grid size x value
   * @return
   */
  uint64_t getGridSizeX() const;

  /**
   * @brief Sets the grid size x value
     @param gridSizeX
  */
  void setGridSizeX(uint64_t gridSizeX);

  /**
   * @brief Returns the grid size y value
   * @return
   */
  uint64_t getGridSizeY() const;

  /**
   * @brief Sets the grid size y value
   *  @param gridSizeY
   */
  void setGridSizeY(uint64_t gridSizeY);

  /**
   * @brief Returns the tile overlap value
   * @return
   */
  double getTileOverlap() const;

  /**
   * @brief Sets the increase in overlap
   * @param tileOverlap
   */
  void setTileOverlap(double tileOverlap);

  /**
   * @brief Returns the increase in overlap value
   * @return
   */
  double getIncreaseOverlap() const;

  /**
   * @brief Sets the tile overlap
   * @param increaseOverlap
   */
  void setIncreaseOverlap(double increaseOverlap);

  /**
   * @brief Returns the first file index
   * @return
   */
  uint64_t getFirstFileIndex() const;

  /**
   * @brief Sets the first file index
   * @param firstFileIndex
   */
  void setFirstFileIndex(uint64_t firstFileIndex);

  /**
   * @brief Returns the frame range to compare
   * @return
   */
  uint64_t getFrameRangeToCompare() const;

  /**
   * @brief Sets the frame range to compare
   * @param frameRangeToCompare
   */
  void setFrameRangeToCompare(uint64_t frameRangeToCompare);

  /**
   * @brief Returns the directory containing images
   * @return
   */
  QDir getImagesDirectory() const;

  /**
   * @brief Sets the images directory
   * @param imagesDirectory
   */
  void setImagesDirectory(QDir imagesDirectory);

  /**
   * @brief Returns the file names of the images
   * @return
   */
  QString getFileNames() const;

  /**
   * @brief Sets the file names for the images
   * @param fileNames
   */
  void setFileNames(QString fileNames);

  /**
   * @brief Returns the layout file
   * @return
   */
  QString getLayoutFile() const;

  /**
   * @brief Sets the layout file
   * @param layoutFile
   */
  void setLayoutFile(QString layoutFile);

  /**
   * @brief Returns the multi series file
   * @return
   */
  QString getMultiSeriesFile() const;

  /**
   * @brief Sets the multi series file
   * @param multiSeriesFile
   */
  void setMultiSeriesFile(QString multiSeriesFile);

  /**
   * @brief Returns the output file name
   * @return
   */
  QString getOutputFileName() const;

  /**
   * @brief Sets the file names for the output
   * @param outputFileName
   */
  void setOutputFileName(QString outputFileName);

  /**
   * @brief Returns the input file list
   * @return
   */
  FileListInfo_t getFileListInfo() const;

  /**
   * @brief Sets the file info for the input
   * @param fileListInfo
   */
  void setFileListInfo(FileListInfo_t fileListInfo);

  /**
   * @brief Returns the regression threshold
   * @return
   */
  double getRegressionThreshold() const;

  /**
   * @brief Sets the regression threshold
   * @param regressionThreshold
   */
  void setRegressionThreshold(double regressionThreshold);

  /**
   * @brief Returns the maximum/average displacement threshold
   * @return
   */
  double getMaxDisplacementThreshold() const;

  /**
   * @brief Sets the maximum/average displacement threshold
   * @param maxDisplacementThreshold
   */
  void setMaxDisplacementThreshold(double maxDisplacementThreshold);

  /**
   * @brief Returns the absolute displacement threshold
   * @return
   */
  double getAbsoluteDisplacementThreshold() const;

  /**
   * @brief Sets the absolute displacement threshold
   * @param absoluteDisplacementThreshold
   */
  void setAbsoluteDisplacementThreshold(double absoluteDisplacementThreshold);

  /**
   * @brief Returns true if adding tiles as ROIs. Returns false otherwise
   * @return
   */
  bool isAddingTilesAsROI() const;

  /**
   * @brief Sets whether to add tiles as ROI
   * @param addTilesAsROI
   */
  void setAddingTilesAsROI(bool addTilesAsROI);

  /**
   * @brief Returns true if computing overlap. Returns false otherwise
   * @return
   */
  bool isComputingOverlap() const;

  /**
   * @brief Sets whether to compute overlap
   * @param absoluteDisplacementThreshold
   */
  void setComputingOverlap(bool computeOverlap);

  /**
   * @brief Returns true if inverting X coordinates. Returns false otherwise
   * @return
   */
  bool isInvertingXCoords() const;

  /**
   * @brief Sets whether to invert X coordinates
   * @param invertXCoords
   */
  void setInvertingXCoords(bool invertXCoords);

  /**
   * @brief Returns true if inverting Y coordinates. Returns false otherwise
   * @return
   */
  bool isInvertingYCoords() const;

  /**
   * @brief  Sets whether to invert Y coordinates
   * @param invertYCoords
   */
  void setInvertingYCoords(bool invertYCoords);

  /**
   * @brief Returns true if ignoring Z state positions. Returns false otherwise
   * @return
   */
  bool isIgnoringZStatePos() const;

  /**
   * @brief Sets whether to ignore Z state positions
   * @param ignoreZStatePos
   */
  void setIgnoringZStatePos(bool ignoreZStatePos);

  /**
   * @brief Returns true if computing subpixel precise alignment between the two images. Returns false otherwise
   * @return
   */
  bool isSubpixelAccuracy() const;

  /**
   * @brief Sets whether to compute subpixel precise alignment between two images
   * @param subpixelAccuracy
   */
  void setSubpixelAccuracy(bool subpixelAccuracy);

  /**
   * @brief Returns true if downsampling tiles. Returns false otherwise
   * @return
   */
  bool isDownsamplingTiles() const;

  /**
   * @brief Sets whether to downsample tiles
   * @param downsampleTiles
   */
  void setDownsamplingTiles(bool downsampleTiles);

  /**
   * @brief Returns true if displaying fusion. Returns false otherwise
   * @return
   */
  bool isDisplayingFusion() const;

  /**
   * @brief Sets whether to display fusion
   * @param displayFusion
   */
  void setDisplayingFusion(bool displayFusion);

  /**
   * @brief Returns true if using virtual input images. Returns false otherwise
   * @return
   */
  bool isUsingVirtualInputImages() const;

  /**
   * @brief Sets whether to use virtual input images
   * @param useVirtualInputImages
   */
  void setUsingVirtualInputImages(bool useVirtualInputImages);

  /**
   * @brief Returns true if confirming files. Returns false otherwise
   * @return
   */
  bool isConfirmingFiles() const;

  /**
   * @brief Sets whether to confirm files
   * @param confirmFiles
   */
  void setConfirmingFiles(bool confirmFiles);

  /**
   * @brief Returns true if using virtual input images. Returns false otherwise
   * @return
   */
  bool isIgnoringCalibration() const;

  /**
   * @brief Sets whether to ignore calibration
   * @param ignoringCalibration
   */
  void setIgnoringCalibration(bool ignoringCalibration);

  /**
   * @brief Returns the montage type
   * @return
   */
  MontageType getMontageType() const;

  /**
   * @brief Sets the montage type
   * @param montageType
   */
  void setMontageType(MontageSettings::MontageType montageType);

  /**
   * @brief Returns the montage order
   * @return
   */
  MontageOrder getMontageOrder() const;

  /**
   * @brief Sets the montage order
   * @param montageOrder
   */
  void setMontageOrder(MontageSettings::MontageOrder montageOrder);

  /**
   * @brief Returns the fusion method
   * @return
   */
  FusionMethod getFusionMethod() const;

  /**
   * @brief Sets the fusion method
   * @param fusionMethod
   */
  void setFusionMethod(MontageSettings::FusionMethod fusionMethod);

  /**
   * @brief Returns the computation parameters
   * @return
   */
  ComputationParameter getComputationParameters() const;

  /**
   * @brief Sets the computation parameters
   * @param computationParameters
   */
  void setComputationParameters(MontageSettings::ComputationParameter computationParameters);

  /**
   * @brief Returns the image output method
   * @return
   */
  ImageOutput getImageOutputMethod() const;

  /**
   * @brief Sets the image output method
   * @param montageType
   */
  void setImageOutputMethod(MontageSettings::ImageOutput imageOutputMethod);

signals:

public slots:

private slots:

private:
  uint64_t m_gridSizeX = 3;
  uint64_t m_gridSizeY = 3;
  double m_tileOverlap = 20.0;
  double m_increaseOverlap = 10.0;
  uint64_t m_firstFileIndex = 1;
  uint64_t m_frameRangeToCompare = 0;
  QDir m_imagesDirectory;
  QString m_layoutFile;
  QString m_multiSeriesFile;
  QString m_fileNames;
  QString m_outputFileName;
  FileListInfo_t m_fileListInfo;
  double m_regressionThreshold = 0.30;
  double m_maxDisplacementThreshold = 2.50;
  double m_absoluteDisplacementThreshold = 3.50;
  bool m_addTilesAsROI = false;
  bool m_computeOverlap = false;
  bool m_invertXCoords = false;
  bool m_invertYCoords = false;
  bool m_ignoreZStatePos = false;
  bool m_subpixelAccuracy = false;
  bool m_downsampleTiles = false;
  bool m_displayFusion = false;
  bool m_useVirtualInputImages = false;
  bool m_confirmFiles = false;
  bool m_ignoreCalibration = false;

  MontageType m_montageType = GridRowByRow;
  MontageOrder m_montageOrder = RightAndDown;
  FusionMethod m_fusionMethod = LinearBlending;
  ComputationParameter m_computationParameters = SaveMemory;
  ImageOutput m_imageOutputMethod = FuseAndDisplay;
};

Q_DECLARE_METATYPE(MontageSettings)
Q_DECLARE_METATYPE(MontageSettings::MontageOrder)
Q_DECLARE_METATYPE(MontageSettings::MontageType)
