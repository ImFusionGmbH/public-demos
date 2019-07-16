#include "DemoFactory.h"

#include "DemoAlgorithm.h"
#include "DemoController.h"

namespace ImFusion
{
	DemoAlgorithmFactory::DemoAlgorithmFactory()
	{
		// register the DemoAlgorithm
		registerAlgorithm<DemoAlgorithm>("Demo;Demo algorithm");
	}


	AlgorithmController* DemoControllerFactory::create(Algorithm* a) const
	{
		// register the DemoController for the DemoAlgorithm
		if (DemoAlgorithm* alg = dynamic_cast<DemoAlgorithm*>(a))
			return new DemoController(alg);
		return 0;
	}
}
