/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/ImFusionPlugin.h>

namespace ImFusion
{
	class AlgorithmFactory;
	class AlgorithmControllerFactory;

	// See also the ExamplePlugin for further documentation
	class Example2D3DRegistrationPlugin : public ImFusionPlugin
	{
	public:
		Example2D3DRegistrationPlugin();
		virtual ~Example2D3DRegistrationPlugin();
		virtual const AlgorithmFactory* getAlgorithmFactory();
		virtual const AlgorithmControllerFactory* getAlgorithmControllerFactory();
	};
}
