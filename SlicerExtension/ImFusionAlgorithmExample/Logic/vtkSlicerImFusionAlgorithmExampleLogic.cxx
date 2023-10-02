// ImFusionAlgorithmExample Logic includes
#include "vtkSlicerImFusionAlgorithmExampleLogic.h"

// Slicer includes
#include <qSlicerAbstractCoreModule.h>
#include <qSlicerApplication.h>
#include <qSlicerCoreApplication.h>
#include <qSlicerLayoutManager.h>
#include <qSlicerModuleManager.h>

#include <vtkMatrix4x4.h>

#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMRMLAbstractLogic.h>
#include <vtkMRMLAbstractViewNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceLogic.h>
#include <vtkMRMLSubjectHierarchyNode.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLVolumePropertyNode.h>
#include <vtkMRMLVolumeRenderingDisplayNode.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSlicerVolumeRenderingLogic.h>
#include <vtkSlicerVolumesLogic.h>
#include <vtkSmartPointer.h>
#include <vtkTrivialProducer.h>
#include <vtkVolumeProperty.h>

// ImFusion includes
#include <ImFusion/Base/Framework.h>
#include <ImFusion/Base/SharedImage.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Base/TypedImage.h>
#include <ImFusion/Core/Mat.h>
#include <ImFusion/GUI/ProgressDialog.h>
#include <ImFusion/Reg/LinearImageRegistration.h>
#include <ImFusion/Reg/RegistrationInitAlgorithm.h>
#include <ImFusion/Reg/SimilarityMeasureFactory.h>

#include <QDebug>
#include <QMessageBox>

#include <cassert>
#include <optional>
#include <string>
#include <utility>
#include <vector>


vtkStandardNewMacro(vtkSlicerImFusionAlgorithmExampleLogic);

vtkSlicerImFusionAlgorithmExampleLogic::vtkSlicerImFusionAlgorithmExampleLogic()
{
	ImFusion::Framework::InitConfig config{};
	config.loadPlugins = true;
	config.minimumLogLevel = ImFusion::Log::Level::Debug;
	ImFusion::Framework::init(std::move(config));
}

vtkSlicerImFusionAlgorithmExampleLogic::~vtkSlicerImFusionAlgorithmExampleLogic() { ImFusion::Framework::deinit(); }

namespace
{
	// Creates and returns a pixelToImage matrix for ImFusion images
	ImFusion::mat4 pixelToImage(const ImFusion::vec3i& dims, const ImFusion::vec3& spacing)
	{
		ImFusion::mat4 pixelToImageFix;
		// clang-format off
		pixelToImageFix << spacing[0], 0.0, 0.0, ((-0.5 * dims[0]) + 0.5)* spacing[0],
			0.0, spacing[1], 0.0, ((-0.5 * dims[1]) + 0.5)* spacing[1],
			0.0, 0.0, spacing[2], ((-0.5 * dims[2]) + 0.5)* spacing[2],
			0.0, 0.0, 0.0, 1.0;
		// clang-format on

		return pixelToImageFix;
	}

	// Converst a 4x4 vtk matrix to an ImFusion::mat4
	ImFusion::mat4 vtkToImFusionMatrix(const vtkMatrix4x4& vtkMat)
	{
		ImFusion::mat4 imfMat;
		for (auto i = 0; i < imfMat.rows(); ++i)
			for (auto j = 0; j < imfMat.cols(); ++j)
				imfMat(i, j) = vtkMat.GetElement(i, j);

		return imfMat;
	}

	// Converts a 4x4 ImFusion matrix to a vtk matrix
	vtkSmartPointer<vtkMatrix4x4> imFusionToVtkMatrix(const ImFusion::mat4& imFusionMatrix)
	{
		vtkSmartPointer<vtkMatrix4x4> vtkMat = vtkSmartPointer<vtkMatrix4x4>::New();
		for (auto i = 0; i < 4; ++i)
			for (auto j = 0; j < 4; ++j)
				vtkMat->SetElement(i, j, imFusionMatrix(i, j));

		return vtkMat;
	}

	// supported type conversions
	std::unordered_map<int, ImFusion::PixelType> VtkToImFusionTypes{{VTK_DOUBLE, ImFusion::PixelType::Double},
																	{VTK_INT, ImFusion::PixelType::Int},
																	{VTK_FLOAT, ImFusion::PixelType::Float},
																	{VTK_UNSIGNED_CHAR, ImFusion::PixelType::UByte},
																	{VTK_CHAR, ImFusion::PixelType::Byte},
																	{VTK_UNSIGNED_INT, ImFusion::PixelType::UInt},
																	{VTK_SHORT, ImFusion::PixelType::Short},
																	{VTK_UNSIGNED_SHORT, ImFusion::PixelType::UShort}};
	// supported type conversions
	std::unordered_map<ImFusion::PixelType, int> ImFusionToVtkTypes{{ImFusion::PixelType::Double, VTK_DOUBLE},
																	{ImFusion::PixelType::Int, VTK_INT},
																	{ImFusion::PixelType::Float, VTK_FLOAT},
																	{ImFusion::PixelType::UByte, VTK_UNSIGNED_CHAR},
																	{ImFusion::PixelType::Byte, VTK_CHAR},
																	{ImFusion::PixelType::UInt, VTK_UNSIGNED_INT},
																	{ImFusion::PixelType::Short, VTK_SHORT},
																	{ImFusion::PixelType::UShort, VTK_UNSIGNED_SHORT}};

	// Converts image from vtk to ImFusion SharedImage - copies the data, does not set any matrix!
	std::unique_ptr<ImFusion::SharedImage> VtkImageToImFusion(vtkImageData* vtkImage)
	{
		if (!vtkImage)
		{
			assert(false);
			return {};
		}

		int dims[3];
		vtkImage->GetDimensions(dims);
		int extent[6];
		auto channels = vtkImage->GetNumberOfScalarComponents();
		assert(channels != 0);

		const ImFusion::vec3i dimensions{dims[0], dims[1], dims[2]};


		std::unique_ptr<ImFusion::MemImage> memImg = nullptr;

		if (auto imfusionType = VtkToImFusionTypes.find(vtkImage->GetScalarType()); imfusionType != VtkToImFusionTypes.end())
		{
			memImg = ImFusion::MemImage::create(ImFusion::ImageDescriptor(imfusionType->second, dimensions, channels));
		}
		else
		{
			QMessageBox::warning(
				nullptr, "ImFusionConversion", QString("VTK Image type ") + QString::number(vtkImage->GetScalarType()) + " not supported");
			return nullptr;
		}

		// memory layout is the same so just copy values from input image to typed image
		memcpy(memImg->data(), vtkImage->GetScalarPointer(), memImg->byteSize());

		return std::make_unique<ImFusion::SharedImage>(std::move(memImg));
	}

	// Converts image from ImFusion to Vtk type - copies the data, does not set any matrix!
	vtkSmartPointer<vtkImageData> ImFusionToVtkImage(const ImFusion::SharedImage& sharedImg)
	{
		vtkSmartPointer<vtkImageData> newVtkImage = vtkSmartPointer<vtkImageData>::New();
		newVtkImage->Initialize();
		vtkNew<vtkTrivialProducer> tp;
		tp->SetOutput(newVtkImage.GetPointer());
		vtkInformation* outInfo = tp->GetOutputInformation(0);

		if (auto vtkType = ImFusionToVtkTypes.find(sharedImg.descriptor().pixelType); vtkType != ImFusionToVtkTypes.end())
		{
			newVtkImage->SetScalarType(vtkType->second, outInfo);
		}
		else
		{
			QMessageBox::warning(nullptr,
								 "ImFusionConversion",
								 QString("ImFusion Image type ") + QString::number(static_cast<int>(sharedImg.descriptor().pixelType)) +
									 " not supported");
			return nullptr;
		}


		newVtkImage->SetDimensions(sharedImg.dimensions()[0], sharedImg.dimensions()[1], sharedImg.dimensions()[2]);
		newVtkImage->SetNumberOfScalarComponents(sharedImg.channels(), outInfo);
		newVtkImage->AllocateScalars(ImFusionToVtkTypes[sharedImg.descriptor().pixelType], sharedImg.channels());

		// memory layout is the same so just copy values from typed image to vtk image
		memcpy(newVtkImage->GetScalarPointer(), sharedImg.mem()->data(), sharedImg.mem()->byteSize());

		return newVtkImage;
	}
}


void vtkSlicerImFusionAlgorithmExampleLogic::PrintSelf(ostream& os, vtkIndent indent) { this->Superclass::PrintSelf(os, indent); }

void vtkSlicerImFusionAlgorithmExampleLogic::registerVolumes(vtkMRMLVolumeNode* fixVolume,
															 vtkMRMLVolumeNode* moveVolume,
															 SimilarityMeasure sm /*= SimilarityMeasure::LC2*/,
															 Device device)
{
	if (!fixVolume || !moveVolume)
		return;

	ImFusion::ProgressDialog progress{};

	auto [fixSharedImageSet, moveSharedImageSet] = transformToSharedImageSets(fixVolume, moveVolume, &progress);

	if (!fixSharedImageSet || !moveSharedImageSet)
		return;


	// registration initialization algorithm
	ImFusion::RegistrationInitAlgorithm regInit{*fixSharedImageSet, *moveSharedImageSet};
	regInit.setProgress(&progress);
	regInit.compute();

	// Linear, rigid image registration
	auto lir = std::make_unique<ImFusion::LinearImageRegistration>(fixSharedImageSet.get(), moveSharedImageSet.get());
	lir->setProgress(&progress);
	lir->setAffine(false);
	lir->similarityMeasureFactory().setMode(static_cast<ImFusion::SimilarityMeasureFactory::Mode>(sm));
	lir->similarityMeasureFactory().setDevice(device == Device::CPU ? ImFusion::Device_CPU : ImFusion::Device_GL);
	lir->compute();
	if (lir->status() != int(ImFusion::Algorithm::Status::Success))
	{
		qDebug() << "Registration failed";
		QMessageBox infoBox{QMessageBox::Information, "Registration", "Registration failed!"};
		infoBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		infoBox.setWindowFlags(Qt::FramelessWindowHint);
		infoBox.exec();
		return;
	}

	// set the ijkToRasMatrix of the moved volume from the imfusion pixelToWorld matrix
	vtkSmartPointer<vtkMatrix4x4> ijkToRASMoving = imFusionToVtkMatrix(moveSharedImageSet->get()->pixelToWorldMatrix());
	moveVolume->SetIJKToRASMatrix(ijkToRASMoving);

	// add nodes to the volume rendering view
	addDisplayNodeForVolume(fixVolume);
	addDisplayNodeForVolume(moveVolume);

	// center the views
	vtkMRMLLinearTransformNode* centeringTransform = GetTransformationToWorldCenter(fixVolume);
	moveVolume->SetAndObserveTransformNodeID(centeringTransform->GetID());
	fixVolume->SetAndObserveTransformNodeID(centeringTransform->GetID());

	// add the nodes to the 2D slice views
	addNodesTo2DViews(moveVolume->GetID(), fixVolume->GetID());

	{
		QMessageBox infoBox{QMessageBox::Information, "Registration", "Registration successful!"};
		infoBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		infoBox.setWindowFlags(Qt::FramelessWindowHint);
		infoBox.exec();
	}
}

std::pair<std::unique_ptr<ImFusion::SharedImageSet>, std::unique_ptr<ImFusion::SharedImageSet>>
	vtkSlicerImFusionAlgorithmExampleLogic::transformToSharedImageSets(vtkMRMLVolumeNode* fixVolume,
																	   vtkMRMLVolumeNode* moveVolume,
																	   ImFusion::Progress* progress)
{
	std::optional<ImFusion::Progress::Task> task = std::nullopt;
	if (progress)
		task = progress->addTask(2, "Conversion to ImFusion types...");

	auto sharedImageFixed = VtkImageToImFusion(fixVolume->GetImageData());
	auto sharedImageMoving = VtkImageToImFusion(moveVolume->GetImageData());
	if (!sharedImageMoving || !sharedImageFixed)
		return {};

	if (task)
		task->incrementStep();

	// set matrices
	vtkSmartPointer<vtkMatrix4x4> ijkToRasFix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> ijkToRasMove = vtkSmartPointer<vtkMatrix4x4>::New();
	fixVolume->GetIJKToRASMatrix(ijkToRasFix);
	moveVolume->GetIJKToRASMatrix(ijkToRasMove);

	double fixSpacing[3] = {0.0, 0.0, 0.0};
	fixVolume->GetSpacing(fixSpacing);
	double moveSpacing[3] = {0.0, 0.0, 0.0};
	moveVolume->GetSpacing(moveSpacing);

	int fixDimensions[3] = {0, 0, 0};
	fixVolume->GetImageData()->GetDimensions(fixDimensions);
	int moveDimensions[3] = {0, 0, 0};
	moveVolume->GetImageData()->GetDimensions(moveDimensions);

	sharedImageFixed->setSpacing({fixSpacing[0], fixSpacing[1], fixSpacing[2]});
	sharedImageMoving->setSpacing({moveSpacing[0], moveSpacing[1], moveSpacing[2]});

	auto fixSharedImageSet = std::make_unique<ImFusion::SharedImageSet>(std::move(sharedImageFixed));
	auto moveSharedImageSet = std::make_unique<ImFusion::SharedImageSet>(std::move(sharedImageMoving));

	const ImFusion::mat4 imfPixelToWorldFix = vtkToImFusionMatrix(*ijkToRasFix);
	const ImFusion::mat4 imfPixelToWorldMove = vtkToImFusionMatrix(*ijkToRasMove);

	const ImFusion::mat4 pixelToImageFix =
		pixelToImage({fixDimensions[0], fixDimensions[1], fixDimensions[2]}, {fixSpacing[0], fixSpacing[1], fixSpacing[2]});
	const ImFusion::mat4 pixelToImageMove =
		pixelToImage({moveDimensions[0], moveDimensions[1], moveDimensions[2]}, {moveSpacing[0], moveSpacing[1], moveSpacing[2]});

	if (task)
		task->incrementStep();

	fixSharedImageSet->setMatrixFromWorld(pixelToImageFix * imfPixelToWorldFix.inverse());
	moveSharedImageSet->setMatrixFromWorld(pixelToImageMove * imfPixelToWorldMove.inverse());

	return {std::move(fixSharedImageSet), std::move(moveSharedImageSet)};
}

vtkMRMLLinearTransformNode* vtkSlicerImFusionAlgorithmExampleLogic::GetTransformationToWorldCenter(vtkMRMLVolumeNode* fixVolume)
{
	vtkSmartPointer<vtkMatrix4x4> ijkToRASFixed = vtkSmartPointer<vtkMatrix4x4>::New();
	fixVolume->GetIJKToRASMatrix(ijkToRASFixed);
	const auto extent = fixVolume->GetImageData()->GetExtent();
	double centerPositionIJK[4] = {
		double(extent[0] + extent[1]) / 2.0, double(extent[2] + extent[3]) / 2.0, double(extent[4] + extent[5]) / 2.0, 1.0};
	double centerPositionRAS[4] = {0.0, 0.0, 0.0, 1.0};
	ijkToRASFixed->MultiplyPoint(centerPositionIJK, centerPositionRAS);

	fixVolume->SetAndObserveTransformNodeID(nullptr);

	std::string transformName = fixVolume->GetName();
	transformName += "_centering transform";
	vtkMRMLLinearTransformNode* centeringTransform =
		vtkMRMLLinearTransformNode::SafeDownCast(fixVolume->GetScene()->AddNewNodeByClass("vtkMRMLLinearTransformNode", transformName));

	vtkNew<vtkMatrix4x4> centeringTransformMatrix;
	centeringTransformMatrix->SetElement(0, 3, -centerPositionRAS[0]);
	centeringTransformMatrix->SetElement(1, 3, -centerPositionRAS[1]);
	centeringTransformMatrix->SetElement(2, 3, -centerPositionRAS[2]);
	centeringTransform->SetMatrixTransformToParent(centeringTransformMatrix);

	return centeringTransform;
}

void vtkSlicerImFusionAlgorithmExampleLogic::addDisplayNodeForVolume(vtkMRMLVolumeNode* volumeNode)
{
	if (qSlicerAbstractCoreModule* volumeRenderingModule = qSlicerCoreApplication::application()->moduleManager()->module("VolumeRendering"))
	{
		if (vtkSlicerVolumeRenderingLogic* volumeRenderingLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(volumeRenderingModule->logic()))
		{
			vtkSmartPointer<vtkMRMLVolumeRenderingDisplayNode> volumeRenderingDisplayNode =
				vtkSmartPointer<vtkMRMLVolumeRenderingDisplayNode>::Take(volumeRenderingLogic->CreateVolumeRenderingDisplayNode());
			GetMRMLScene()->AddNode(volumeRenderingDisplayNode);
			volumeNode->AddAndObserveDisplayNodeID(volumeRenderingDisplayNode->GetID());
			volumeRenderingLogic->UpdateDisplayNodeFromVolumeNode(volumeRenderingDisplayNode, volumeNode, nullptr, nullptr, false);

			// sets the transfer function etc
			volumeRenderingLogic->SetRecommendedVolumeRenderingProperties(volumeRenderingDisplayNode);
		}
	}
}

void vtkSlicerImFusionAlgorithmExampleLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
	vtkNew<vtkIntArray> events;
	events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
	events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
	events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
	this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

void vtkSlicerImFusionAlgorithmExampleLogic::ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData) {}

void vtkSlicerImFusionAlgorithmExampleLogic::RegisterNodes() {}

void vtkSlicerImFusionAlgorithmExampleLogic::UpdateFromMRMLScene() { assert(this->GetMRMLScene() != 0); }


void vtkSlicerImFusionAlgorithmExampleLogic::addNodesTo2DViews(const std::string& foreVolumeID, const std::string& backVolumeID, double blending)
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	vtkCollection* sliceLogics = layoutManager ? layoutManager->mrmlSliceLogics() : nullptr;
	if (sliceLogics)
	{
		for (int i = 0; i < sliceLogics->GetNumberOfItems(); ++i)
		{
			vtkMRMLSliceLogic* sliceLogic = vtkMRMLSliceLogic::SafeDownCast(sliceLogics->GetItemAsObject(i));
			if (!sliceLogic)
				continue;

			sliceLogic->GetSliceCompositeNode()->SetReferenceBackgroundVolumeID(backVolumeID.c_str());
			sliceLogic->GetSliceCompositeNode()->SetReferenceForegroundVolumeID(foreVolumeID.c_str());
			sliceLogic->GetSliceCompositeNode()->SetForegroundOpacity(blending);
		}
	}

	layoutManager->resetSliceViews();
}

void vtkSlicerImFusionAlgorithmExampleLogic::copyVolumeDisplayNodeProperties(vtkMRMLVolumeNode& inputVolume,
																			 vtkSmartPointer<vtkMRMLScalarVolumeNode> newVolNode)
{
	if (vtkMRMLScalarVolumeDisplayNode* inputScalarVolumeDisplayNode =
			vtkMRMLScalarVolumeDisplayNode::SafeDownCast(inputVolume.GetVolumeDisplayNode()))
	{
		vtkMRMLScalarVolumeDisplayNode* newScalarVolumeDisplayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(newVolNode->GetVolumeDisplayNode());
		int disabledModify = newScalarVolumeDisplayNode->StartModify();
		newScalarVolumeDisplayNode->SetAutoWindowLevel(0);
		newScalarVolumeDisplayNode->SetWindowLevel(inputScalarVolumeDisplayNode->GetWindow(), inputScalarVolumeDisplayNode->GetLevel());
		newScalarVolumeDisplayNode->SetAndObserveColorNodeID(inputScalarVolumeDisplayNode->GetColorNodeID());
		newScalarVolumeDisplayNode->EndModify(disabledModify);
	}
}
