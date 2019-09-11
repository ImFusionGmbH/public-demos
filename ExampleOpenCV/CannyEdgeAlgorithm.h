#pragma once

#include <ImFusion/Base/Algorithm.h>
#include <ImFusion/Base/Mat.h>

namespace ImFusion
{
	class SharedImage;

	/** \brief	Algorithm to detect edges using Canny
	 */
	class CannyEdgeAlgorithm : public Algorithm
	{
	public:
		CannyEdgeAlgorithm(SharedImage* image);

		virtual ~CannyEdgeAlgorithm() override;

		void setThreshold(double threshold);
		void setRatio(int ratio);

		static bool createCompatible(const DataList& data, Algorithm** a = 0);

		virtual void compute() override;

		void configure(const Properties* p) override;
		void configuration(Properties* p) const override;

		virtual void output(DataList& data) override;

	protected:
		SharedImage* m_inputImage;
		std::unique_ptr<SharedImageSet> m_outputImage;

		double m_threshold = 100.0;
		int m_ratio = 3;
	};
}
