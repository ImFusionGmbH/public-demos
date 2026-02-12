#include "DemoStreamDataAlgorithm.h"

#include <ImFusion/Base/DataList.h>
#include <ImFusion/Base/MemImage.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Core/GL/ImageProgram.h>
#include <ImFusion/Core/GL/Texture.h>
#include <ImFusion/GL/GlImage.h>
#include <ImFusion/Stream/TrackingStream.h>
#include <ImFusion/Stream/TrackingStreamData.h>


namespace ImFusion
{
	DemoStreamDataAlgorithm::DemoStreamDataAlgorithm(TrackingStream& trackingStream)
		: m_trackingStream(trackingStream)
	{
		// Connect to stream's signalStreamData. The callback runs on the stream's worker thread,
		// so avoid blocking operations or UI updates here.
		m_trackingStream.signalStreamData.connect(this, [](std::shared_ptr<const StreamData> streamData) {
			// typed<T>() safely casts the base StreamData to the expected type
			auto tsd = streamData->typed<TrackingStreamData>();
			if (!tsd)
			{
				LOG_ERROR("DemoStreamDataAlgorithm", "Tracking Stream data is null");
				return;
			}

			const std::vector<ImFusion::TrackingInstrument> instruments = tsd->instruments();
			if (instruments.empty())
				return;

			for (const auto& ti : instruments)
			{
				LOG_INFO("DemoStreamDataAlgorithm",
						 "Instrument: " + ti.id.toIDModelNameString() << "\n"
																	  << "Matrix:\n"
																	  << ti.matrix << "\n"
																	  << "Quality:\n"
																	  << ti.quality);
			}
		});
	}


	bool DemoStreamDataAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		// we expect exactly one tracking stream
		if (data.size() != 1)
			return false;
		std::vector<Data*> streams = data.getAll(Data::TRACKINGSTREAM);
		if (streams.empty())
			return false;
		auto ts = dynamic_cast<TrackingStream*>(streams[0]);
		if (!ts)
			return false;

		// Requirements are met, create the algorithm if asked
		if (a)
			*a = new DemoStreamDataAlgorithm(*ts);
		return true;
	}


	void DemoStreamDataAlgorithm::compute()
	{
		// set generic error status until we have finished
		m_status = static_cast<int>(Status::Error);

		// set algorithm status to success
		m_status = static_cast<int>(Status::Success);
	}


	OwningDataList DemoStreamDataAlgorithm::takeOutput() { return {}; }
}
