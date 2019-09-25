/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/AlgorithmControllerFactory.h>
#include <ImFusion/Base/AlgorithmFactory.h>

namespace ImFusion
{
	class Algorithm;

	/// AlgorithmFactory for ITK integration
	class ITKAlgorithmFactory : public AlgorithmFactory
	{
	public:
		ITKAlgorithmFactory();
	};

	/// AlgorithmControllerFactory for ITK integration
	class ITKControllerFactory : public AlgorithmControllerFactory
	{
	public:
		virtual AlgorithmController* create(Algorithm* a) const;
	};
}
