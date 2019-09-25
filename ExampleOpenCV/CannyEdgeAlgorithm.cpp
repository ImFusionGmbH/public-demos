#include <CannyEdgeAlgorithm.h>
#include <ImFusion/Base/Log.h>
#include <ImFusion/Base/OpenCV.h>
#include <ImFusion/Base/SharedImage.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Base/TypedImage.h>

#include <opencv2/opencv.hpp>

namespace ImFusion
{
	CannyEdgeAlgorithm::CannyEdgeAlgorithm(SharedImage* image)
		: m_inputImage(image)
	{
		configureDefaults();
	}

	CannyEdgeAlgorithm::~CannyEdgeAlgorithm() {}


	bool CannyEdgeAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		// check that there is only one data selected
		if (data.size() != 1)
			return false;

		// check that it is a shared image set, and contains exactly one image
		SharedImageSet* is = data.getImage(Data::IMAGE);
		if (is && is->images().size() == 1)
		{
			// retrieves the first image, and creates the algorithm
			SharedImage* image = is->get();
			if (image)
			{
				if (a)
				{
					*a = new CannyEdgeAlgorithm(image);
				}
				return true;
			}
		}

		return false;
	}

	void CannyEdgeAlgorithm::setThreshold(double threshold) { m_threshold = threshold; }


	void CannyEdgeAlgorithm::setRatio(int ratio) { m_ratio = ratio; }


	void CannyEdgeAlgorithm::output(DataList& data)
	{
		data.add(m_outputImage.release());    // give ownership to data
	}


	void CannyEdgeAlgorithm::compute()
	{
		cv::Mat input, output;

		// prepare output Shared Image Set
		m_outputImage = std::make_unique<SharedImageSet>();

		// prepare status as error in case the computation fails
		m_status = static_cast<int>(Algorithm::Status::Error);

		// convert to OpenCV
		input = OpenCV::convert(m_inputImage->mem());

		// run opencv canny
		cv::Canny(input, output, m_threshold, m_threshold * m_ratio, 3);

		// convert to ImFusion
		std::unique_ptr<TypedImage<double>> image = OpenCV::convert(output);

		// add the transformed image to the output shared image set
		m_outputImage->add(image.release());

		// change status to success
		m_status = static_cast<int>(Algorithm::Status::Success);
	}
}
