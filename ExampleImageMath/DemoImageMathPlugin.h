/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */

#include <ImFusion/Base/ImFusionLibPlugin.h>
#include <ImFusion/Core/Resource/Repository.h>

namespace ImFusion
{
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class DemoImageMathPlugin : public ImFusionLibPlugin
	{
	public:
		DemoImageMathPlugin();
		~DemoImageMathPlugin() override;

		static const char* id() { return "Demo.ImageMath"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		std::vector<std::string> dependencies() const override { return {"ImFusion.ImageMath"}; }

		PluginBase::Status init() override;
	};
}
