#include "DemoDicomExtensionFactory.h"

namespace ImFusion
{
	DemoDicomExtensionAlgorithmFactory::DemoDicomExtensionAlgorithmFactory()
		: AlgorithmFactory("ExampleDicom", false)
	{
	}

	DemoDicomExtensionControllerFactory::DemoDicomExtensionControllerFactory()
		: AlgorithmControllerFactory("ExampleDicom", false)
	{
	}

	AlgorithmController* DemoDicomExtensionControllerFactory::create(Algorithm* a) const { return nullptr; }
}
