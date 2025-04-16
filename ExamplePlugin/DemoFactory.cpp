#include "DemoFactory.h"

#include "DemoAlgorithm.h"
#include "DemoController.h"

namespace ImFusion
{
	DemoAlgorithmFactory::DemoAlgorithmFactory()
		: AlgorithmFactory("ExamplePlugin", false)
	{
		// register the DemoAlgorithm
		registerAlgorithm<DemoAlgorithm>("DemoAlgorithm", "Demo;Demo algorithm");
	}

	DemoControllerFactory::DemoControllerFactory()
		: AlgorithmControllerFactory("ExamplePlugin", false)
	{
	}

	AlgorithmController* DemoControllerFactory::create(Algorithm* a) const
	{
		// register the DemoController for the DemoAlgorithm
		if (auto alg = dynamic_cast<DemoAlgorithm*>(a))
			return new DemoController(alg);
		return nullptr;
	}
}
