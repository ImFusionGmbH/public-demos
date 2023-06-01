/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/CT/XRay2D3DRegistrationInitialization.h>
#include <ImFusion/CT/XRay2D3DRegistrationInitializationKeyPoints.h>

namespace ImFusion {


	class Custom2D3DRegistrationInitialization : public CT::XRay2D3DRegistrationInitialization
	{
	public:
		Custom2D3DRegistrationInitialization(CT::XRay2D3DRegistrationAlgorithm& regAlg, mat4 groundTruthPose);

		// Implement XRay2D3DRegistrationInitialization interface
		bool canInitialize() const override { return true; }
		std::optional<CT::XRay2D3DRegistrationInitialization::InitializationResult> initialize(CT::ConeBeamGeometry& geom, SharedImageSet& shots, const SharedImageSet& volume, MaskEditor* maskAlgorithm) override;

		// Additional getter method
		CT::XRay2D3DRegistrationInitializationKeyPoints* kpAlg() { return m_kpAlg.get(); };

	private:
		std::unique_ptr<CT::XRay2D3DRegistrationInitializationKeyPoints> m_kpAlg;
		mat4 m_groundTruthPose;
	};
}