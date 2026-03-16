#include "MachineLearningInferencePlugin.h"

#include "MachineLearningInferenceFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::MachineLearningInferencePlugin)


namespace ImFusion
{
	MachineLearningInferencePlugin::MachineLearningInferencePlugin() = default;


	MachineLearningInferencePlugin::~MachineLearningInferencePlugin() = default;

	PluginBase::Status MachineLearningInferencePlugin::init()
	{
		registerFactories(
			std::make_unique<MachineLearningInferenceAlgorithmFactory>(), std::make_unique<MachineLearningInferenceControllerFactory>(), nullptr);
		return Status::Success;
	}
}
