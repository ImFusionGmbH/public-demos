/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/AlgorithmControllerFactory.h>
#include <ImFusion/Base/AlgorithmFactory.h>

namespace ImFusion
{
	class Algorithm;

	/// AlgorithmFactory for demo plugin
	class StreamExamplesAlgorithmFactory : public AlgorithmFactory
	{
	public:
		StreamExamplesAlgorithmFactory();
	};

	/// AlgorithmControllerFactory for demo plugin
	class StreamExamplesControllerFactory : public AlgorithmControllerFactory
	{
	public:
		StreamExamplesControllerFactory();

		AlgorithmController* create(Algorithm* a) const override;
	};
}
