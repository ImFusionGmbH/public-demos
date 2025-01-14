/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/AlgorithmControllerFactory.h>
#include <ImFusion/Base/AlgorithmFactory.h>

namespace ImFusion
{
	class Algorithm;

	/// Will be registered via AnnotationHandlePlugin
	class AnnotationHandleAlgorithmFactory : public AlgorithmFactory
	{
	public:
		/// Creates the AnnotationHandleAlgorithmFactory
		AnnotationHandleAlgorithmFactory();
	};

	/// Will be registered via AnnotationHandlePlugin
	class AnnotationHandleControllerFactory : public AlgorithmControllerFactory
	{
	public:
		/// Creates the AnnotationHandleControllerFactory
		AnnotationHandleControllerFactory();

		/// If an AnnotationHandleAlgorithm is passed, returns an AnnotationHandleController associated with it.
		AlgorithmController* create(Algorithm* a) const override;
	};
}
