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
		FiberTractAlgorithmControllerFactory()
			: AlgorithmControllerFactory("TractographyPlugin", false)
		{
		}

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


	TractographyPlugin::TractographyPlugin() {}


	TractographyPlugin::~TractographyPlugin() {}

	PluginBase::Status TractographyPlugin::init()
	{
		DataDisplayHandlerFactory::registerHandler("TractographyPlugin.FiberDataDisplayHandler",
												   []() { return std::make_unique<FiberDataDisplayHandler>(); });

		DataComponentFactory::get().registerComponent(FiberDataRenderer::Options().id(),
													  []() { return std::make_unique<FiberDataRenderer::Options>(); });

		registerFactories(std::make_unique<FiberTractAlgorithmFactory>(),
						  std::make_unique<FiberTractAlgorithmControllerFactory>(),
						  std::make_unique<FiberTractDataAnnotationFactory>());

		return Status::Success;
	}
}
