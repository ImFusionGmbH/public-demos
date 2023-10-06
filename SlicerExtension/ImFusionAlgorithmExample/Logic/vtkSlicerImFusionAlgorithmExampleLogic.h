// .NAME vtkSlicerImFusionAlgorithmExampleLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerImFusionAlgorithmExampleLogic_h
#define __vtkSlicerImFusionAlgorithmExampleLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include "vtkSlicerImFusionAlgorithmExampleModuleLogicExport.h"

#include <cstdlib>
#include <memory>
#include <utility>

class vtkMRMLVolumeNode;
class vtkMRMLScalarVolumeNode;
class vtkMRMLLinearTransformNode;

namespace ImFusion
{
	class SharedImageSet;
	class Progress;
}

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_IMFUSIONALGORITHMEXAMPLE_MODULE_LOGIC_EXPORT vtkSlicerImFusionAlgorithmExampleLogic : public vtkSlicerModuleLogic
{
public:
	enum class SimilarityMeasure
	{
		SAD = 0,
		SSD = 1,
		NCC = 2,
		LNCC = 3,
		LC2 = 4,
		MI = 5
	};

	enum class Device
	{
		GPU = 0,
		CPU = 1
	};

	static vtkSlicerImFusionAlgorithmExampleLogic* New();
	vtkTypeMacro(vtkSlicerImFusionAlgorithmExampleLogic, vtkSlicerModuleLogic);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	// Registers the two volumes to one another with a linear registration using the ImFusionLib algorithm
	// Similarity measure and Device (CPU or GPU) can be set.
	void registerVolumes(vtkMRMLVolumeNode* fixVolume,
						 vtkMRMLVolumeNode* moveVolume,
						 SimilarityMeasure sm = SimilarityMeasure::LC2,
						 Device device = Device::GPU);


	// Adds a vtkMRMLDisplayNode to the scene - default display properties are added
	void addDisplayNodeForVolume(vtkMRMLVolumeNode* volumeNode);

protected:
	// Constructor - initializes ImFusion Framework
	vtkSlicerImFusionAlgorithmExampleLogic();

	// Destructor - deinitializes ImFusion Framework
	~vtkSlicerImFusionAlgorithmExampleLogic() override;

	void SetMRMLSceneInternal(vtkMRMLScene* newScene) override;
	void ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData) override;
	/// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
	void RegisterNodes() override;
	void UpdateFromMRMLScene() override;

	// Transforms a pair of volumes to a pair of shared image sets
	std::pair<std::unique_ptr<ImFusion::SharedImageSet>, std::unique_ptr<ImFusion::SharedImageSet>>
		transformToSharedImageSets(vtkMRMLVolumeNode* fixVolume, vtkMRMLVolumeNode* moveVolume, ImFusion::Progress* progress = nullptr);

	// Adds the volumes with the given IDs to the 2-D views with the given blending (0-1)
	void addNodesTo2DViews(const std::string& foreVolumeID, const std::string& backVolumeID, double blending = 1.0);

	// Copies the display properties from one node to the other
	void copyVolumeDisplayNodeProperties(vtkMRMLVolumeNode& inputVolume, vtkSmartPointer<vtkMRMLScalarVolumeNode> newVolNode);

	// Returns the transformation from the volume to the world center
	vtkMRMLLinearTransformNode* GetTransformationToWorldCenter(vtkMRMLVolumeNode* fixVolume);

private:
	vtkSlicerImFusionAlgorithmExampleLogic(const vtkSlicerImFusionAlgorithmExampleLogic&);    // Not implemented
	void operator=(const vtkSlicerImFusionAlgorithmExampleLogic&);                            // Not implemented

	vtkMRMLVolumeNode* m_inputNode;
};

#endif
