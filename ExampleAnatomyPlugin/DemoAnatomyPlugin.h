/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/ImFusionLibPlugin.h>
#include <ImFusion/Core/Resource/Repository.h>

namespace ImFusion
{
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class DemoAnatomyPlugin : public ImFusionLibPlugin
	{
	public:
		DemoAnatomyPlugin();
		~DemoAnatomyPlugin() override;

		static const char* id() { return "Demo.ExampleAnatomy"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		std::vector<std::string> dependencies() const override { return {"ImFusion.Anatomy"}; }

		Status init() override;
	};
}
