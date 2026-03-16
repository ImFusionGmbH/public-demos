#include "MachineLearningInferenceFactory.h"

#include "RunPretrainedModelAlgorithm.h"

#include <ImFusion/GUI/DefaultAlgorithmController.h>

namespace ImFusion
{
	MachineLearningInferenceAlgorithmFactory::MachineLearningInferenceAlgorithmFactory()
		: AlgorithmFactory("MachineLearningInferencePlugin", false)
	{
		// register the RunPretrainedModelAlgorithm in the sub-category "Machine Learning"
		registerAlgorithm<RunPretrainedModelAlgorithm>("RunPretrainedModel", "Machine Learning;My Demo Machine Learning Algorithm");
	}

	MachineLearningInferenceControllerFactory::MachineLearningInferenceControllerFactory()
		: AlgorithmControllerFactory("MachineLearningInference", false)
	{
	}

	AlgorithmController* MachineLearningInferenceControllerFactory::create(Algorithm* a) const
	{
		// register the MachineLearningInferenceController for the MachineLearningInferenceAlgorithm
		if (RunPretrainedModelAlgorithm* alg = dynamic_cast<RunPretrainedModelAlgorithm*>(a))
			return new DefaultAlgorithmController(alg);
		return 0;
	}
}
