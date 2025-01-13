/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/Algorithm.h>
#include <ImFusion/Core/Parameter.h>

namespace ImFusion
{
	/// The AnnotationHandleAlgorithm is very simple because the main logic of the interactive overlay is happening in AnnotationHandleController and AnnotationHandle.
	class AnnotationHandleAlgorithm : public Algorithm
	{
	public:
		/// Creates the AnnotationHandleAlgorithm
		AnnotationHandleAlgorithm();

		/// Factory method to check for applicability or to create the algorithm
		static bool createCompatible(const DataList& data, Algorithm** a = nullptr);

		/// Applies the processing (nothing to be done here for this example)
		void compute() override { m_status = static_cast<int>(Status::Success); }

		/// Parameter to make the step size adjustable in the widget
		Parameter<int> p_stepSize = {"stepSize", 2, this};
	};
}
