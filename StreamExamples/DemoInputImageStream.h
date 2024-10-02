#pragma once

#include <ImFusion/Core/Parameter.h>
#include <ImFusion/Stream/ImageStream.h>

#include <optional>


namespace ImFusion
{
	class DemoInputImageStream : public ImageStream, public SignalReceiver
	{
	public:
		explicit DemoInputImageStream();

		~DemoInputImageStream() override;

		// Inherited via ImageStream
		std::string uuid() override;

		// For the parametrization of the stream, you can use members of type Parameter<T>
		Parameter<std::string> p_streamUrl{"streamUrl", "<default-url>", *this};

	protected:
		// Implementations of the stream interface for state changes.
		// openImpl is called when the stream is requested to open, and the state is changed only if it was successful.
		// Similarly for closeImpl, startImpl and stopImpl.
		bool openImpl() override;
		bool closeImpl() override;
		bool startImpl() override;
		bool stopImpl() override;

		// Each Stream has a statemachine, that changes with the open, start, stop and close methods.
		// When the stream is running, this method is called in a loop by the Stream base class, on it's own thread, allowing the class to poll for new data and then emit the signal with it.
		// Note: But Streams are not limited to this. The signalNewData can also be emitted from any other thread / callback.
		//       In that case you only have to ensure that the thread has an OpenGL context associated with it, see
		//       the GL::ContextManager::createContext() method.
		std::optional<WorkContinuation> doWork() override;

	private:
		// Add any private members you need for your image source here
		int m_frameCounter = 0;    // Example frame counter, just to create the demo images
	};
}
