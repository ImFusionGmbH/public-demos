#pragma once

#include <ImFusion/Base/ImFusionLibPlugin.h>

namespace ImFusion
{
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class OpenCVPlugin : public ImFusionLibPlugin
	{
	public:
		OpenCVPlugin();
		virtual ~OpenCVPlugin();

		static const char* id() { return "Demo.OpenCV"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		PluginBase::Status init() override;
	};
}
