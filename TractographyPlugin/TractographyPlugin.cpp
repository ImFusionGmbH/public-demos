#include "TractographyPlugin.h"

#include "FiberDataDisplayHandler.h"
#include "FiberDataIoAlgorithm.h"

#include <ImFusion/Base/AlgorithmControllerFactory.h>
#include <ImFusion/Base/DataAnnotationFactory.h>
#include <ImFusion/Base/DataComponentFactory.h>
#include <ImFusion/Base/IoAlgorithmFactory.h>
#include <ImFusion/GUI/IoController.h>


IMFUSION_REGISTER_PLUGIN(ImFusion::TractographyPlugin)

namespace ImFusion
{
	class FiberTractAlgorithmFactory : public IoAlgorithmFactory
	{
	public:
		FiberTractAlgorithmFactory()
			: IoAlgorithmFactory("")
		{
			registerAlgorithm<FiberDataIoAlgorithm>("Fiber Tracts");
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


	TractographyPlugin::~TractographyPlugin()
	{
	}


	const AlgorithmFactory* TractographyPlugin::getAlgorithmFactory()
	{
		return nullptr;
	}


	const IoAlgorithmFactory* TractographyPlugin::getIoAlgorithmFactory()
	{
		return new FiberTractAlgorithmFactory();
	}


	const AlgorithmControllerFactory* TractographyPlugin::getAlgorithmControllerFactory()
	{
		return new FiberTractAlgorithmControllerFactory;
	}


	const DataAnnotationFactory* TractographyPlugin::getDataAnnotationFactory()
	{
		return new FiberTractDataAnnotationFactory();
	}


}
