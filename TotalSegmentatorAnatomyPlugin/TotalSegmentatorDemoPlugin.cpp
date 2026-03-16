#include "TotalSegmentatorDemoPlugin.h"

#include "TotalSegmentatorDemoFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::TotalSegmentatorDemoPlugin)

namespace ImFusion
{
	TotalSegmentatorDemoPlugin::TotalSegmentatorDemoPlugin() = default;

	TotalSegmentatorDemoPlugin::~TotalSegmentatorDemoPlugin() = default;

	PluginBase::Status TotalSegmentatorDemoPlugin::init()
	{
		registerFactories(
			std::make_unique<TotalSegmentatorDemoAlgorithmFactory>(), std::make_unique<TotalSegmentatorDemoControllerFactory>(), nullptr);

		return Status::Success;
	}
}
