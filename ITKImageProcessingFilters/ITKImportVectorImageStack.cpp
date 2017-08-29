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

//#include <QtCore/QDir>
//#include <QtCore/QString>
//#include <QtGui/QImage>
//#include <QtGui/QImageReader>


#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

#include "ITKImageProcessingPlugin.h"
#include "ITKImageProcessing/ITKImageProcessingConstants.h"
#include "ITKImageProcessing/ITKImageProcessingFilters/itkGetComponentsDimensions.h"
#include "ITKImageProcessing/ITKImageProcessingFilters/itkInPlaceImageToDream3DDataFilter.h"
#include "ITKImageProcessing/ITKImageProcessingVersion.h"
#include "ITKImageProcessing/FilterParameters/ImportVectorImageStackFilterParameter.h"
#include "ITKImageProcessing/ITKImageProcessingFilters/itkDream3DImage.h"
#include "ITKImageProcessing/ITKImageProcessingFilters/ITKImageProcessingHelpers.hpp"

#include <itkImageFileReader.h>

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
  setErrorCondition(0);
  setWarningCondition(0);
  

  QString ss;
  
  if(m_InputFileListInfo.InputPath.isEmpty() == true)
  {
    ss = QObject::tr("The input directory must be set");
    setErrorCondition(-13);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0 || nullptr == m.get())
  {
    return;
  }
  
  
  // Now generate all the file names the user is asking for and populate the table
  const QVector<QString> fileList = ITKImageProcessing::Helpers::getVectorFileList(m_InputFileListInfo);
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
    
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  
  const bool dataCheck = true;
  readImage(fileList, dataCheck);
  
  // set up component dimensions for the vector image array
  QVector<size_t> arraydims(2);
  arraydims[0] = static_cast<size_t>((m_InputFileListInfo.EndComp - m_InputFileListInfo.StartComp) + 1);
  arraydims[1] = static_cast<size_t>(m_PixelBytes);
  
  // The "readImage()" method chain will end up adding an AttributeArray but it will not
  // be sized correctly for what we need. So we are going to remove it, and then
  // add back in the proper one.
  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer am = dc->getAttributeMatrix(getCellAttributeMatrixName());
  IDataArray::Pointer daPtr = am->removeAttributeArray(getImageDataArrayName());
  
  // This would be for a gray scale image
  DataArrayPath tempPath(getDataContainerName(), getCellAttributeMatrixName(), getImageDataArrayName());
  m_VectorDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, arraydims);
  if(nullptr != m_VectorDataPtr.lock().get())
  {
    m_VectorData = m_VectorDataPtr.lock()->getPointer(0);
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKImportVectorImageStack::readImage(const QVector<QString>& fileList, bool dataCheck)
{
  try
  {
    for(int fileIndex = 0; fileIndex < fileList.size(); ++fileIndex)
    {
      const std::string fileName = fileList[fileIndex].toStdString();
      if(!itksys::SystemTools::FileExists(fileName))
      {
        setErrorCondition(-7);
        QString errorMessage = "File does not exist: %1";
        notifyErrorMessage(getHumanLabel(), errorMessage.arg(fileName.c_str()), getErrorCondition());
        return;
      }
    }
    const QString filename = fileList[0];
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(filename.toLatin1(), itk::ImageIOFactory::ReadMode);
    if(nullptr == imageIO)
    {
      setErrorCondition(-5);
      QString errorMessage = "ITK could not read the given file \"%1\". Format is likely unsupported.";
      notifyErrorMessage(getHumanLabel(), errorMessage.arg(filename), getErrorCondition());
      return;
    }
    imageIO->SetFileName(filename.toLatin1());
    imageIO->ReadImageInformation();

    typedef itk::ImageIOBase::IOComponentType ComponentType;
    const ComponentType type = imageIO->GetComponentType();
    const unsigned int dimensions = imageIO->GetNumberOfDimensions();
    if(dimensions != 2)
    {
      setErrorCondition(-1);
      const QString errorMessage = "Slice image dimensions do not equal 2";
      notifyErrorMessage(getHumanLabel(), errorMessage, getErrorCondition());
      return;
    }

    switch(type)
    {
    case itk::ImageIOBase::UCHAR:
      ITKImageProcessing::Helpers::readImageWithPixelType<unsigned char, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 1;
      break;
    case itk::ImageIOBase::CHAR:
      ITKImageProcessing::Helpers::readImageWithPixelType<char, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 1;
      break;
    case itk::ImageIOBase::USHORT:
      ITKImageProcessing::Helpers::readImageWithPixelType<unsigned short, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 2;
      break;
    case itk::ImageIOBase::SHORT:
      ITKImageProcessing::Helpers::readImageWithPixelType<short, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 2;
      break;
    case itk::ImageIOBase::UINT:
      ITKImageProcessing::Helpers::readImageWithPixelType<unsigned int, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 4;
      break;
    case itk::ImageIOBase::INT:
      ITKImageProcessing::Helpers::readImageWithPixelType<int, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 4;
      break;
    case itk::ImageIOBase::ULONG:
      ITKImageProcessing::Helpers::readImageWithPixelType<unsigned long, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 8;
      break;
    case itk::ImageIOBase::LONG:
      ITKImageProcessing::Helpers::readImageWithPixelType<long, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 8;
      break;
    case itk::ImageIOBase::FLOAT:
      ITKImageProcessing::Helpers::readImageWithPixelType<float, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 4;
      break;
    case itk::ImageIOBase::DOUBLE:
      ITKImageProcessing::Helpers::readImageWithPixelType<double, ITKImportVectorImageStack>(this, fileList, dataCheck);
      m_PixelBytes = 8;
      break;
    default:
      setErrorCondition(-4);
      QString errorMessage = QString("Unsupported pixel type: %1.").arg(imageIO->GetComponentTypeAsString(type).c_str());
      notifyErrorMessage(getHumanLabel(), errorMessage, getErrorCondition());
      break;
    }
  } catch(itk::ExceptionObject& err)
  {
    setErrorCondition(-55559);
    QString errorMessage = "ITK exception was thrown while processing input file: %1";
    notifyErrorMessage(getHumanLabel(), errorMessage.arg(err.what()), getErrorCondition());
    return;
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
  return "ITKImageProcessing";
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
  return "ITK::Import Images (3D Vector Stack)";
}
