#include "Example2D3DRegistrationPlugin.h"

#include "Example2D3DRegistrationFactory.h"

// Export free factory function to instantiate plugin
#ifdef WIN32
extern "C" __declspec(dllexport) ImFusion::ImFusionPlugin * createPlugin()
{
	return new ImFusion::Example2D3DRegistrationPlugin;
}
#else
extern "C" ImFusion::ImFusionPlugin * createPlugin()
{
	return new ImFusion::Example2D3DRegistrationPlugin;
}
#endif


namespace ImFusion
{
	Example2D3DRegistrationPlugin::Example2D3DRegistrationPlugin() {}


	Example2D3DRegistrationPlugin::~Example2D3DRegistrationPlugin() {}


	const ImFusion::AlgorithmFactory* Example2D3DRegistrationPlugin::getAlgorithmFactory() { return new Example2D3DRegistrationAlgorithmFactory; }


	const ImFusion::AlgorithmControllerFactory* Example2D3DRegistrationPlugin::getAlgorithmControllerFactory()
	{
		return new Example2D3DRegistrationControllerFactory;
	}
}
