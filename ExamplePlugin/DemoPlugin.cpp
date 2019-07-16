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
	DemoPlugin::DemoPlugin()
	{
		m_algFactory = new DemoAlgorithmFactory;
		m_algCtrlFactory = new DemoControllerFactory;
	}


	DemoPlugin::~DemoPlugin() {}


	const ImFusion::AlgorithmFactory* DemoPlugin::getAlgorithmFactory() { return m_algFactory; }


	const ImFusion::AlgorithmControllerFactory* DemoPlugin::getAlgorithmControllerFactory() { return m_algCtrlFactory; }
}
