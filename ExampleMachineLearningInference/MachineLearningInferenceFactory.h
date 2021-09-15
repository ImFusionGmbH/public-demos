/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/AlgorithmControllerFactory.h>
#include <ImFusion/Base/AlgorithmFactory.h>

namespace ImFusion
{
	class Algorithm;

	/// AlgorithmFactory for MachineLearningInference plugin
	class MachineLearningInferenceAlgorithmFactory : public AlgorithmFactory
	{
	public:
		MachineLearningInferenceAlgorithmFactory();
	};

	/// AlgorithmControllerFactory for MachineLearningInference plugin
	class MachineLearningInferenceControllerFactory : public AlgorithmControllerFactory
	{
	public:
		virtual AlgorithmController* create(Algorithm* a) const;
	};
}
