#include "ITKFactory.h"

#include "ITKCannyEdgeAlgorithm.h"

#include <ImFusion/GUI/DefaultAlgorithmController.h>

namespace ImFusion
{
	ITKAlgorithmFactory::ITKAlgorithmFactory()
		: AlgorithmFactory("ITKPlugin")
	{
		registerAlgorithm<ITKCannyEdgeAlgorithm>("ITKCannyEdge", "ITK;Canny Edge");
	}

	ITKControllerFactory::ITKControllerFactory()
		: AlgorithmControllerFactory("ITK")
	{
	}

	AlgorithmController* ITKControllerFactory::create(Algorithm* a) const
	{
		if (ITKCannyEdgeAlgorithm* alg = dynamic_cast<ITKCannyEdgeAlgorithm*>(a))
			return new DefaultAlgorithmController(alg);
		return 0;
	}
}
