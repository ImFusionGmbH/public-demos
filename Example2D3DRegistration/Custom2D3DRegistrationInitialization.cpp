
#include "Custom2D3DRegistrationInitialization.h"

#include <ImFusion/Base/Log.h>
#include <ImFusion/CT/XRay2D3DRegistrationInitialization.h>
#include <ImFusion/CT/XRay2D3DRegistrationInitializationKeyPoints.h>
#include <ImFusion/CT/ConeBeamData.h>
#include <ImFusion/Core/Utils/Optional.h>

#undef IMFUSION_LOG_DEFAULT_CATEGORY
#define IMFUSION_LOG_DEFAULT_CATEGORY "Example2D3DRegistrationController"

using namespace ImFusion;

// In this constructor, we just created an instance of the key points initialization method we need
// for the bundle adjustment later.
Custom2D3DRegistrationInitialization::Custom2D3DRegistrationInitialization(XRay2D3DRegistrationAlgorithm& regAlg, mat4 groundTruthPose)
	: XRay2D3DRegistrationInitialization(regAlg)
	, m_kpAlg(std::make_unique<XRay2D3DRegistrationInitializationKeyPoints>(regAlg))
	, m_groundTruthPose(groundTruthPose)
{
}

// This class performs the initialization
Utils::Optional<XRay2D3DRegistrationInitialization::InitializationResult> Custom2D3DRegistrationInitialization::initialize(ConeBeamGeometry& geom,
																	   SharedImageSet& shots,
																	   const SharedImageSet& volume,
																	   MaskEditor* maskAlgorithm)
{
	if (m_kpAlg == nullptr)    // basic consistency check
	{
		LOG_ERROR("Custom initialization not correctly initialized");
		return Utils::Optional<InitializationResult>();
	}

	// We will configure the m_kpAlg member using the Properties interface, described in more
	// detail in Properties.h and the XRay2D3DRegistrationInitializationKeyPoints.h header file.
	Properties keyPointsConfig;

	// The reference locations of three keypoints in world-coordinates are specified
	vec3 keyPoint0World(0.0, -40.0, 0.0);
	vec3 keyPoint1World(0.0, -30.0, 30.0);
	vec3 keyPoint2World(0.0, -60.0, 30.0);
	// We project these reference locations onto the x-ray images.
	for (int i = 0; i < shots.size(); i++)
	{

                // We calculate the forward projection of the points with the "ground Truth" pose we are given
                // To do this, the `toGroundTruth` function calculates the locations of these points so that the
                // images with the current pose are the same as the images of the key points under the ground truth pose.
                mat4 currentIsoMatrix = m_regAlg->shotsWithGeom().geometry().isoMatrix();
                auto toGroundTruth = [this,&currentIsoMatrix](vec3 inputPoint) -> vec3 { return (currentIsoMatrix.inverse()*m_groundTruthPose* inputPoint.homogeneous()).hnormalized().eval(); };
                //Calculate forward projection onto the detector
		vec2 kp0ForwardProjectioni = m_kpAlg->forwardProjection(toGroundTruth(keyPoint0World), i);
		vec2 kp1ForwardProjectioni = m_kpAlg->forwardProjection(toGroundTruth(keyPoint1World), i);
		vec2 kp2ForwardProjectioni = m_kpAlg->forwardProjection(toGroundTruth(keyPoint2World), i);

		// Set values of the forward projections at "Shot{i}/keyPoint_kp{0,1}"
		keyPointsConfig.setParam("Shot" + std::to_string(i) + "/keyPoint_kp0", vec3(kp0ForwardProjectioni[0], kp0ForwardProjectioni[1], 0.0));
		keyPointsConfig.setParam("Shot" + std::to_string(i) + "/keyPoint_kp1", vec3(kp1ForwardProjectioni[0], kp1ForwardProjectioni[1], 0.0));
		keyPointsConfig.setParam("Shot" + std::to_string(i) + "/keyPoint_kp2", vec3(kp2ForwardProjectioni[0], kp2ForwardProjectioni[1], 0.0));
	}

	//We add a bit of "noise" below so that the initialization is not exact
	keyPointsConfig.setParam("Volume/keyPoint_kp0", (keyPoint0World + vec3(0.0, 1.0, 1.0)).eval());
	keyPointsConfig.setParam("Volume/keyPoint_kp1", (keyPoint1World+ vec3(-1.0, 0.0, 0.0)).eval());
	keyPointsConfig.setParam("Volume/keyPoint_kp2", (keyPoint2World + vec3(0.0, 1.0, 1.0)).eval());

	// This configures m_kpAlg.
	m_kpAlg->configure(&keyPointsConfig);

	// We now run the bundle adjustment. On failure, this returns Utils::Optional<InitializationResult>()
	Utils::Optional<InitializationResult> result = m_kpAlg->initialize(geom, shots, volume, maskAlgorithm);

	// We can also modify the values of the masks here. The below sets masks that do not
	// crop away anything.
	if (result)
	{
		for (int i = 0; i < shots.size(); i++)
		{
			// Set masks on shot i
			shots.setFocus(i);
			Properties p;
			maskAlgorithm->configuration(&p);
			// Minimum and maximum are normalized to [0,1]
			p.setParam("min", vec3f(0.0, 0.0, -1.0));
			p.setParam("max", vec3f(1.0, 1.0, 1.0));
			maskAlgorithm->configure(&p);
		}
	}
	return result;
}
