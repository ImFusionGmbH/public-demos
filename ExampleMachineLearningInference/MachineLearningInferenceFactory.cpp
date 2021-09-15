#include "MachineLearningInferenceFactory.h"
#include "RunPretrainedModelAlgorithm.h"

#include <ImFusion/GUI/DefaultAlgorithmController.h>

namespace ImFusion
{
	MachineLearningInferenceAlgorithmFactory::MachineLearningInferenceAlgorithmFactory()
	{
		// register the RunPretrainedModelAlgorithm in the sub-category "Machine Learning"
		registerAlgorithm<RunPretrainedModelAlgorithm>("Machine Learning;My Demo Machine Learning Algorithm");
	}


	AlgorithmController* MachineLearningInferenceControllerFactory::create(Algorithm* a) const
	{
		// register the MachineLearningInferenceController for the MachineLearningInferenceAlgorithm
		if (RunPretrainedModelAlgorithm* alg = dynamic_cast<RunPretrainedModelAlgorithm*>(a))
			return new DefaultAlgorithmController(alg);
		return 0;
	}
}
