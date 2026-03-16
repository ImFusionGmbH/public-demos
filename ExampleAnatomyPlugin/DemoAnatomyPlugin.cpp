#include "DemoAnatomyPlugin.h"

#include "DemoAnatomyFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::DemoAnatomyPlugin)

namespace ImFusion
{
	DemoAnatomyPlugin::DemoAnatomyPlugin() = default;

	DemoAnatomyPlugin::~DemoAnatomyPlugin() = default;

	PluginBase::Status DemoAnatomyPlugin::init()
	{
		registerFactories(std::make_unique<DemoAnatomyAlgorithmFactory>(), std::make_unique<DemoAnatomyControllerFactory>(), nullptr);
		return Status::Success;
	}
}
