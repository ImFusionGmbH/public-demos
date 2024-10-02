/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/Algorithm.h>
#include <ImFusion/Core/Parameter.h>
#include <ImFusion/Core/Signal.h>

#include <memory>

namespace ImFusion
{
	class LiveTrackingStream;

	/// Simple demonstration of a custom Algorithm.
	/// This algorithm will consume a LiveTrackingStream and print the tracking matrix to the console.
	class DemoStreamDataAlgorithm : public Algorithm, public SignalReceiver
	{
	public:
		/// Creates the algorithm instance with an image
		explicit DemoStreamDataAlgorithm(LiveTrackingStream& trackingStream);

		/// \name	Methods implementing the algorithm interface
		//\{
		/// Factory method to check for applicability or to create the algorithm
		static bool createCompatible(const DataList& data, Algorithm** a = nullptr);

		/// Applies the processing
		void compute() override;

		/// If new data was created, make it available here
		OwningDataList takeOutput() override;
		//\}

	private:
		const LiveTrackingStream& m_trackingStream;    ///< Input tracking stream
	};
}
