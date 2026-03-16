#include "ITKPlugin.h"

#include "ITKFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::ITKPlugin)


namespace ImFusion
{
	ITKPlugin::ITKPlugin() = default;

	ITKPlugin::~ITKPlugin() = default;

	PluginBase::Status ITKPlugin::init()
	{
		registerFactories(std::make_unique<ITKAlgorithmFactory>(), std::make_unique<ITKControllerFactory>(), nullptr);
		return Status::Success;
	};
}
