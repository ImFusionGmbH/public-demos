#include "DemoPlugin.h"

#include "DemoFactory.h"

// Export free factory function to instantiate plugin
#ifdef WIN32
extern "C" __declspec(dllexport) ImFusion::ImFusionPlugin* createPlugin()
{
	return new ImFusion::DemoPlugin;
}
#else
extern "C" ImFusion::ImFusionPlugin* createPlugin()
{
	return new ImFusion::DemoPlugin;
}
#endif


namespace ImFusion
{
	DemoPlugin::DemoPlugin() {}


	DemoPlugin::~DemoPlugin() {}


	const ImFusion::AlgorithmFactory* DemoPlugin::getAlgorithmFactory() { return new DemoAlgorithmFactory; }


	const ImFusion::AlgorithmControllerFactory* DemoPlugin::getAlgorithmControllerFactory() { return new DemoControllerFactory; }
}
