#include "DemoOpenGLFactory.h"

#include "DemoOpenGLAlgorithm.h"
#include "DemoOpenGLController.h"

#include <ImFusion/GUI/DefaultAlgorithmController.h>

namespace ImFusion
{
	DemoOpenGLAlgorithmFactory::DemoOpenGLAlgorithmFactory()
		: AlgorithmFactory("ExampleOpenGL")
	{
		// register the DemoOpenGLAlgorithm
		registerAlgorithm<DemoOpenGLAlgorithm>("DemoOpenGLAlgorithm", "Demo;Demo OpenGL algorithm");
	}

	DemoOpenGLControllerFactory::DemoOpenGLControllerFactory()
		: AlgorithmControllerFactory("ExampleOpenGL")
	{
	}

	AlgorithmController* DemoOpenGLControllerFactory::create(Algorithm* a) const
	{
		// register the DemoOpenGLController for the DemoOpenGLAlgorithm
		if (auto alg = dynamic_cast<DemoOpenGLAlgorithm*>(a))
			return new DemoOpenGLController(alg);
		return nullptr;
	}
}
