
#include "Custom2D3DRegistrationInitialization.h"

#include <ImFusion/Base/Log.h>
#include <ImFusion/CT/XRay2D3DRegistrationInitialization.h>
#include <ImFusion/CT/XRay2D3DRegistrationInitializationKeyPoints.h>
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
Utils::Optional<mat4> Custom2D3DRegistrationInitialization::initialize(ConeBeamGeometry& geom,
																	   SharedImageSet& shots,
																	   const SharedImageSet& volume,
																	   MaskEditor* maskAlgorithm)
{
	if (m_kpAlg == nullptr)    // basic consistency check
	{
		LOG_ERROR("Custom initialization not correctly initialized");
		return Utils::Optional<mat4>();
	}

	// We will configure the m_kpAlg member using the Properties interface, described in more
	// detail in Properties.h and the XRay2D3DRegistrationInitializationKeyPoints.h header file.
	Properties keyPointsConfig;

	// The reference locations of three keypoints in world-coordinates are specified
	vec3 keyPoint0World(0.0, 0.0, 0.0);
	vec3 keyPoint1World(0.0, -30.0, 30.0);
	vec3 keyPoint2World(0.0, 30.0, 30.0);
	// We project these reference locations onto the x-ray images.
	for (int i = 0; i < shots.size(); i++)
	{
		//Calculate forward projection onto the detector
		vec2 kp0ForwardProjectioni = m_kpAlg->forwardProjection(keyPoint0World, i);
		vec2 kp1ForwardProjectioni = m_kpAlg->forwardProjection(keyPoint1World, i);
		vec2 kp2ForwardProjectioni = m_kpAlg->forwardProjection(keyPoint2World, i);

		// Set values of the forward projections at "Shot{i}/keyPoint_kp{0,1}"
		keyPointsConfig.setParam("Shot" + std::to_string(i) + "/keyPoint_kp0", vec3(kp0ForwardProjectioni[0], kp0ForwardProjectioni[1], 0.0));
		keyPointsConfig.setParam("Shot" + std::to_string(i) + "/keyPoint_kp1", vec3(kp1ForwardProjectioni[0], kp1ForwardProjectioni[1], 0.0));
		keyPointsConfig.setParam("Shot" + std::to_string(i) + "/keyPoint_kp2", vec3(kp2ForwardProjectioni[0], kp2ForwardProjectioni[1], 0.0));
	}

	// We use the ground truth pose of the volume to calculate locations of the keypoints on the volume
	// for the correct pose. These are calculated in image coordinates of the volume.
	auto toGroundTruth = [this](vec3 inputPoint) -> vec3 { return (m_groundTruthPose.inverse() * inputPoint.homogeneous()).hnormalized().eval(); };
	vec3 keyPoint0Volume = toGroundTruth(keyPoint0World);
	vec3 keyPoint1Volume = toGroundTruth(keyPoint1World);
	vec3 keyPoint2Volume = toGroundTruth(keyPoint2World);

	// The multiplication with volume.matrixToWorld() is needed as the algorithm expects key points in world coordinates.
	// The following lambda function performs this multiplication.
	auto toWorld = [&volume](vec3 inputPoint) -> vec3 { return (volume.matrixToWorld() * inputPoint.homogeneous()).hnormalized().eval(); };

	//We add a bit of "noise" below so that the initialization is not exact
	keyPointsConfig.setParam("Volume/keyPoint_kp0", (toWorld(keyPoint0Volume) + vec3(0.0, 1.0, 1.0)).eval());
	keyPointsConfig.setParam("Volume/keyPoint_kp1", (toWorld(keyPoint1Volume) + vec3(-1.0, 0.0, 0.0)).eval());
	keyPointsConfig.setParam("Volume/keyPoint_kp2", (toWorld(keyPoint2Volume) + vec3(0.0, 1.0, 1.0)).eval());

	// This configures m_kpAlg.
	m_kpAlg->configure(&keyPointsConfig);

	// We now run the bundle adjustment. On failure, this returns Utils::Optional<mat4>()
	Utils::Optional<mat4> result = m_kpAlg->initialize(geom, shots, volume, maskAlgorithm);

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
