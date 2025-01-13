#include "AnnotationHandlePlugin.h"

#include "AnnotationHandleAlgorithmFactory.h"

// Export free factory function to instantiate plugin
IMFUSION_REGISTER_PLUGIN(ImFusion::AnnotationHandlePlugin)

namespace ImFusion
{
	AnnotationHandlePlugin::AnnotationHandlePlugin() = default;
	AnnotationHandlePlugin::~AnnotationHandlePlugin() = default;

	const AlgorithmFactory* AnnotationHandlePlugin::getAlgorithmFactory() { return new AnnotationHandleAlgorithmFactory; }

	const AlgorithmControllerFactory* AnnotationHandlePlugin::getAlgorithmControllerFactory() { return new AnnotationHandleControllerFactory; }
}
