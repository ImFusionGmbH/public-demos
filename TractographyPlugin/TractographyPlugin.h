#pragma once

#include <ImFusion/Base/ImFusionPlugin.h>

namespace ImFusion
{
	class AlgorithmFactory;
	class AlgorithmControllerFactory;

	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class TractographyPlugin : public ImFusionPlugin
	{
	public:
		TractographyPlugin();
		virtual ~TractographyPlugin();
		const AlgorithmFactory* getAlgorithmFactory() override;
		const IoAlgorithmFactory* getIoAlgorithmFactory() override;
		const AlgorithmControllerFactory* getAlgorithmControllerFactory() override;
		const DataAnnotationFactory* getDataAnnotationFactory() override;

	};
}
