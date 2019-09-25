/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/Algorithm.h>
#include <ImFusion/Base/ParameterMixin.h>

namespace ImFusion
{
	class MemImage;
	template <typename T>
	class TypedImage;
	class SharedImage;

	/// Canny edge algorithm showcasing ITK integration in ImFusion SDK
	/// This algorithm will extract the edges of the input image using ITK filters
	class ITKCannyEdgeAlgorithm : public ParameterMixin<Algorithm>
	{
	public:
		ITKCannyEdgeAlgorithm(const SharedImage& image);
		virtual ~ITKCannyEdgeAlgorithm() override;

		static bool createCompatible(const DataList& data, Algorithm** a = 0);

		virtual void compute() override;
		virtual void output(DataList& data) override;
	private:
		std::unique_ptr<TypedImage<double>> m_input;
		std::unique_ptr<TypedImage<double>> m_output;

		Parameter<int> m_variance = Parameter<int>("variance", 0, *this);
		Parameter<double> m_threshold = Parameter<double>("threshold", 60.0f, *this);
		Parameter<int> m_ratio = Parameter<int>("ratio", 3, *this);

	};
}
