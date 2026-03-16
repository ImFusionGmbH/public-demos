/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/ImFusionPlugin.h>
#include <ImFusion/Core/Resource/Repository.h>

namespace ImFusion
{
	class AlgorithmFactory;
	class AlgorithmControllerFactory;

	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class StreamExamplesPlugin : public ImFusionLibPlugin
	{
	public:
		StreamExamplesPlugin();
		~StreamExamplesPlugin() override;

		static const char* id() { return "StreamExamples"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		std::vector<std::string> dependencies() const override { return {"ImFusion.Stream"}; }

		Status init() override;
	};
}
