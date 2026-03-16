#pragma once

#include <ImFusion/Base/ImFusionLibPlugin.h>

namespace ImFusion
{
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class MachineLearningInferencePlugin : public ImFusionLibPlugin
	{
	public:
		MachineLearningInferencePlugin();
		virtual ~MachineLearningInferencePlugin();

		static const char* id() { return "MachineLearningInference"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		std::vector<std::string> dependencies() const override { return {"ImFusion.Torch", "ImFusion.OnnxRuntime"}; }

		PluginBase::Status init() override;
	};
}
