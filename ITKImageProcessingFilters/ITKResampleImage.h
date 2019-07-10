// File automatically generated

/*
 * Your License or Copyright can go here
 */

#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

#include "ITKImageProcessingBase.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

#include <SIMPLib/FilterParameters/FloatVec3FilterParameter.h>
#include <SIMPLib/FilterParameters/IntFilterParameter.h>
#include <itkResampleImageFilter.h>
#include <itkIdentityTransform.h>

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKResampleImage class. See [Filter documentation](@ref ITKResampleImage) for details.
 */
class ITKImageProcessing_EXPORT ITKResampleImage : public ITKImageProcessingBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKResampleImage SUPERCLASS ITKImageProcessingBase)
  PYB11_PROPERTY(int32_t OutputSpacing READ getOutputSpacing WRITE setOutputSpacing)
  PYB11_PROPERTY(IntVec3Type OutputSize READ getOutputSize WRITE setOutputSize)

public:
  SIMPL_SHARED_POINTERS(ITKResampleImage)
  SIMPL_FILTER_NEW_MACRO(ITKResampleImage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKResampleImage, AbstractFilter)

  ~ITKResampleImage() override;

  SIMPL_FILTER_PARAMETER(int32_t, OutputSpacing)
  Q_PROPERTY(int32_t OutputSpacing READ getOutputSpacing WRITE setOutputSpacing)

  SIMPL_FILTER_PARAMETER(IntVec3Type, OutputSize)
  Q_PROPERTY(IntVec3Type OutputSize READ getOutputSize WRITE setOutputSize)

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

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
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

protected:
  ITKResampleImage();

  /**
   * @brief dataCheckInternal overloads dataCheckInternal in ITKImageBase and calls templated dataCheck
   */
  void dataCheckInternal() override;

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  template <typename InputImageType, typename OutputImageType, unsigned int Dimension> void dataCheck();

  /**
  * @brief filterInternal overloads filterInternal in ITKImageBase and calls templated filter
  */
  void filterInternal() override;

  /**
  * @brief Applies the filter
  */
  template <typename InputImageType, typename OutputImageType, unsigned int Dimension> void filter();

public:
  ITKResampleImage(const ITKResampleImage&) = delete;    // Copy Constructor Not Implemented
  ITKResampleImage(ITKResampleImage&&) = delete;         // Move Constructor Not Implemented
  ITKResampleImage& operator=(const ITKResampleImage&) = delete; // Copy Assignment Not Implemented
  ITKResampleImage& operator=(ITKResampleImage&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

