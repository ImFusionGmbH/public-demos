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
		, m_outputImage(std::make_unique<SharedImageSet>())
	{
		configureDefaults();
	}

	CannyEdgeAlgorithm::~CannyEdgeAlgorithm() {}


	bool CannyEdgeAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		if (data.size() != 1)
			return false;

		SharedImageSet* is = data.getImage(Data::IMAGE);
		if (is)
		{
			if (!is->images().empty())
			{
				if (is->images().front())
				{
					if (a)
						* a = new CannyEdgeAlgorithm(is->images().front());
					return true;
				}
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


	void CannyEdgeAlgorithm::configure(const Properties* p)
	{
		if (!p)
			return;

		p->param("threshold", m_threshold);
		p->param("ratio", m_ratio);
	}


	void CannyEdgeAlgorithm::configuration(Properties* p) const
	{
		if (!p)
			return;

		p->setParam("threshold", m_threshold);
		p->setParam("ratio", m_ratio);
	}


	void CannyEdgeAlgorithm::compute()
	{
		cv::Mat input, output;

		// conver to OpenCV
		input = OpenCV::convert(m_inputImage->mem());

		cv::Canny(input, output, m_threshold, m_threshold * m_ratio, 3);

		// convert to ImFusion

		std::unique_ptr<TypedImage<double>> image = OpenCV::convert(output);

		m_outputImage->add(image.release());

		m_status = static_cast<int>(Algorithm::Status::Success);
	}
}
