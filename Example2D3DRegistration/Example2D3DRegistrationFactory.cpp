#include <type_traits>
#include "Example2D3DRegistrationFactory.h"

#include "Example2D3DRegistrationAlgorithm.h"
#include "Example2D3DRegistrationController.h"

namespace ImFusion
{
	Example2D3DRegistrationAlgorithmFactory::Example2D3DRegistrationAlgorithmFactory()
	{
		// register the Example2D3DRegistrationAlgorithm
		registerAlgorithm<Example2D3DRegistrationAlgorithm>("Example2D3DRegistration;Example2D3DRegistration algorithm");
	}

	AlgorithmController* Example2D3DRegistrationControllerFactory::create(Algorithm* a) const
	{
		// register the Example2D3DRegistrationController for the Example2D3DRegistrationAlgorithm
		if (Example2D3DRegistrationAlgorithm* alg = dynamic_cast<Example2D3DRegistrationAlgorithm*>(a))
			return new Example2D3DRegistrationController(alg);
		return 0;
	}
}