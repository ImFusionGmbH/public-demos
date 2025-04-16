#include "AnnotationHandleAlgorithmFactory.h"

#include "AnnotationHandleAlgorithm.h"
#include "AnnotationHandleController.h"

namespace ImFusion
{
	AnnotationHandleAlgorithmFactory::AnnotationHandleAlgorithmFactory()
		: AlgorithmFactory("AnnotationHandle", false)
	{
		// register the AnnotationHandleAlgorithm
		registerAlgorithm<AnnotationHandleAlgorithm>("AnnotationHandleAlgorithm", "Demo;Handle for Annotations");
	}

	AnnotationHandleControllerFactory::AnnotationHandleControllerFactory()
		: AlgorithmControllerFactory("AnnotationHandle", false)
	{
	}

	AlgorithmController* AnnotationHandleControllerFactory::create(Algorithm* a) const
	{
		// register the AnnotationHandleController for the AnnotationHandleAlgorithm
		if (auto alg = dynamic_cast<AnnotationHandleAlgorithm*>(a))
			return new AnnotationHandleController(alg);
		return nullptr;
	}
}
