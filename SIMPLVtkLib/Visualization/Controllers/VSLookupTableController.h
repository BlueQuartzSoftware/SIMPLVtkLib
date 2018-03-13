/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <vector>

#include <vtkSmartPointer.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkColorTransferFunction;
class QJsonObject;

/**
* @class VSLookupTableController VSLookupTableController.h
* @brief This class handles the loading and manipulation of color data for use
* in the vtkMapper and vtkScalarBarActor classes.  The main functionality is
* loading json presets to be applied to vtkMappers through the color transfer 
* function.  Secondarily, the color scale can be inverted or adjusted to a given
* scale to match a given data array.
*/
class SIMPLVtkLib_EXPORT VSLookupTableController
{
public:
  struct RgbPos_t
  {
    double x, r, g, b;
  };

  /**
  * @brief Constructor
  */
  VSLookupTableController();

  /**
  * @brief Deconstructor
  */
  virtual ~VSLookupTableController();

  /**
  * @brief Create the color transfer function
  */
  void createColorTransferFunction();

  /**
  * @brief Returns the color transfer function
  * @return
  */
  vtkSmartPointer<vtkColorTransferFunction> getColorTransferFunction();

  /**
  * @brief Sets the color transfer function
  * @param colorTransferFunction
  */
  void setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction);

  /**
  * @brief Parse the given QJsonObject for RGB values and placement in the color transfer function
  * @param json
  */
  void parseRgbJson(const QJsonObject& json);

  /**
  * @brief Copies the values from another VSLookupTableController
  * @param other
  */
  void copy(const VSLookupTableController& other);

  /**
  * @brief Inverts the placement values for colors in the lookup table
  */
  void invert();

  /**
  * @brief normalizes the placement of colors in the RGB array so that the range becomes [0, 1]
  */
  void normalizePositions();

  /**
  * @brief Returns the placement range for values in the lookup table
  * @return
  */
  double* getRange();

  /**
  * @brief Sets the range to a new set of values
  * @param range
  */
  void setRange(double range[2]);

  /**
  * @brief Sets the range to a new set of values
  * @param min
  * @param max
  */
  void setRange(double min, double max);

  /**
  * @brief Adds a point to the lookup table
  * @param x
  * @param r
  * @param g
  * @param b
  */
  void addRgbPoint(double x, double r, double g, double b);

  /**
  * @brief Adds a point to the lookup table
  * @param x
  * @param rgb
  */
  void addRgbPoint(double x, double rgb[3]);
  
  /**
  * @brief Adds a point to the lookup table
  * @param rgbPos
  */
  void addRgbPoint(RgbPos_t rgbPos);

  /**
  * @brief Returns the number of specified color points in the table
  * @return
  */
  int getNumberOfRgbPoints();

  /**
  * @brief Returns the rgb color at the given index
  * @param index
  * @return
  */
  double* getRgbColor(int index);

  /**
  * @brief Sets the color at the given index
  * @param index
  * @param r
  * @param g
  * @param b
  */
  void setColor(int index, double r, double g, double b);

  /**
  * @brief Sets the color at the given index
  * @param index
  * @param rgb
  */
  void setColor(int index, double rgb[3]);

  /**
  * @brief Returns the position of the item at the given index
  * @param index
  * @return
  */
  double getRgbPosition(int index);

  /**
  * @brief Sets the position of the item at the given index
  * @param index
  * @param x
  */
  void setRgbPositon(int index, double x);

  /**
  * @brief Removes the color at the given index
  * @param index
  */
  void removeRgbPoint(int index);

  /**
  * @brief Sets the color to use for NAN values
  * @param color
  */
  void setNANColor(double color[3]);

  /**
  * @brief Returns the color used for NAN values
  * @return
  */
  double* getNANColor();

  /**
  * @brief Returns true if both VSLookupTableControllers are identical, otherwise returns false.
  * @param other
  * @return
  */
  bool equals(VSLookupTableController* other);

protected:
  /**
  * @brief Updates the color transfer function with the latest values
  */
  void update();

private:
  std::vector<RgbPos_t> m_BaseRgbPosVector;
  std::vector<RgbPos_t> m_RgbPosVector;
  static std::vector<RgbPos_t> m_DefaultRgbPosVector;

  double* m_Range;
  double* m_BaseRange;

  vtkSmartPointer<vtkColorTransferFunction> m_ColorTransferFunction;
};
