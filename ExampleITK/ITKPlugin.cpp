#include "ITKPlugin.h"

#include "ITKFactory.h"

//! [Export free factory function to instantiate plugin]
#ifdef WIN32
extern "C" __declspec(dllexport) ImFusion::ImFusionPlugin* createPlugin()
{
	return new ImFusion::ITKPlugin;
}
#else
extern "C" ImFusion::ImFusionPlugin* createPlugin()
{
	return new ImFusion::ITKPlugin;
}
#endif
//! [Export free factory function to instantiate plugin]


namespace ImFusion
{
	ITKPlugin::ITKPlugin()
	{
		m_algFactory = new ITKAlgorithmFactory;
		m_algCtrlFactory = new ITKControllerFactory;
	}


	ITKPlugin::~ITKPlugin() {}


	const ImFusion::AlgorithmFactory* ITKPlugin::getAlgorithmFactory() { return m_algFactory; }


	const ImFusion::AlgorithmControllerFactory* ITKPlugin::getAlgorithmControllerFactory() { return m_algCtrlFactory; }
}
