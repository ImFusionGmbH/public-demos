/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/AlgorithmControllerFactory.h>
#include <ImFusion/Base/AlgorithmFactory.h>

namespace ImFusion
{
	class Algorithm;

	// See also the ExamplePlugin for further documentation

	class Example2D3DRegistrationAlgorithmFactory : public AlgorithmFactory
	{
	public:
		Example2D3DRegistrationAlgorithmFactory();
	};

	class Example2D3DRegistrationControllerFactory : public AlgorithmControllerFactory
	{
	public:
		virtual AlgorithmController* create(Algorithm* a) const;
	};
}
