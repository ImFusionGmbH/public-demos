#include "OpenCVPlugin.h"

#include "OpenCVFactory.h"

// Export free factory function to instantiate plugin
#ifdef WIN32
extern "C" __declspec(dllexport) ImFusion::ImFusionPlugin* createPlugin()
{
	return new ImFusion::OpenCVPlugin;
}
#else
extern "C" ImFusion::ImFusionPlugin* createPlugin()
{
	return new ImFusion::OpenCVPlugin;
}
#endif


namespace ImFusion
{
	OpenCVPlugin::OpenCVPlugin()
	{
		m_algFactory = new OpenCVFactory;
		m_algCtrlFactory = new OpenCVControllerFactory;
	}


	OpenCVPlugin::~OpenCVPlugin() {}


	const ImFusion::AlgorithmFactory* OpenCVPlugin::getAlgorithmFactory() { return m_algFactory; }


	const ImFusion::AlgorithmControllerFactory* OpenCVPlugin::getAlgorithmControllerFactory() { return m_algCtrlFactory; }
}
