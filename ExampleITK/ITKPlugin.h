#pragma once

#include <ImFusion/Base/ImFusionLibPlugin.h>

namespace ImFusion
{
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class ITKPlugin : public ImFusionLibPlugin
	{
	public:
		ITKPlugin();
		virtual ~ITKPlugin();

		static const char* id() { return "Demo.ITK"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		PluginBase::Status init() override;
	};
}
