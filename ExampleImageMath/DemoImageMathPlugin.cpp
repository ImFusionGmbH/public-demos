#include "DemoImageMathPlugin.h"

#include "DemoImageMathFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::DemoImageMathPlugin)

namespace ImFusion
{
	DemoImageMathPlugin::DemoImageMathPlugin() = default;


	DemoImageMathPlugin::~DemoImageMathPlugin() = default;

	PluginBase::Status DemoImageMathPlugin::init()
	{
		registerFactories(std::make_unique<DemoImageMathAlgorithmFactory>(), std::make_unique<DemoImageMathControllerFactory>(), nullptr);
		return Status::Success;
	}
}
