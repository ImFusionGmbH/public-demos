/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include "Custom2D3DRegistrationInitialization.h"

#include <ImFusion/Base/Algorithm.h>
#include <ImFusion/CT/ConeBeamData.h>
#include <ImFusion/CT/XRay2D3DRegistrationAlgorithm.h>

#include <memory>

namespace ImFusion
{
	using namespace CT;

	class SharedImageSet;

	// Demonstration of how to use the 2D/3D Registration algorithm from the SDK.
	class Example2D3DRegistrationAlgorithm : public Algorithm
	{
	public:
		// Creates the algorithm instance with an image
		Example2D3DRegistrationAlgorithm(SharedImageSet* img);
		~Example2D3DRegistrationAlgorithm();


		// \name	Methods implementing the algorithm interface
		//\{
		// Factory method to check for applicability or to create the algorithm
		static bool createCompatible(const DataList& data, Algorithm** a = 0);

		void compute() override;

		// If new data was created, make it available here
		OwningDataList takeOutput() override;
		//\}

		// Getter and setter methods required by the controller.
		XRay2D3DRegistrationAlgorithm* regAlg() { return m_regAlg.get(); }
		Custom2D3DRegistrationInitialization* customInit() { return m_customInit; };


	private:
		SharedImageSet* m_volumeIn = nullptr;                            //< Volume
		std::unique_ptr<ConeBeamData> m_projections;                     //< Projections
		std::unique_ptr<XRay2D3DRegistrationAlgorithm> m_regAlg;         //< Nested Registration Algorithm
		Custom2D3DRegistrationInitialization* m_customInit = nullptr;    //< Pointer to custom initialization object.
	};
}
