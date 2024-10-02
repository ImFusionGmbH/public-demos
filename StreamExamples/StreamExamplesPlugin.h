/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/ImFusionPlugin.h>
#include <ImFusion/Core/Resource/Repository.h>

namespace ImFusion
{
	class AlgorithmFactory;
	class AlgorithmControllerFactory;

	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class StreamExamplesPlugin : public ImFusionPlugin
	{
	public:
		StreamExamplesPlugin();
		~StreamExamplesPlugin() override;
		const AlgorithmFactory* getAlgorithmFactory() override;
		const AlgorithmControllerFactory* getAlgorithmControllerFactory() override;
	};
}
