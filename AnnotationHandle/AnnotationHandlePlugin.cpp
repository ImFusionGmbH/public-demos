#include "AnnotationHandlePlugin.h"

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::AnnotationHandlePlugin)

namespace ImFusion
{
	AnnotationHandlePlugin::AnnotationHandlePlugin() = default;

	AnnotationHandlePlugin::~AnnotationHandlePlugin() = default;

	PluginBase::Status AnnotationHandlePlugin::init()
	{
		registerFactories(std::make_unique<AnnotationHandleAlgorithmFactory>(), std::make_unique<AnnotationHandleControllerFactory>(), nullptr);
		return Status::Success;
	};
}
