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

#include <QtCore/QPointF>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "ITKImageProcessing/ITKImageProcessingFilters/ITKImageReader.h"
#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

// our PIMPL private class
class ITKImportMontagePrivate;

struct ITKMontageCache
{
  ITKImageReader::Pointer imageReader;
  QString filePath;
  int row;
  int col;
  QPointF coords;
};

/**
 * @brief The ITKImportMontage class. See [Filter documentation](@ref ITKImportMontage) for details.
 */
class ITKImageProcessing_EXPORT ITKImportMontage : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKImportMontage SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(QString DataContainerPrefix READ getDataContainerPrefix WRITE setDataContainerPrefix)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString AttributeArrayName READ getAttributeArrayName WRITE setAttributeArrayName)
  PYB11_PROPERTY(bool ChangeOrigin READ getChangeOrigin WRITE setChangeOrigin)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(bool ChangeSpacing READ getChangeSpacing WRITE setChangeSpacing)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(QString MontageInformation READ getMontageInformation)
  PYB11_PROPERTY(int RowCount READ getRowCount)
  PYB11_PROPERTY(int ColumnCount READ getColumnCount)
  Q_DECLARE_PRIVATE(ITKImportMontage)

public:
  SIMPL_SHARED_POINTERS(ITKImportMontage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKImportMontage, AbstractFilter)

  ~ITKImportMontage() override;

  SIMPL_FILTER_PARAMETER(QString, DataContainerPrefix)
  Q_PROPERTY(QString DataContainerPrefix READ getDataContainerPrefix WRITE setDataContainerPrefix)

  SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, AttributeArrayName)
  Q_PROPERTY(QString AttributeArrayName READ getAttributeArrayName WRITE setAttributeArrayName)

  SIMPL_FILTER_PARAMETER(bool, ChangeOrigin)
  Q_PROPERTY(bool ChangeOrigin READ getChangeOrigin WRITE setChangeOrigin)

  SIMPL_FILTER_PARAMETER(FloatVec3Type, Origin)
  Q_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)

  SIMPL_FILTER_PARAMETER(bool, UsePixelCoordinates)
  Q_PROPERTY(bool UsePixelCoordinates READ getUsePixelCoordinates WRITE setUsePixelCoordinates)

  SIMPL_FILTER_PARAMETER(bool, ChangeSpacing)
  Q_PROPERTY(bool ChangeSpacing READ getChangeSpacing WRITE setChangeSpacing)

  SIMPL_FILTER_PARAMETER(FloatVec3Type, Spacing)
  Q_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)

  SIMPL_GET_PROPERTY(int, RowCount)
  Q_PROPERTY(int RowCount READ getRowCount)

  SIMPL_GET_PROPERTY(int, ColumnCount)
  Q_PROPERTY(int ColumnCount READ getColumnCount)

  SIMPL_FILTER_PARAMETER(int32_t, LengthUnit)
  Q_PROPERTY(int32_t LengthUnit READ getLengthUnit WRITE setLengthUnit)

  SIMPL_GET_PROPERTY(QString, MontageInformation)
  Q_PROPERTY(QString MontageInformation READ getMontageInformation)

  using MontageCacheVector = std::vector<ITKMontageCache>;

  SIMPL_PIMPL_PROPERTY_DECL(ITKImportMontage::MontageCacheVector, MontageCacheVector)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief preflight Reimplemented from @see AbstractFilter class
   */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  SIMPL_SET_PROPERTY(int, RowCount)
  SIMPL_SET_PROPERTY(int, ColumnCount)

  ITKImportMontage();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief appendToCache
   * @param reader
   * @param filePath
   * @param coords
   * @param row
   * @param col
   */
  void appendToCache(const ITKImageReader::Pointer &reader, const QString &filePath, QPointF coords, int row, int col, FloatVec3Type spacing);

  /**
   * @brief readImageFile
   * @param filePath
   * @param coords
   * @param row
   * @param col
   */
  void readImageFile(const QString &filePath, QPointF coords, int row, int col);

  /**
   * @brief readImagesFromCache
   */
  void readImagesFromCache();

  /**
   * @brief adjustOriginAndSpacing
   */
  void adjustOriginAndSpacing();

private:
  QScopedPointer<ITKImportMontagePrivate> const d_ptr;

  QString m_MontageInformation;

  int m_RowCount = 0;
  int m_ColumnCount = 0;

public :
  ITKImportMontage(const ITKImportMontage&) = delete; // Copy Constructor Not Implemented
  ITKImportMontage(ITKImportMontage&&) = delete;                   // Move Constructor Not Implemented
  ITKImportMontage& operator=(const ITKImportMontage&) = delete;   // Copy Assignment Not Implemented
  ITKImportMontage& operator=(ITKImportMontage&&) = delete;        // Move Assignment Not Implemented
};

