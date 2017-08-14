#ifndef _iTKImageProcessingHelpers_H_
#define _iTKImageProcessingHelpers_H_

#include <QtCore/QString>
#include <QtCore/QVector>

#include <itkImageSeriesReader.h>

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

#include "ITKImageProcessing/ITKImageProcessingFilters/itkDream3DImage.h"
#include "ITKImageProcessing/ITKImageProcessingFilters/itkInPlaceImageToDream3DDataFilter.h"


namespace ITKImageProcessing
{
namespace Helpers
{


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename FileListInfo>
QVector<QString> getVectorFileList(FileListInfo& inputFileListInfo)
{
  bool hasMissingFiles = false;
  bool orderAscending = false;

  if(inputFileListInfo.Ordering == 0)
  {
    orderAscending = true;
  }
  else if(inputFileListInfo.Ordering == 1)
  {
    orderAscending = false;
  }

  // Now generate all the file names the user is asking for and populate the table
  return FilePathGenerator::GenerateVectorFileList(inputFileListInfo.StartIndex, inputFileListInfo.EndIndex,
                                                   inputFileListInfo.StartComp, inputFileListInfo.EndComp,
                                                   hasMissingFiles, orderAscending,
                                                   inputFileListInfo.InputPath,
                                                   inputFileListInfo.FilePrefix, inputFileListInfo.Separator,
                                                   inputFileListInfo.FileSuffix, inputFileListInfo.FileExtension,
                                                   inputFileListInfo.PaddingDigits);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename FileListInfo>
QVector<QString> getFileList(FileListInfo& inputFileListInfo)
{
  bool hasMissingFiles = false;
  bool orderAscending = false;

  if(inputFileListInfo.Ordering == 0)
  {
    orderAscending = true;
  }
  else if(inputFileListInfo.Ordering == 1)
  {
    orderAscending = false;
  }

  // Now generate all the file names the user is asking for and populate the table
  return FilePathGenerator::GenerateFileList(inputFileListInfo.StartIndex, inputFileListInfo.EndIndex,
                                             hasMissingFiles, orderAscending, inputFileListInfo.InputPath,
                                             inputFileListInfo.FilePrefix, inputFileListInfo.FileSuffix,
                                             inputFileListInfo.FileExtension, inputFileListInfo.PaddingDigits);
}



/**
 *
 */
template <typename TPixel, typename FilterType>
void readImageOutputInformation(FilterType* filter, typename itk::ImageSeriesReader<itk::Dream3DImage<TPixel, 3>>::Pointer& reader, DataContainer::Pointer& container)
{
  const unsigned int Dimension = 3;
  typedef itk::Dream3DImage<TPixel, Dimension> ImageType;

  reader->UpdateOutputInformation();
  const typename ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  QVector<size_t> tDims(Dimension, 1);
  for(unsigned int i = 0; i < Dimension; i++)
  {
    tDims[i] = size[i];
  }

  QVector<size_t> cDims(1, 1);
  AttributeMatrix::Pointer cellAttrMat = container->createNonPrereqAttributeMatrix<AbstractFilter>(filter, filter->getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(filter->getErrorCondition() < 0)
  {
    return;
  }
  DataArrayPath path;
  path.update(filter->getDataContainerName(), filter->getCellAttributeMatrixName(), filter->getImageDataArrayName());
  filter->getDataContainerArray()->template createNonPrereqArrayFromPath<DataArray<TPixel>, AbstractFilter, TPixel>(filter, path, 0, cDims);
}

/**
 *
 */
template <typename TPixel, typename FilterType>
void readImageWithPixelType(FilterType* filter, const QVector<QString>& fileList, bool dataCheck)
{
  DataContainer::Pointer container = filter->getDataContainerArray()->getDataContainer(filter->getDataContainerName());
  if(nullptr == container.get())
  {
    filter->setErrorCondition(-4);
    filter->notifyErrorMessage(filter->getHumanLabel(), "Container not found.", filter->getErrorCondition());
    return;
  }

  const unsigned int Dimension = 3;
  typedef itk::Dream3DImage<TPixel, Dimension> ImageType;
  typedef itk::ImageSeriesReader<ImageType> ReaderType;
  typedef itk::InPlaceImageToDream3DDataFilter<TPixel, Dimension> ToDream3DType;

  typename ReaderType::Pointer reader = ReaderType::New();
  typename ReaderType::FileNamesContainer fileNames(fileList.size());
  for(size_t fileIndex = 0; fileIndex < fileList.size(); ++fileIndex)
  {
    fileNames[fileIndex] = fileList[fileIndex].toStdString();
  }
  reader->SetFileNames(fileNames);

  if(dataCheck)
  {
    readImageOutputInformation<TPixel, FilterType>(filter, reader, container);
  }
  else
  {
    typename ToDream3DType::Pointer toDream3DFilter = ToDream3DType::New();
    toDream3DFilter->SetInput(reader->GetOutput());
    toDream3DFilter->SetInPlace(true);
    toDream3DFilter->SetAttributeMatrixArrayName(filter->getCellAttributeMatrixName().toStdString());
    toDream3DFilter->SetDataArrayName(filter->getImageDataArrayName().toStdString());
    toDream3DFilter->SetDataContainer(container);
    toDream3DFilter->Update();
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  FloatVec3_t resolution = filter->getResolution();
  image->setResolution(resolution.x, resolution.y, resolution.z);
  filter->setResolution(resolution);

  FloatVec3_t origin = filter->getOrigin();
  image->setOrigin(origin.x, origin.y, origin.z);
  filter->setOrigin(origin);

  const typename ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  QVector<size_t> tDims(Dimension, 1);
  for(unsigned int i = 0; i < Dimension; i++)
  {
    tDims[i] = size[i];
  }
  image->setDimensions(tDims[0], tDims[1], tDims[2]);
  container->setGeometry(image);
}
}
}

#endif /* _iTKImageProcessingHelpers_H_ */
