/*
 * Your License or Copyright can go here
 */

#pragma once

#include "ITKImageBase.h"

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKImageProcessingBase class. See [Filter documentation](@ref ITKImageProcessingBase) for details.
 */
class ITKImageProcessing_EXPORT ITKImageProcessingBase : public ITKImageBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKImageProcessingBase SUPERCLASS ITKImageBase)
  PYB11_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)
  PYB11_PROPERTY(QString NewCellArrayName READ getNewCellArrayName WRITE setNewCellArrayName)
  PYB11_PROPERTY(bool SaveAsNewArray READ getSaveAsNewArray WRITE setSaveAsNewArray)

public:
  SIMPL_SHARED_POINTERS(ITKImageProcessingBase)
  SIMPL_FILTER_NEW_MACRO(ITKImageProcessingBase)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKImageProcessingBase, ITKImageBase)

  ~ITKImageProcessingBase() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, SelectedCellArrayPath)
  Q_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)

  SIMPL_FILTER_PARAMETER(QString, NewCellArrayName)
  Q_PROPERTY(QString NewCellArrayName READ getNewCellArrayName WRITE setNewCellArrayName)

  SIMPL_FILTER_PARAMETER(bool, SaveAsNewArray)
  Q_PROPERTY(bool SaveAsNewArray READ getSaveAsNewArray WRITE setSaveAsNewArray)

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
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  virtual const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

protected:
  ITKImageProcessingBase();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void dataCheck()
  {
    // typedef typename itk::NumericTraits<InputPixelType>::ValueType InputValueType;
    typedef typename itk::NumericTraits<OutputPixelType>::ValueType OutputValueType;
    // Check data array
    imageCheck<InputPixelType, Dimension>(getSelectedCellArrayPath());

    if(getErrorCode() < 0)
    {
      return;
    }
    std::vector<size_t> outputDims = ITKDream3DHelper::GetComponentsDimensions<OutputPixelType>();
    if(m_SaveAsNewArray)
    {
      DataArrayPath tempPath;
      tempPath.update(getSelectedCellArrayPath().getDataContainerName(), getSelectedCellArrayPath().getAttributeMatrixName(), getNewCellArrayName());
      m_NewCellArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<OutputValueType>, AbstractFilter, OutputValueType>(
          this, tempPath, 0, outputDims);           /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if(nullptr != m_NewCellArrayPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
      {
        m_NewCellArray = m_NewCellArrayPtr.lock()->getVoidPointer(0);
      } /* Now assign the raw pointer to data from the DataArray<T> object */
    }
    else
    {
      m_NewCellArrayPtr = DataArray<OutputValueType>::NullPointer();
      m_NewCellArray = nullptr;
    }
  }

  /**
  * @brief Applies the filter
  */
  template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension, typename FilterType> void filter(FilterType* filter)
  {
    std::string outputArrayName = getSelectedCellArrayPath().getDataArrayName().toStdString();

    if(getSaveAsNewArray())
    {
      outputArrayName = getNewCellArrayName().toStdString();
    }
    ITKImageBase::filter<InputPixelType, OutputPixelType, Dimension, FilterType>(filter, outputArrayName, getSaveAsNewArray(), getSelectedCellArrayPath());
  }

  /**
  * @brief Applies the filter, casting the input to float
  */
  template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension, typename FilterType, typename FloatImageType> void filterCastToFloat(FilterType* filter)
  {
    std::string outputArrayName = getSelectedCellArrayPath().getDataArrayName().toStdString();

    if(getSaveAsNewArray())
    {
      outputArrayName = getNewCellArrayName().toStdString();
    }
    ITKImageBase::filterCastToFloat<InputPixelType, OutputPixelType, Dimension, FilterType, FloatImageType>(filter, outputArrayName, getSaveAsNewArray(), getSelectedCellArrayPath());
  }

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DEFINE_IDATAARRAY_VARIABLE(NewCellArray)

public:
  ITKImageProcessingBase(const ITKImageProcessingBase&) = delete;            // Copy Constructor Not Implemented
  ITKImageProcessingBase(ITKImageProcessingBase&&) = delete;                 // Move Constructor Not Implemented
  ITKImageProcessingBase& operator=(const ITKImageProcessingBase&) = delete; // Copy Assignment Not Implemented
  ITKImageProcessingBase& operator=(ITKImageProcessingBase&&) = delete;      // Move Assignment Not Implemented
};

