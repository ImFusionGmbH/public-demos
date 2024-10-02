#include "DemoStreamDataAlgorithm.h"

#include <ImFusion/Base/DataList.h>
#include <ImFusion/Base/MemImage.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Core/GL/ImageProgram.h>
#include <ImFusion/Core/GL/Texture.h>
#include <ImFusion/GL/GlImage.h>
#include <ImFusion/Stream/LiveTrackingStream.h>
#include <ImFusion/Stream/TrackingStreamData.h>


namespace ImFusion
{
	DemoStreamDataAlgorithm::DemoStreamDataAlgorithm(LiveTrackingStream& trackingStream)
		: m_trackingStream(trackingStream)
	{
		m_trackingStream.signalNewData.connect(this, [](auto& streamData) {
			auto tsd = streamData.template typed<TrackingStreamData>();
			if (!tsd)
			{
				LOG_ERROR("DemoStreamDataAlgorithm", "Tracking Stream data is null");
				return;
			}

			auto instruments = tsd->instruments();
			if (instruments.empty())
				return;

			for (const auto& ti : instruments)
			{
				LOG_INFO("DemoStreamDataAlgorithm",
						 "Instrument: " + ti.name << "\n"
												  << "Matrix:\n"
												  << ti.matrix << "\n"
												  << "Quality:\n"
												  << ti.quality);
			}
		});
	}


	bool DemoStreamDataAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		// we expect exactly one tracking stream, and optionally one image
		if (data.size() != 1)
			return false;
		std::vector<Data*> streams = data.getAll(Data::LIVETRACKINGSTREAM);
		if (streams.empty())
			return false;
		auto ts = dynamic_cast<LiveTrackingStream*>(streams[0]);
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
