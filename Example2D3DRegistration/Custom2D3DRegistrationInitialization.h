/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/CT/XRay2D3DRegistrationInitialization.h>
#include <ImFusion/CT/XRay2D3DRegistrationInitializationKeyPoints.h>

namespace ImFusion {

	using namespace CT;

	class Custom2D3DRegistrationInitialization : public XRay2D3DRegistrationInitialization
	{
	public:
		Custom2D3DRegistrationInitialization(XRay2D3DRegistrationAlgorithm& regAlg, mat4 groundTruthPose);

		// Implement XRay2D3DRegistrationInitialization interface
		bool canInitialize() const override { return true; }
		Utils::Optional<XRay2D3DRegistrationInitialization::InitializationResult> initialize(ConeBeamGeometry& geom, SharedImageSet& shots, const SharedImageSet& volume, MaskEditor* maskAlgorithm) override;

		// Additional getter method
		XRay2D3DRegistrationInitializationKeyPoints* kpAlg() { return m_kpAlg.get(); };

	private:
		std::unique_ptr<XRay2D3DRegistrationInitializationKeyPoints> m_kpAlg;
		mat4 m_groundTruthPose;
	};
}