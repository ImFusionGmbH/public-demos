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
		// register the DemoStreamDataAlgorithm
		registerAlgorithm<DemoStreamDataAlgorithm>("DemoStreamDataAlgorithm", "Demo;Demo Stream Data Algorithm");

		// register the Stream. The last two template argument (false, false) set the stream to not be opened or connected on creation
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

		// use a default controller (which is a simple GUI for all Parameters) for the DemoInputImageStream
		if (auto alg = dynamic_cast<CreateStreamIoAlgorithm<DemoInputImageStream, false, false>*>(a))
			return new StreamControllerBase(alg, true);
		return nullptr;
	}
}
