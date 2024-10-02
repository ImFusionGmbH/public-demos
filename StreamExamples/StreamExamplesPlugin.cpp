#include "StreamExamplesPlugin.h"

#include "StreamExamplesFactory.h"

IMFUSION_REGISTER_PLUGIN(ImFusion::StreamExamplesPlugin)

namespace ImFusion
{
	StreamExamplesPlugin::StreamExamplesPlugin() = default;


	StreamExamplesPlugin::~StreamExamplesPlugin() = default;


	const AlgorithmFactory* StreamExamplesPlugin::getAlgorithmFactory() { return new StreamExamplesAlgorithmFactory(); }


	const AlgorithmControllerFactory* StreamExamplesPlugin::getAlgorithmControllerFactory() { return new StreamExamplesControllerFactory(); }
}
