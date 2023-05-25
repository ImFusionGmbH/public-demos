#include "Example2D3DRegistrationAlgorithm.h"

#include <ImFusion/Base/DataList.h>
#include <ImFusion/Base/ImageProcessing.h>
#include <ImFusion/Base/MemImage.h>
#include <ImFusion/Base/Pose.h>
#include <ImFusion/Base/SharedImage.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Base/Utils/Images.h>
#include <ImFusion/Core/Log.h>
#include <ImFusion/CT/ConeBeamData.h>
#include <ImFusion/CT/ConeBeamSimulation.h>
#include <ImFusion/CT/XRay2D3DRegistrationAlgorithm.h>

// The following sets the log category for this file to "Example2D3DRegistration"
#undef IMFUSION_LOG_DEFAULT_CATEGORY
#define IMFUSION_LOG_DEFAULT_CATEGORY "Example2D3DRegistration"

namespace ImFusion
{
	Example2D3DRegistrationAlgorithm::Example2D3DRegistrationAlgorithm(SharedImageSet* volumeIn)
		: m_volumeIn(volumeIn)
	{
		// Convert the volume to use unsigned values internally.
		// ConeBeamSimulation currently uses the storage values of the volume,
		// so this is needed to avoid negative values.
		m_volumeIn->prepare();
	}

	Example2D3DRegistrationAlgorithm::~Example2D3DRegistrationAlgorithm()
	{
		/// Since the registration algorithm uses the projections, make sure to delete registration algorithm first.
		m_regAlg = nullptr;
		m_projections = nullptr;
	}


	bool Example2D3DRegistrationAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		// check requirements to create the algorithm. In this case, we want to take in a single volume.

		if (data.size() != 1)
			return false;

		SharedImageSet* img = data.getImage(Data::VOLUME);
		if (img == nullptr)
			return false;

		// requirements are met, create the algorithm if asked
		if (a)
		{
			*a = new Example2D3DRegistrationAlgorithm(img);
		}
		return true;
	}


	// This function does all of the work of this class
	void Example2D3DRegistrationAlgorithm::compute()
	{
		// Basic consistency check.
		if (m_volumeIn == nullptr)
		{
			LOG_ERROR("Algorithm incorrectly initialized");
			return;
		}

		// We create some simulated X-ray images.
		CT::ConeBeamSimulation simulation(*m_volumeIn->get(0));
		// This is the pose of the simulated X-ray images relative to the volume.
		mat4 groundTruthIso = Pose::eulerToMat(vec3(30.0, 70.0, 3.0), vec3(5.0, -5.0, 2.0));
		simulation.geometry().setIsoMatrix(groundTruthIso);

		auto& geom = simulation.geometry();
		geom.sourceDetDistance = 1000.0;
		geom.sourcePatDistance = 500.0;
		geom.detSizeX = 200;
		geom.detSizeY = 200;
		geom.angleRange = 90;
		Properties p;
		simulation.configuration(&p);
		p.setParam("width", 384);
		p.setParam("height", 512);
		p.setParam("frames", 2);
		p.setParam("i0", 0.0);
		p.setParam("volPars/reconSize", 300);
		simulation.configure(&p);
		simulation.compute();    //<This runs the simulation

		// We save the result
		m_projections = simulation.takeOutput().extractFirst<CT::ConeBeamData>();
		Utils::autoWindow(*m_projections);

		// Reset the iso parameters to a different pose differing from the ground truth.
		m_projections->geometry().setIsoMatrix(Pose::eulerToMat(vec3(200, 2.0, 3.0), vec3(-10.0, 0.0, 0.0)));

		// Start an instance of XRay2D3DRegistrationAlgorithm with the volume and the projections.
		// We set a custom initialization mode with an instance of Custom2D3DRegistrationInitialization
		// that implements the initialization of the registration.
		m_regAlg = std::make_unique<CT::XRay2D3DRegistrationAlgorithm>(*m_projections, *m_volumeIn);
		m_regAlg->p_initializationMode = CT::XRay2D3DRegistrationAlgorithm::InitializationMode::Custom;
		auto customInit = std::make_unique<Custom2D3DRegistrationInitialization>(*m_regAlg, groundTruthIso);
		m_customInit = customInit.get();
		m_regAlg->setCustomInitialization(std::move(customInit));
	}


	OwningDataList Example2D3DRegistrationAlgorithm::takeOutput()
	{
		// if we have produced some output, add it to the list
		return OwningDataList(std::move(m_projections));
	}
}