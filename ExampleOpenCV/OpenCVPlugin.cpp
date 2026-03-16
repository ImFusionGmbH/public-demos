#include "OpenCVPlugin.h"

#include "OpenCVFactory.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::OpenCVPlugin)

namespace ImFusion
{
	OpenCVPlugin::OpenCVPlugin() = default;


	OpenCVPlugin::~OpenCVPlugin() = default;

	PluginBase::Status OpenCVPlugin::init()
	{
		registerFactories(std::make_unique<OpenCVFactory>(), std::make_unique<OpenCVControllerFactory>(), nullptr);

		return Status::Success;
	}
}
