#include "Example2D3DRegistrationPlugin.h"

#include "Example2D3DRegistrationFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::Example2D3DRegistrationPlugin)

namespace ImFusion
{
	Example2D3DRegistrationPlugin::Example2D3DRegistrationPlugin() = default;

	Example2D3DRegistrationPlugin::~Example2D3DRegistrationPlugin() = default;

	PluginBase::Status Example2D3DRegistrationPlugin::init()
	{
		registerFactories(
			std::make_unique<Example2D3DRegistrationAlgorithmFactory>(), std::make_unique<Example2D3DRegistrationControllerFactory>(), nullptr);
		return Status::Success;
	}
}
