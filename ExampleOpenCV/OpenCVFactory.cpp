#include "OpenCVFactory.h"

#include "CannyEdgeAlgorithm.h"
#include <ImFusion/GUI/DefaultAlgorithmController.h>

namespace ImFusion
{
	OpenCVFactory::OpenCVFactory()
	{
		// register the CannyEdgeAlgorithm
		registerAlgorithm<CannyEdgeAlgorithm>("OpenCV;Canny edge detection");
	}


	AlgorithmController* OpenCVControllerFactory::create(Algorithm* a) const
	{
		// register the DefaultController for the CannyEdgeAlgorithm
		if (CannyEdgeAlgorithm * alg = dynamic_cast<CannyEdgeAlgorithm*>(a))
			return new DefaultAlgorithmController(alg);
		return 0;
	}
}
