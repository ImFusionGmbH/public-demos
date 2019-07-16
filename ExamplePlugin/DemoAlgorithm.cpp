#include "DemoAlgorithm.h"

#include <ImFusion/Base/DataList.h>
#include <ImFusion/Base/MemImage.h>
#include <ImFusion/Base/SharedImage.h>
#include <ImFusion/Base/SharedImageSet.h>


namespace ImFusion
{
	DemoAlgorithm::DemoAlgorithm(SharedImageSet* img)
		: m_imgIn(img)
		, m_imgOut(0)
	{
		configureDefaults();
	}


	bool DemoAlgorithm::createCompatible(const DataList& data, Algorithm** a)
	{
		// check requirements to create the algorithm
		if (data.size() != 1)
			return false;
		SharedImageSet* img = data.getImage(Data::UNKNOWN);    // in our case, any image is fine
		if (img == 0)
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
		m_imgOut = new SharedImageSet();
		for (int i = 0; i < m_imgIn->size(); i++)
		{
			// clone raw memory image
			// (we could also clone m_imgIn directly, but this is for demo purposes)
			SharedImage* imgNew = new SharedImage(*m_imgIn->mem(i)->clone());

			// make sure the clone also shares the same modality
			imgNew->setModality(m_imgIn->get(i)->modality());

			// carry the image transformation along
			if (m_imgIn->get(i)->matrixPointer())
				imgNew->setMatrix(m_imgIn->get(i)->matrix());

			// actual downsampling
			imgNew->mem()->downsample(m_factor, m_factor, m_factor);

			// compute never returns data directly - instead,
			// the output method needs to be called, which
			// fills a list of output data - see below
			m_imgOut->add(imgNew);
			m_imgOut->get(i)->setDirtyMem();
		}
	}


	void DemoAlgorithm::output(DataList& dataOut)
	{
		// if we have produced some output, add it to the list
		// attention: membership is hereby transferred to the one
		// calling output()
		if (m_imgOut)
			dataOut.add(m_imgOut);

		// remove our local reference, it is not owned
		// by this algorithm anymore!
		m_imgOut = 0;
	}


	void DemoAlgorithm::configure(const Properties* p)
	{
		// this method restores our members when a workspace file is loaded
		if (p == 0)
			return;
		p->param("factor", m_factor);
		for (int i = 0; i < (int)m_listeners.size(); ++i)
			m_listeners[i]->algorithmParametersChanged();
	}


	void DemoAlgorithm::configuration(Properties* p) const
	{
		// this method is necessary to store our settings in a workspace file
		if (p == 0)
			return;
		p->setParam("factor", m_factor, 2);
	}
}
