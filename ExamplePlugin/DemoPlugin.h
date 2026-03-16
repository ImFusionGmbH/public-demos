#pragma once

#include <ImFusion/Base/ImFusionLibPlugin.h>

namespace ImFusion
{
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class DemoPlugin : public ImFusionLibPlugin
	{
	public:
		DemoPlugin();
		~DemoPlugin() override;

		static const char* id() { return "Demo.Plugin"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		PluginBase::Status init() override;
	};
}
