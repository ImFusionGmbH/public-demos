#include "RunPretrainedModelAlgorithm.h"

#include <ImFusion/Base/DataList.h>
#include <ImFusion/Base/Log.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/ImageMath/SharedImageSetArithmetic.h>
#include <ImFusion/ML/PixelwiseLearningModel.h>
#include <ImFusion/ML/Operations.h>

namespace IM = ImFusion::ImageMath;

namespace ImFusion
{
	RunPretrainedModelAlgorithm::RunPretrainedModelAlgorithm(SharedImageSet* img)
		: m_imgIn(img)
	{
		// we define the parameter as Path so that the controller includes a button to open an OpenDialog
		modelPath.setType(Properties::ParamType::Path);
		modelPath.setLabel("Model Path");
		modelPath.setAttribute("filter", "YAML files (*.yaml)");
	}


	bool RunPretrainedModelAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		// check requirements to create the algorithm
		if (data.size() != 1)
			return false;

		// accept images from all modalities
		SharedImageSet* inputImg = data.getImage(Data::UNKNOWN);
		if (inputImg == nullptr)
			return false;

		// only accept 2D RGB images
		if (inputImg->img()->dimension() != 2 || inputImg->img()->channels() != 3)
			return false;

		// requirements are met, create the algorithm if asked
		if (a)
		{
			*a = new RunPretrainedModelAlgorithm(inputImg);
		}
		return true;
	}


	void RunPretrainedModelAlgorithm::compute()
	{
		// set generic error status until we have finished
		m_status = Status::Error;

		// create the machine learning model
		ML::PixelwiseLearningModel model(modelPath);

		if (!model.isInitialized())
		{
			LOG_ERROR("RunPretrainedModelAlgorithm", "Model could not be initialized");
			return;
		}

		// run the model and retrieve its output	
		m_imgOut = model.predict(m_imgIn);

		// set algorithm status to success
		if (m_imgOut)
			m_status = Status::Success;
	}


	OwningDataList RunPretrainedModelAlgorithm::takeOutput()
	{
		// if we have produced some output, add it to the list
		return OwningDataList(std::move(m_imgOut));
	}
}
