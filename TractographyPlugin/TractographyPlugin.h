#pragma once

#include <ImFusion/Base/ImFusionPlugin.h>

namespace ImFusion
{
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class TractographyPlugin : public ImFusionLibPlugin
	{
	public:
		TractographyPlugin();
		virtual ~TractographyPlugin();

		static const char* id() { return "Demo.TractographyPlugin"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		PluginBase::Status init() override;
	};
}
