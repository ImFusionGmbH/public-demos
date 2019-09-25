#pragma once

#include <ImFusion/Base/Algorithm.h>
#include <ImFusion/Base/Mat.h>
#include <ImFusion/Base/ParameterMixin.h>

namespace ImFusion
{
	class SharedImage;

	/** \brief	Algorithm to detect edges using Canny
	 */
	class CannyEdgeAlgorithm : public ParameterMixin<Algorithm>
	{
	public:
		CannyEdgeAlgorithm(SharedImage* image);

		virtual ~CannyEdgeAlgorithm() override;

		void setThreshold(double threshold);
		void setRatio(int ratio);

		static bool createCompatible(const DataList& data, Algorithm** a = 0);

		virtual void compute() override;

		virtual void output(DataList& data) override;

	protected:
		SharedImage* m_inputImage;
		std::unique_ptr<SharedImageSet> m_outputImage;

		Parameter<double> m_threshold = Parameter<double>("threshold", 100.0, *this);
		Parameter<int> m_ratio = Parameter<int>("ratio", 3, *this);
	};
}
