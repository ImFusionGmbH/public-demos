/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/ImFusionPlugin.h>

namespace ImFusion
{
	/// Minimal example for defining a plugin implementing a custom interactive overlay for GlPointBasedAnnotations
	class AnnotationHandlePlugin : public ImFusionPlugin
	{
	public:
		/// Create the AnnotationHandlePlugin
		AnnotationHandlePlugin();
		/// Destroy the AnnotationHandlePlugin
		~AnnotationHandlePlugin() override;
		/// Returns an AnnotationHandleAlgorithmFactory.
		const AlgorithmFactory* getAlgorithmFactory() override;
		/// Returns an AnnotationHandleControllerFactory.
		const AlgorithmControllerFactory* getAlgorithmControllerFactory() override;
	};
}
