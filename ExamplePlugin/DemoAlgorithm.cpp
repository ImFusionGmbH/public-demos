#include "DemoAlgorithm.h"

#include <ImFusion/Base/DataList.h>
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
			(*a)->setInput(data);
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
			// clone raw memory image
			// (we could also clone m_imgIn directly, but this is for demo purposes)
			std::unique_ptr<MemImage> newMem(m_imgIn->mem(i)->clone());

			// perform the downsampling
			newMem->downsample(m_factor, m_factor, m_factor);

			// create a SharedImage to hold newMem and copy over modality and transformation matrix
			// NOTE: if we had cloned m_imgIn initially, we would not need to to these steps
			auto sharedImage = std::make_unique<SharedImage>(*newMem.release());
			sharedImage->setModality(m_imgIn->get(i)->modality());
			sharedImage->setMatrix(m_imgIn->get(i)->matrix());

			// compute never returns data directly - instead,
			// the output method needs to be called, which
			// fills a list of output data - see below
			m_imgOut->add(sharedImage.release());
		}

		// set algorithm status to success
		m_status = static_cast<int>(Status::Success);
	}


	void DemoAlgorithm::output(DataList& dataOut)
	{
		// if we have produced some output, add it to the list
		// attention: membership is hereby transferred to the one calling output()
		if (m_imgOut)
			dataOut.add(m_imgOut.release());
	}


	void DemoAlgorithm::configure(const Properties* p)
	{
		// this method restores our members when a workspace file is loaded
		if (p == nullptr)
			return;

		p->param("factor", m_factor);
		for (int i = 0; i < (int)m_listeners.size(); ++i)
			m_listeners[i]->algorithmParametersChanged();
	}


	void DemoAlgorithm::configuration(Properties* p) const
	{
		// this method is necessary to store our settings in a workspace file
		if (p == nullptr)
			return;

		p->setParam("factor", m_factor, 2);
	}
}
