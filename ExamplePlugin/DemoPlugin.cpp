#include "DemoPlugin.h"

#include "DemoFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::DemoPlugin)

namespace ImFusion
{
	DemoPlugin::DemoPlugin() = default;


	DemoPlugin::~DemoPlugin() = default;

	PluginBase::Status DemoPlugin::init()
	{
		registerFactories(std::make_unique<DemoAlgorithmFactory>(), std::make_unique<DemoControllerFactory>(), nullptr);

		return Status::Success;
	}
}
