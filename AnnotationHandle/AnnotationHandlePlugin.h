/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include "AnnotationHandleAlgorithmFactory.h"

#include <ImFusion/Base/ImFusionLibPlugin.h>

namespace ImFusion
{
	/// Minimal example for defining a plugin implementing a custom interactive overlay for GlPointBasedAnnotations
	class AnnotationHandlePlugin : public ImFusionLibPlugin
	{
	public:
		/// Create the AnnotationHandlePlugin
		AnnotationHandlePlugin();
		/// Destroy the AnnotationHandlePlugin
		~AnnotationHandlePlugin() override;

		static const char* id() { return "Demo.AnnotationHandle"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		Status init() override;
	};
}
