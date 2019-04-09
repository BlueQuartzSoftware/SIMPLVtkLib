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

#include "MontageSettings.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"

class VSRootFilter;

/**
 * @class TileConfigFileGenerator TileConfigFileGenerator.h
 * @brief This class handles the tile configuration file generation
 */
class SIMPLVtkLib_EXPORT TileConfigFileGenerator : public QObject
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   */
  TileConfigFileGenerator();

  /**
   * @brief Constructor
   * @param parent
   */
  TileConfigFileGenerator(FileListInfo_t fileListInfo, MontageSettings::MontageType montageType, MontageSettings::MontageOrder montageOrder, int gridSizeX, int gridSizeY, double tileOverlap,
                          QString outputFilename);

  /**
   * @brief Copy constructor
   * @param model
   */
  TileConfigFileGenerator(const TileConfigFileGenerator& model);

  /**
   * @brief Deconstructor
   */
  virtual ~TileConfigFileGenerator() = default;

  /**
   * @brief Returns the QObject parent since the parent() method is overloaded
   * @return
   */
  QObject* parentObject() const;

  /**
   * @brief
   */
  void buildTileConfigFile() const;

private:
  int m_gridSizeX;
  int m_gridSizeY;
  double m_tileOverlap;
  FileListInfo_t m_fileListInfo;
  MontageSettings::MontageType m_montageType;
  MontageSettings::MontageOrder m_montageOrder;
  QString m_outputFilename;
};

Q_DECLARE_METATYPE(TileConfigFileGenerator)
