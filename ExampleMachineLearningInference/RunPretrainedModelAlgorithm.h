/* Copyright (c) 2012-2021 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/Algorithm.h>
#include <ImFusion/Core/Parameter.h>

#include <memory>

namespace ImFusion
{
	class SharedImageSet;

	/// Simple demo of a custom algorithm using a pre-trained segmentation model on Imagenet.
	/// The input is expected to have values in [0;255], the algorithm will take care of pre-processing the image.
	class RunPretrainedModelAlgorithm : public Algorithm
	{
	public:
		/// Creates the algorithm instance with an image
		RunPretrainedModelAlgorithm(const SharedImageSet* img);

		/// \name	Methods implementing the algorithm interface
		//\{
		/// Factory method to check for applicability or to create the algorithm
		static bool createCompatible(const DataList& data, Algorithm** a = 0);

		/// Applies the processing
		void compute() override;

		/// If new data was created, make it available here
		OwningDataList takeOutput() override;
		//\}

		// Path to the model
		Parameter<std::string>	modelPath = { "modelPath" /* string ID of the parameter*/ , "" /* default value*/, *this /* automatically register this parameter to the class */ };

	private:
		const SharedImageSet* m_imgIn = nullptr;           ///< Input image to process
		std::unique_ptr<SharedImageSet> m_imgOut;    ///< Result of the neural network
	};
}
