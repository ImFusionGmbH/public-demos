#include <ImFusion/Base/ImageProcessing.h>
#include <ImFusion/Base/TypedImage.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Core/Log.h>
#include "ITKCannyEdgeAlgorithm.h"
#include "ITK.h"

#include "itkImage.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkRGBToLuminanceImageFilter.h"


namespace ImFusion
{
	ITKCannyEdgeAlgorithm::ITKCannyEdgeAlgorithm(const SharedImage& image)
	{
		m_input.reset(ImageProcessing::createFloat(*image.mem(), ImageProcessing::Normalization::ValueRange)->convert<double>(0.0f, 1.0f));

		configureDefaults();
		m_variance.setRange(0, 2);
		m_ratio.setRange(1, 10);
		m_threshold.setRange(0, 255);
		m_threshold.setAttribute("step", "0.5");
	}

	ITKCannyEdgeAlgorithm::~ITKCannyEdgeAlgorithm() {}


	bool ITKCannyEdgeAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		if (data.size() != 1)
			return false;

		SharedImageSet* is = data.getImage(Data::IMAGE);
		if (is)
		{
			if (is->images().size() == 1)
			{
				SharedImage* image = is->get();
				if (image)
				{
					if (a)
					{
						*a = new ITKCannyEdgeAlgorithm(*image);
					}
					return true;
				}
			}
		}

		return false;
	}

	void ITKCannyEdgeAlgorithm::compute()
	{
		using RGBToGrayscaleType = itk::RGBToLuminanceImageFilter< ITK::RGBImageType, ITK::RealImageType >;
		using RescaleFilterType = itk::RescaleIntensityImageFilter< ITK::RealImageType, ITK::RealImageType >;
		using CannyFilterType = itk::CannyEdgeDetectionImageFilter< ITK::RealImageType, ITK::RealImageType >;
		using GrayscaleToRGBType = itk::CastImageFilter<ITK::RealImageType, ITK::RGBImageType>;


		RGBToGrayscaleType::Pointer rgbToGray = RGBToGrayscaleType::New();
		RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
		CannyFilterType::Pointer cannyFilter = CannyFilterType::New();
		GrayscaleToRGBType::Pointer grayToRGB = GrayscaleToRGBType::New();

		ITK::RGBImageType::Pointer image;

		// initialize status as error
		m_status = static_cast<int>(Algorithm::Status::Error);

		// check channels, the converted is limited to RGB images
		if (m_input->channels() != 3)
		{
			LOG_ERROR("Can only apply filter on RGB images");
			return;
		}

		// create image in ITK
		image = ITK::convert(*m_input);
		if (!image)
			return;

		// sequence RGB2Gray -> Rescale values -> Canny filter -> Gray2RGB
		rgbToGray->SetInput(image);
		rescaleFilter->SetInput(rgbToGray->GetOutput());
		cannyFilter->SetInput(rescaleFilter->GetOutput());
		grayToRGB->SetInput(cannyFilter->GetOutput());
		image = grayToRGB->GetOutput();

		// rescale configuration
		{
			rescaleFilter->SetOutputMinimum(0);
			rescaleFilter->SetOutputMaximum(255);
		}

		// canny configuration
		{
			float lowerThreshold = m_threshold / m_ratio;

			cannyFilter->SetVariance(m_variance);
			cannyFilter->SetUpperThreshold(m_threshold);
			cannyFilter->SetLowerThreshold(lowerThreshold);
		}

		// back to imfusion
		m_output = ITK::convert(image); // ITK filter execution done before conversion

		// mark computation as success
		m_status = static_cast<int>(Algorithm::Status::Success);
	}

	OwningDataList ITKCannyEdgeAlgorithm::takeOutput()
	{
		return OwningDataList(std::make_unique<SharedImageSet>(std::move(m_output)));
	}
}
