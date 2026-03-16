#include "DemoImageMathAlgorithm.h"

#include <ImFusion/Base/DataList.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Base/Utils/TypeSwitcher.h>
#include <ImFusion/Core/GL/Texture.h>
#include <ImFusion/ImageMath/SharedImageSetArithmetic.h>


namespace ImFusion
{
	DemoImageMathAlgorithm::DemoImageMathAlgorithm(SharedImageSet* img)
		: m_imgIn(img)
	{
	}


	bool DemoImageMathAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		// check requirements to create the algorithm
		if (data.size() != 1)
			return false;
		SharedImageSet* img = data.getImage(Data::UNKNOWN);    // in our case, any image is fine
		if (img == nullptr)
			return false;

		// requirements are met, create the algorithm if asked
		if (a)
		{
			*a = new DemoImageMathAlgorithm(img);
		}
		return true;
	}


	void DemoImageMathAlgorithm::compute()
	{
		// set generic error status until we have finished
		m_status = static_cast<int>(Status::Error);

		const auto& descIn = m_imgIn->get()->descriptor();
		if (descIn.channels != 3)
		{
			LOG_ERROR("Expected an RGB image with 3 channels, but got " << descIn.channels << " channel(s)!");
			return;
		}

		// ImageMath enables arithmetic operations on images supporting GPU and CPU paths
		// E.g. you can add two images with
		// ImageMath::makeArray(*img1) + ImageMath::makeArray(*img2)
		// or average them via
		// 0.5 * (ImageMath::makeArray(*img1) + ImageMath::makeArray(*img2))

		// Another example would be to convert a rgb image to grayscale
		m_imgOut = (ImageMath::makeArray(*m_imgIn) * p_weights.value()).channelSum().evaluateIntoImage<float>();

		// We can also create masks from ImageMath, e.g. here we create a circular mask
		// auto tc = ImageMath::makeCoordinates(*m_imgOut->get(), ImageMath::CoordinateType::Texture);
		// auto circle = (tc - 0.5).head<2>().length() < 0.5;
		// m_imgOut->setMask(circle.toMask());

		// We can also create masks which use the pixel value, e.g. here we create a mask which is true for pixels with a value between 50 and 150
		const auto& desc = m_imgOut->get()->descriptor();
		ImFusion::Utils::typeSwitch(desc.pixelType, [&](auto wrapper) -> void {
			using T = typename decltype(wrapper)::type;

			auto var = ImageMath::StashVariable<T>(desc.channels);

			m_imgOut->setMask(((var > 50 && var < 150)).toMask());
		});

		// set algorithm status to success
		m_status = static_cast<int>(Status::Success);
	}


	OwningDataList DemoImageMathAlgorithm::takeOutput()
	{
		// if we have produced some output, add it to the list
		return OwningDataList(std::move(m_imgOut));
	}
}
