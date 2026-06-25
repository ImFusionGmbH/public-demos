#include "DemoDicomExtensionFactory.h"

namespace ImFusion
{
	DemoDicomExtensionAlgorithmFactory::DemoDicomExtensionAlgorithmFactory()
		: AlgorithmFactory("ExampleDicom")
	{
	}

	DemoDicomExtensionControllerFactory::DemoDicomExtensionControllerFactory()
		: AlgorithmControllerFactory("ExampleDicom")
	{
	}

	AlgorithmController* DemoDicomExtensionControllerFactory::create(Algorithm* a) const { return nullptr; }
}
