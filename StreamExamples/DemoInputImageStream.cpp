#include "DemoInputImageStream.h"

#include <ImFusion/Base/MemImage.h>
#include <ImFusion/Base/TypedImage.h>
#include <ImFusion/Core/Log.h>
#include <ImFusion/GL/GlImage.h>
#include <ImFusion/GL/SharedImage.h>
#include <ImFusion/Stream/ImageStreamData.h>


namespace ImFusion
{
	DemoInputImageStream::DemoInputImageStream()
		: ImageStream("DemoInputImageStream")
	{
	}


	// Always call close() in destructor to ensure proper cleanup and state transition
	// before member destruction.
	DemoInputImageStream::~DemoInputImageStream() { close(); }


	bool DemoInputImageStream::openImpl()
	{
		// Open a connection to your image source here, e.g. a camera or a network stream, and return true if successful
		return true;
	}


	bool DemoInputImageStream::closeImpl()
	{
		// Close the connection to your image source here, and return true if successful
		return true;
	}


	bool DemoInputImageStream::startImpl()
	{
		// Send a start signal to your image source, if applicable, and return true if successful
		return true;
	}


	bool DemoInputImageStream::stopImpl()
	{
		// Send a stop signal to your image source, if applicable, and return true if successful
		return true;
	}


	std::optional<Stream::WorkContinuation> DemoInputImageStream::doWork()
	{
		const int intervalMs = 10;
		const auto nextIterationTime = std::chrono::system_clock::now() + std::chrono::milliseconds(intervalMs);

		// Get the next frame from the image source, preprocess it if necessary, and emit it
		TypedImage<uint8_t> memImage(vec3i{64, 128, 1}, /*numChannels = */ 1);

		// In this demo, we simply fill a single pixel in the image with a color based on the frame counter
		memImage.fill(0);                                         // Fill the image with zeros (black)
		memImage.pointer()[m_frameCounter % (64 * 128)] = 255;    // Set one pixel to white
		m_frameCounter++;

		// SharedImage is a unified container that can hold CPU (MemImage), GPU (GlImage),
		// or both representations. Consumers can request the format they need.
		auto sharedImage = std::make_unique<SharedImage>(std::move(memImage));

		// Set the metadata for the image, e.g. spacing, modality, etc.
		sharedImage->setSpacing({0.75, 0.42, 1.0});
		sharedImage->setModality(Data::Modality::ULTRASOUND);

		// Emit the image
		// Note how we do not reuse the memImage or the sharedImage after this point.
		// This is required, because we might otherwise modify data a consumers of this stream is working with.
		auto streamData = std::make_shared<ImageStreamData>(this, std::move(sharedImage));
		signalStreamData.emitSignal(streamData);

		// This will trigger the next iteration at the specified time.
		// Alternatively you can also return `WorkContinuation{}` to run the next iteration as soon as possible
		return WorkContinuation{nextIterationTime};
	}
}
