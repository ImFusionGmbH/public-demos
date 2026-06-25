#include "DemoImageMathFactory.h"

#include "DemoImageMathAlgorithm.h"

#include <ImFusion/GUI/DefaultAlgorithmController.h>

namespace ImFusion
{
	DemoImageMathAlgorithmFactory::DemoImageMathAlgorithmFactory()
		: AlgorithmFactory("ExampleImageMath")
	{
		// register the DemoImageMathAlgorithm
		registerAlgorithm<DemoImageMathAlgorithm>("DemoImageMathAlgorithm", "Demo;Demo ImageMath algorithm");
	}

	DemoImageMathControllerFactory::DemoImageMathControllerFactory()
		: AlgorithmControllerFactory("ExampleImageMath")
	{
	}

	AlgorithmController* DemoImageMathControllerFactory::create(Algorithm* a) const
	{
		// register the DemoController for the DemoImageMathAlgorithm
		if (auto alg = dynamic_cast<DemoImageMathAlgorithm*>(a))
			return new DefaultAlgorithmController(alg);
		return nullptr;
	}
}
