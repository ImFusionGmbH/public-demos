#include "DemoAnatomyFactory.h"

#include "DemoAnatomyAlgorithm.h"

#include <ImFusion/GUI/DefaultAlgorithmController.h>

namespace ImFusion
{
	DemoAnatomyAlgorithmFactory::DemoAnatomyAlgorithmFactory()
	: AlgorithmFactory("DemoAnatomy", false)
	{
		// register the DemoAlgorithm
		registerAlgorithm<DemoAnatomyAlgorithm>("DemoAnatomy", "Demo;Anatomy Demo algorithm");
	}

	DemoAnatomyControllerFactory::DemoAnatomyControllerFactory()
		: AlgorithmControllerFactory("DemoAnatomy", false)
	{
	}

	AlgorithmController* DemoAnatomyControllerFactory::create(Algorithm* a) const
	{
		// register the DemoController for the DemoAlgorithm
		if (auto alg = dynamic_cast<DemoAnatomyAlgorithm*>(a))
			return new DefaultAlgorithmController(alg);
		return nullptr;
	}
}
