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
*    United States Air Force Prime Contract FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ITKImageReader.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "ITKImageProcessing/ITKImageProcessingConstants.h"
#include "ITKImageProcessing/ITKImageProcessingVersion.h"
#include "ITKImageProcessingPlugin.h"


/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ITKImageReaderPrivate
{
  Q_DISABLE_COPY(ITKImageReaderPrivate)
  Q_DECLARE_PUBLIC(ITKImageReader)
  ITKImageReader* const q_ptr;
  ITKImageReaderPrivate(ITKImageReader* ptr);
  QString m_FileNameCache;
  QDateTime m_LastRead;
  IDataArray::Pointer m_ImageArrayCache;
  ImageGeom::Pointer m_DCGeometryCache;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKImageReaderPrivate::ITKImageReaderPrivate(ITKImageReader* ptr)
: q_ptr(ptr)
, m_FileNameCache("")
{
}

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKImageReader::ITKImageReader()
: m_FileName("")
, m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_ImageDataArrayName(SIMPL::CellData::ImageData)
, d_ptr(new ITKImageReaderPrivate(this))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKImageReader::~ITKImageReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL_PIMPL_PROPERTY_DEF(ITKImageReader, QString, FileNameCache)
SIMPL_PIMPL_PROPERTY_DEF(ITKImageReader, QDateTime, LastRead)
SIMPL_PIMPL_PROPERTY_DEF(ITKImageReader, IDataArray::Pointer, ImageArrayCache)
SIMPL_PIMPL_PROPERTY_DEF(ITKImageReader, ImageGeom::Pointer, DCGeometryCache)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImageReader::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  QString supportedExtensions = ITKImageProcessingPlugin::getListSupportedReadExtensions();
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("File", FileName, FilterParameter::Parameter, ITKImageReader, supportedExtensions, "Image"));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ITKImageReader));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, ITKImageReader));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Image Data", ImageDataArrayName, DataContainerName, CellAttributeMatrixName, FilterParameter::CreatedArray, ITKImageReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImageReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFileName(reader->readString("FileName", getFileName()));
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImageReader::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImageReader::dataCheck()
{
  // check file name exists
  QFileInfo fi(getFileName());

  if(getFileName().isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: %1").arg(getFileName());
    setErrorCondition(-388, ss);
  }

  if(getDataContainerName().isEmpty())
  {
    setErrorCondition(-2, "No container name.");
    return;
  }

  DataContainer::Pointer container = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), DataContainerID);
  if(container.get() == nullptr)
  {
    setErrorCondition(-3, "No container.");
    return;
  }
  DataArrayPath dap(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getImageDataArrayName());
  readImage(dap, true);
  // If we got here, that means that there is no error
  clearErrorCode();
  clearWarningCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImageReader::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImageReader::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
  notifyProgressMessage(100, QString("Importing %1").arg(getFileName()));
  DataArrayPath dap(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getImageDataArrayName());
  readImage(dap, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ITKImageReader::newFilterInstance(bool copyFilterParameters) const
{
  ITKImageReader::Pointer filter = ITKImageReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImageReader::getCompiledLibraryName() const
{
  return ITKImageProcessingConstants::ITKImageProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImageReader::getBrandingString() const
{
  return "ITKImageProcessing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImageReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ITKImageProcessing::Version::Major() << "." << ITKImageProcessing::Version::Minor() << "." << ITKImageProcessing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImageReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ITKImageReader::getUuid()
{
  return QUuid("{653b7b5c-03cb-5b32-8c3e-3637745e5ff6}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImageReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImageReader::getHumanLabel() const
{
  return "ITK::Image Reader";
}

#define ITK_IMAGE_READER_CLASS_NAME ITKImageReader

#include "SIMPLib/ITK/itkGetComponentsDimensions.h"
#include "SIMPLib/ITK/itkInPlaceImageToDream3DDataFilter.h"

#include "SIMPLib/ITK/itkImageReaderHelper.cpp"
