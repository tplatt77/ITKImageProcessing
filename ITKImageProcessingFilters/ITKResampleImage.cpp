// File automatically generated

/*
 * Your License or Copyright can go here
 */

#include "ITKImageProcessing/ITKImageProcessingFilters/ITKResampleImage.h"
#include "SIMPLib/ITK/SimpleITKEnums.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/Geometry/ImageGeom.h"

#include "SIMPLib/ITK/Dream3DTemplateAliasMacro.h"
#include "SIMPLib/ITK/itkDream3DImage.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKResampleImage::ITKResampleImage()
{
  m_OutputSpacing = StaticCastScalar<double, double, double>(32u);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKResampleImage::~ITKResampleImage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKResampleImage::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Output Spacing", OutputSpacing, FilterParameter::Parameter, ITKResampleImage));
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Output Size", OutputSize, FilterParameter::Parameter, ITKResampleImage));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save as New Array", SaveAsNewArray, FilterParameter::Parameter, ITKResampleImage, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to filter", SelectedCellArrayPath, FilterParameter::RequiredArray, ITKResampleImage, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Filtered Array", NewCellArrayName, FilterParameter::CreatedArray, ITKResampleImage));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKResampleImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath(reader->readDataArrayPath("SelectedCellArrayPath", getSelectedCellArrayPath()));
  setNewCellArrayName(reader->readString("NewCellArrayName", getNewCellArrayName()));
  setSaveAsNewArray(reader->readValue("SaveAsNewArray", getSaveAsNewArray()));
  setOutputSpacing(reader->readValue("OutputSpacing", getOutputSpacing()));
  setOutputSize(reader->readIntVec3("OutputSize", getOutputSize()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKResampleImage::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  // Check consistency of parameters
  this->CheckIntegerEntry<int, int>(m_OutputSpacing, "OutputSpacing", true);
  this->CheckVectorEntry<int, IntVec3Type>(m_OutputSize, "OutputSize", true);

  ITKImageProcessingBase::dataCheck<InputPixelType, OutputPixelType, Dimension>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKResampleImage::dataCheckInternal()
{
  Dream3DArraySwitchMacro(this->dataCheck, getSelectedCellArrayPath(), -4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKResampleImage::filter()
{
  typedef itk::Dream3DImage<InputPixelType, Dimension> InputImageType;
  typedef itk::Dream3DImage<OutputPixelType, Dimension> OutputImageType;
  // define filter
  typedef itk::ResampleImageFilter<InputImageType, OutputImageType> FilterType;
  using TransformPrecisionType = double;
  using TransformType = itk::IdentityTransform<TransformPrecisionType, Dimension>;
  using ImageType = itk::Image<InputPixelType, Dimension>;
  typename FilterType::Pointer filter = FilterType::New();
  ImageType::SpacingType outputSpacing;
  for(unsigned int dim = 0; dim < Dimension; dim++)
  {
    outputSpacing[dim] = static_cast<unsigned int>(m_OutputSpacing);
  }
  ImageType::SizeType outputSize;
  outputSize[0] = m_OutputSize.getX() / m_OutputSpacing;
  outputSize[1] = m_OutputSize.getY() / m_OutputSpacing;
  if(Dimension > 2)
  {
    outputSize[2] = m_OutputSize.getZ();
  }
  filter->SetSize(outputSize);
  filter->SetTransform(TransformType::New());
  filter->SetOutputSpacing(outputSpacing);
  this->ITKImageProcessingBase::filter<InputPixelType, OutputPixelType, Dimension, FilterType>(filter);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKResampleImage::filterInternal()
{
  Dream3DArraySwitchMacro(this->filter, getSelectedCellArrayPath(), -4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ITKResampleImage::newFilterInstance(bool copyFilterParameters) const
{
  ITKResampleImage::Pointer filter = ITKResampleImage::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKResampleImage::getHumanLabel() const
{
  return "ITK::Rescale Image Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ITKResampleImage::getUuid()
{
  return QUuid("{45cec151-2950-51f8-8a32-47d6716d8721}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKResampleImage::getSubGroupName() const
{
  return "ITK BiasCorrection";
}
