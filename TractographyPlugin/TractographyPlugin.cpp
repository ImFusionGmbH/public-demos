#include "TractographyPlugin.h"

#include "FiberDataDisplayHandler.h"
#include "FiberDataIoAlgorithm.h"

#include <ImFusion/Base/AlgorithmControllerFactory.h>
#include <ImFusion/Base/AlgorithmFactory.h>
#include <ImFusion/Base/DataAnnotationFactory.h>
#include <ImFusion/Base/DataComponentFactory.h>
#include <ImFusion/GUI/IoController.h>


IMFUSION_REGISTER_PLUGIN(ImFusion::TractographyPlugin)

namespace ImFusion
{
	class FiberTractAlgorithmFactory : public AlgorithmFactory
	{
	public:
		FiberTractAlgorithmFactory()
			: AlgorithmFactory("TractographyPlugin", false)
		{
			registerAlgorithm<FiberDataIoAlgorithm>("FiberDataIo", "Fiber Tracts");
		}
	};


	// ================================================================================================


	class FiberTractAlgorithmControllerFactory : public AlgorithmControllerFactory
	{
	public:
		AlgorithmController* create(Algorithm* a) const override
		{
			if (FiberDataIoAlgorithm* alg = dynamic_cast<FiberDataIoAlgorithm*>(a))
				return new IoController(alg);
			return nullptr;
		}
	};


	// ================================================================================================


	class FiberTractDataAnnotationFactory : public DataAnnotationFactory
	{
	public:
		InteractiveObject* createDataAnnotation(const std::string& name, Data& data) const override { return nullptr; }
		std::vector<std::string> compatibleDataAnnotations(Data& data) const override { return {}; }
		std::string dataDescription(const Data& data) const override
		{
			if (auto fibers = dynamic_cast<const FiberData*>(&data))
				return "Fiber Tract";
			return "";
		}


		std::string dataIconPath(const Data& data) const override
		{
			if (typeid(data) == typeid(FiberData))
				return ":/icons/IconFiberTract.png";
			return "";
		}
	};


	TractographyPlugin::TractographyPlugin()
	{
		DataDisplayHandlerFactory::registerHandler("TractographyPlugin.FiberDataDisplayHandler",
												   []() { return std::make_unique<FiberDataDisplayHandler>(); });

		DataComponentFactory::get().registerComponent(FiberDataRenderer::Options().id(),
													  []() { return std::make_unique<FiberDataRenderer::Options>(); });
	}


	TractographyPlugin::~TractographyPlugin() {}


	const AlgorithmFactory* TractographyPlugin::getAlgorithmFactory() { return nullptr; }


	const AlgorithmControllerFactory* TractographyPlugin::getAlgorithmControllerFactory() { return new FiberTractAlgorithmControllerFactory; }


	const DataAnnotationFactory* TractographyPlugin::getDataAnnotationFactory() { return new FiberTractDataAnnotationFactory(); }


}
