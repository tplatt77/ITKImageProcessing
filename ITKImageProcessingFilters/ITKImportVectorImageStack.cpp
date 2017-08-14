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

#include "ITKImportVectorImageStack.h"

#include <string.h>

#include <limits>

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QImageReader>


#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

#include "ITKImageProcessing/FilterParameters/ImportVectorImageStackFilterParameter.h"
#include "ITKImageProcessing/ITKImageProcessingConstants.h"
#include "ITKImageProcessing/ITKImageProcessingVersion.h"
#include "ITKImageProcessing/ITKImageProcessingPlugin.h"
#include "ITKImageProcessing/ITKImageProcessingFilters/ITKImageProcessingHelpers.hpp"

#include "itksys/SystemTools.hxx"

// Include the MOC generated file for this class
#include "moc_ITKImportVectorImageStack.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKImportVectorImageStack::ITKImportVectorImageStack()
: AbstractFilter()
, m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
//, m_InputFileListInfo.StartIndex(0)
//, m_EndIndex(0)
//, m_InputFileListInfo.InputPath("")
//, m_FilePrefix("")
//, m_Separator("")
//, m_FileSuffix("")
//, m_FileExtension("")
//, m_PaddingDigits(0)
//, m_RefFrameZDir(SIMPL::RefFrameZDir::LowtoHigh)
//, m_ImageDataArrayName(SIMPL::CellData::VectorData)
, m_ImageDataArrayName(SIMPL::CellData::ImageData)
, m_VectorData(nullptr)
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKImportVectorImageStack::~ITKImportVectorImageStack()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImportVectorImageStack::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  //parameters.push_back(ImportVectorImageStackFilterParameter::New("Import Image Data", "ImageVector", getImageVector(), FilterParameter::Parameter, this));
  parameters.push_back(SIMPL_NEW_VECTORFILELISTINFO_FP("Input File List", InputFileListInfo, FilterParameter::Parameter, ITKImportVectorImageStack));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Name", DataContainerName, FilterParameter::CreatedArray, ITKImportVectorImageStack));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::CreatedArray, ITKImportVectorImageStack));
  parameters.push_back(SIMPL_NEW_STRING_FP("VectorData", ImageDataArrayName, FilterParameter::CreatedArray, ITKImportVectorImageStack));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImportVectorImageStack::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setImageDataArrayName(reader->readString("ImageDataArrayName", getImageDataArrayName()));
//  setStartIndex(reader->readValue("StartIndex", getStartIndex()));
//  setEndIndex(reader->readValue("EndIndex", getEndIndex()));
//  setStartComp(reader->readValue("StartComp", getStartComp()));
//  setEndComp(reader->readValue("EndComp", getEndComp()));
//  setPaddingDigits(reader->readValue("PaddingDigits", getPaddingDigits()));
//  setRefFrameZDir(reader->readValue("RefFrameZDir", getRefFrameZDir()));
//  setInputPath(reader->readString("InputPath", getInputPath()));
//  setFilePrefix(reader->readString("FilePrefix", getFilePrefix()));
//  setSeparator(reader->readString("Separator", getSeparator()));
//  setFileSuffix(reader->readString("FileSuffix", getFileSuffix()));
//  setFileExtension(reader->readString("FileExtension", getFileExtension()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setResolution(reader->readFloatVec3("Resolution", getResolution()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImportVectorImageStack::initialize()
{
  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;

  m_InputFileListInfo.FileExtension = QString("tif");
  m_InputFileListInfo.StartIndex = 0;
  m_InputFileListInfo.EndIndex = 0;
  m_InputFileListInfo.PaddingDigits = 0;
  m_InputFileListInfo.StartComp = 0;
  m_InputFileListInfo.EndComp = 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImportVectorImageStack::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  setWarningCondition(0);
  QString ss;

  if(m_InputFileListInfo.InputPath.isEmpty() == true)
  {
    ss = QObject::tr("The Input Directory must be set before executing this filter.");
    setErrorCondition(-13);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0 || nullptr == m.get())
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = ITKImageProcessing::Helpers::getVectorFileList<VectorFileListInfo_t>(m_InputFileListInfo);

  if(fileList.size() == 0)
  {
    ss.clear();
    QTextStream out(&ss);
    out << " No files have been selected for import. Have you set the input directory and other values so that input files will be generated?\n";
    out << "InputPath: " << m_InputFileListInfo.InputPath << "\n";
    out << "FilePrefix: " << m_InputFileListInfo.FilePrefix << "\n";
    out << "FileSuffix: " << m_InputFileListInfo.FileSuffix << "\n";
    out << "FileExtension: " << m_InputFileListInfo.FileExtension << "\n";
    out << "PaddingDigits: " << m_InputFileListInfo.PaddingDigits << "\n";
    out << "StartIndex: " << m_InputFileListInfo.StartIndex << "\n";
    out << "EndIndex: " << m_InputFileListInfo.EndIndex << "\n";
    out << "StartComp: " << m_InputFileListInfo.StartComp << "\n";
    out << "EndComp: " << m_InputFileListInfo.EndComp << "\n";
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else
  {
    // If we have RGB or RGBA Images then we are going to have to change things a bit.
    // We should read the file and see what we have? Of course Qt is going to read it up into
    // an RGB array by default
    int err = 0;
    QImage image(fileList[0]);
    int64_t dims[3] = {image.width(), image.height(), ((m_InputFileListInfo.EndIndex - m_InputFileListInfo.StartIndex) + 1)};
    size_t pixelBytes = 0;
    QImageReader reader((fileList[0]));
    QImage::Format format = reader.imageFormat();
    switch(format)
    {
    case QImage::Format_Indexed8:
#if(QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QImage::Format_Grayscale8:
#endif
      pixelBytes = 1;
      break;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
      pixelBytes = 4;
      break;
    default:
      pixelBytes = 0;
    }

    if(pixelBytes == 0)
    {
      ss = QObject::tr("Image format is of unsupported type (QImage::Format=%1). Imported images must be either grayscale, RGB, or ARGB").arg(format);
      setErrorCondition(-4400);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    /* Sanity check what we are trying to load to make sure it can fit in our address space.
      * Note that this does not guarantee the user has enough left, just that the
      * size of the volume can fit in the address space of the program
      */
    int64_t max = std::numeric_limits<int64_t>::max();

    if(dims[0] * dims[1] * dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.").arg((dims[0] * dims[1] * dims[2]));
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, err);
    }

    if(dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version."
                               " dim[0]=%1  dim[1]=%2  dim[2]=%3")
                       .arg(dims[0])
                       .arg(dims[1])
                       .arg(dims[2]);
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, err);
    }
    /* ************ End Sanity Check *************************** */

    m->getGeometryAs<ImageGeom>()->setDimensions(static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]));
    m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
    m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

    QVector<size_t> tDims(3, 0);
    for(int i = 0; i < 3; i++)
    {
      tDims[i] = dims[i];
    }
    AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
    if(getErrorCondition() < 0)
    {
      return;
    }

    // set up component dimensions for the vector image array
    QVector<size_t> arraydims(2);
    arraydims[0] = ((m_InputFileListInfo.EndComp - m_InputFileListInfo.StartComp) + 1);
    arraydims[1] = pixelBytes;

    // This would be for a gray scale image
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getImageDataArrayName());
    m_VectorDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, arraydims);
    if(nullptr != m_VectorDataPtr.lock().get())
    {
      m_VectorData = m_VectorDataPtr.lock()->getPointer(0);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImportVectorImageStack::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImportVectorImageStack::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  QVector<size_t> tDims = cellAttrMat->getTupleDimensions();
  size_t imageWidth = tDims[0];
  size_t imageHeight = tDims[1];
  // size_t numVecImages = tDims[2];

  size_t numComps = m_VectorDataPtr.lock()->getNumberOfComponents();
  QVector<size_t> cDims = m_VectorDataPtr.lock()->getComponentDimensions();
  // size_t vecDim = cDims[0];
  size_t pixDepth = cDims[1];

  size_t numCompsPerVecImage = imageHeight * imageWidth * numComps;

  int64_t comp = m_InputFileListInfo.StartComp;
  size_t compSpot;
  int64_t imageNum = m_InputFileListInfo.StartIndex;
  size_t imageSpot;
  size_t imageCompShift;

  uint8_t pixVal = 0;

  bool stackLowToHigh = false;

  if(SIMPL::RefFrameZDir::LowtoHigh == m_InputFileListInfo.Ordering)
  {
    stackLowToHigh = true;
  }
  else if(SIMPL::RefFrameZDir::HightoLow == m_InputFileListInfo.Ordering)
  {
    stackLowToHigh = false;
  }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = ITKImageProcessing::Helpers::getVectorFileList<VectorFileListInfo_t>(m_InputFileListInfo);

  for(QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    QString imageFName = *filepath;
    QString ss = QObject::tr("Importing file %1").arg(imageFName);
    notifyStatusMessage(getHumanLabel(), ss);

    QImage image(imageFName);
    if(image.isNull() == true)
    {
      setErrorCondition(-14000);
      notifyErrorMessage(getHumanLabel(), "Failed to load Image file", getErrorCondition());
    }

    compSpot = (comp - m_InputFileListInfo.StartComp);
    imageSpot = (imageNum - m_InputFileListInfo.StartIndex);
    imageCompShift = numCompsPerVecImage * imageSpot;
    size_t compStride = (compSpot * pixDepth);
    size_t src = 0;
    size_t dst = 0;
    size_t hStride;
    for(size_t i = 0; i < imageHeight; ++i)
    {
      uint8_t* source = image.scanLine(i);
      src = 0;
      hStride = i * imageWidth;
      for(size_t j = 0; j < imageWidth; ++j)
      {
        dst = imageCompShift + (numComps * (hStride + j)) + compStride;
        src = j * pixDepth;
        for(size_t k = 0; k < pixDepth; ++k)
        {
          pixVal = source[src + k];
          m_VectorData[dst + k] = pixVal;
        }
      }
    }

    ++comp;
    if(comp > m_InputFileListInfo.EndComp)
    {
      comp = m_InputFileListInfo.StartComp;
      imageNum++;
    }
    if(getCancel() == true)
    {
      notifyStatusMessage(getHumanLabel(), "Conversion was Canceled");
      return;
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ITKImportVectorImageStack::newFilterInstance(bool copyFilterParameters)
{
  ITKImportVectorImageStack::Pointer filter = ITKImportVectorImageStack::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    // We are going to hand copy all of the parameters because the other way of copying the parameters are going to
    // miss some of them because we are not enumerating all of them.
    SIMPL_COPY_INSTANCEVAR(DataContainerName)
    SIMPL_COPY_INSTANCEVAR(CellAttributeMatrixName)
    SIMPL_COPY_INSTANCEVAR(Resolution)
    SIMPL_COPY_INSTANCEVAR(Origin)
    SIMPL_COPY_INSTANCEVAR(InputFileListInfo)
    SIMPL_COPY_INSTANCEVAR(ImageStack)
    SIMPL_COPY_INSTANCEVAR(ImageDataArrayName)
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImportVectorImageStack::getCompiledLibraryName()
{
    return ITKImageProcessingConstants::ITKImageProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImportVectorImageStack::getBrandingString()
{
  return "ImageIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImportVectorImageStack::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << ITKImageProcessing::Version::Major() << "." << ITKImageProcessing::Version::Minor() << "." << ITKImageProcessing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImportVectorImageStack::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImportVectorImageStack::getSubGroupName()
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKImportVectorImageStack::getHumanLabel()
{
  return "Import Images (3D Vector Stack)";
}
