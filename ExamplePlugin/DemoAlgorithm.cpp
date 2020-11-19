#include "DemoAlgorithm.h"

#include <ImFusion/Base/DataList.h>
#include <ImFusion/Base/ImageProcessing.h>
#include <ImFusion/Base/MemImage.h>
#include <ImFusion/Base/SharedImage.h>
#include <ImFusion/Base/SharedImageSet.h>


namespace ImFusion
{
	DemoAlgorithm::DemoAlgorithm(SharedImageSet* img)
		: m_imgIn(img)
	{
	}


	bool DemoAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		// check requirements to create the algorithm
		if (data.size() != 1)
			return false;
		SharedImageSet* img = data.getImage(Data::UNKNOWN);    // in our case, any image is fine
		if (img == nullptr)
			return false;

		// requirements are met, create the algorithm if asked
		if (a)
		{
			*a = new DemoAlgorithm(img);
		}
		return true;
	}


	void DemoAlgorithm::compute()
	{
		// set generic error status until we have finished
		m_status = static_cast<int>(Status::Error);

		m_imgOut = std::make_unique<SharedImageSet>();
		for (int i = 0; i < m_imgIn->size(); i++)
		{
			// Use ImageProcessing functions to perform the downsampling
			std::unique_ptr<MemImage> newMem = ImageProcessing::createDownsampled(*m_imgIn->mem(i), m_factor, m_factor);

			// create a SharedImage to hold newMem and copy over modality and transformation matrix
			// NOTE: if we had cloned m_imgIn initially, we would not need to to these steps
			auto sharedImage = std::make_unique<SharedImage>(std::move(newMem));
			sharedImage->setModality(m_imgIn->get(i)->modality());
			sharedImage->setMatrix(m_imgIn->get(i)->matrix());

			// compute never returns data directly - instead,
			// the output method needs to be called, which
			// fills a list of output data - see below
			m_imgOut->add(std::move(sharedImage));
		}

		// set algorithm status to success
		m_status = static_cast<int>(Status::Success);
	}


	OwningDataList DemoAlgorithm::takeOutput()
	{
		// if we have produced some output, add it to the list
		return OwningDataList(std::move(m_imgOut));
	}


	void DemoAlgorithm::configure(const Properties* p)
	{
		// this method restores our members when a workspace file is loaded
		if (p == nullptr)
			return;

		p->param("factor", m_factor);
		signalParametersChanged.emitSignal();
	}


	void DemoAlgorithm::configuration(Properties* p) const
	{
		// this method is necessary to store our settings in a workspace file
		if (p == nullptr)
			return;

		p->setParam("factor", m_factor, 2);
	}
}
