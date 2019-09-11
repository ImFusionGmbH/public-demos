/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/AlgorithmControllerFactory.h>
#include <ImFusion/Base/AlgorithmFactory.h>

namespace ImFusion
{
	class Algorithm;

	/// AlgorithmFactory for OpenCV plugin
	class OpenCVFactory : public AlgorithmFactory
	{
	public:
		OpenCVFactory();
	};

	/// AlgorithmControllerFactory for OpenCV plugin
	class OpenCVControllerFactory : public AlgorithmControllerFactory
	{
	public:
		virtual AlgorithmController* create(Algorithm* a) const;
	};
}
