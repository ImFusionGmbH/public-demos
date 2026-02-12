#include "StreamExamplesFactory.h"

#include "DemoInputImageStream.h"
#include "DemoStreamDataAlgorithm.h"

#include <ImFusion/GUI/DefaultAlgorithmController.h>
#include <ImFusion/Stream/CreateStreamIoAlgorithm.h>
#include <ImFusion/Stream/StreamControllerBase.h>

namespace ImFusion
{
	StreamExamplesAlgorithmFactory::StreamExamplesAlgorithmFactory()
		: AlgorithmFactory("StreamExamples", false)
	{
		// Second arg is the menu path: "Category;Display Name" -> appears under Demo menu
		registerAlgorithm<DemoStreamDataAlgorithm>("DemoStreamDataAlgorithm", "Demo;Demo Stream Data Algorithm");

		// CreateStreamIoAlgorithm wraps a Stream as an Algorithm for the data model.
		// Template args: <StreamType, autoOpen, autoConnect>
		// false, false = user must manually open/connect the stream after creation
		registerAlgorithm<CreateStreamIoAlgorithm<DemoInputImageStream, false, false>>("DemoInputImageStream",
																								 "IO;Demo Input Image Stream");
	}

	StreamExamplesControllerFactory::StreamExamplesControllerFactory()
		: AlgorithmControllerFactory("StreamExamples", false)
	{
	}

	AlgorithmController* StreamExamplesControllerFactory::create(Algorithm* a) const
	{
		// register the DemoController for the DemoStreamDataAlgorithm
		if (auto alg = dynamic_cast<DemoStreamDataAlgorithm*>(a))
			return new DefaultAlgorithmController(alg);

		// StreamControllerBase provides start/stop/open/close controls.
		// Second arg (true) = show Properties panel alongside stream controls.
		if (auto alg = dynamic_cast<CreateStreamIoAlgorithm<DemoInputImageStream, false, false>*>(a))
			return new StreamControllerBase(alg, true);
		return nullptr;
	}
}
