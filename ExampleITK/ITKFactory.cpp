#include "ITKFactory.h"

#include "ITKCannyEdgeAlgorithm.h"
#include <ImFusion/GUI/DefaultAlgorithmController.h>

namespace ImFusion
{
	ITKAlgorithmFactory::ITKAlgorithmFactory()
	{
		registerAlgorithm<ITKCannyEdgeAlgorithm>("ITK;Canny Edge");
	}

	AlgorithmController* ITKControllerFactory::create(Algorithm* a) const
	{
		if (ITKCannyEdgeAlgorithm * alg = dynamic_cast<ITKCannyEdgeAlgorithm*>(a))
			return new DefaultAlgorithmController(alg);
		return 0;
	}
}
