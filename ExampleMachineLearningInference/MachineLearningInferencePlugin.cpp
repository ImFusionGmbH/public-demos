#include "MachineLearningInferencePlugin.h"

#include "MachineLearningInferenceFactory.h"

// Export free factory function to instantiate plugin
#ifdef WIN32
extern "C" __declspec(dllexport) ImFusion::ImFusionPlugin* createPlugin()
{
	return new ImFusion::MachineLearningInferencePlugin;
}
#else
extern "C" ImFusion::ImFusionPlugin* createPlugin()
{
	return new ImFusion::MachineLearningInferencePlugin;
}
#endif


namespace ImFusion
{
	MachineLearningInferencePlugin::MachineLearningInferencePlugin()
	{
		m_algFactory = new MachineLearningInferenceAlgorithmFactory;
		m_algCtrlFactory = new MachineLearningInferenceControllerFactory;
	}


	MachineLearningInferencePlugin::~MachineLearningInferencePlugin() {}


	const ImFusion::AlgorithmFactory* MachineLearningInferencePlugin::getAlgorithmFactory() { return m_algFactory; }


	const ImFusion::AlgorithmControllerFactory* MachineLearningInferencePlugin::getAlgorithmControllerFactory() { return m_algCtrlFactory; }
}
