#include "StreamExamplesPlugin.h"

#include "StreamExamplesFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::StreamExamplesPlugin)

namespace ImFusion
{
	StreamExamplesPlugin::StreamExamplesPlugin() {}


	StreamExamplesPlugin::~StreamExamplesPlugin() = default;

	PluginBase::Status StreamExamplesPlugin::init()
	{
		// Registers AlgorithmFactory, ControllerFactory, (optional) DataAnnotationFactory to make
		// the algorithms and controllers available
		registerFactories(std::make_unique<StreamExamplesAlgorithmFactory>(), std::make_unique<StreamExamplesControllerFactory>(), nullptr);

		return Status::Success;
	}
}
