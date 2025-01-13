#include "AnnotationHandleAlgorithm.h"

#include <ImFusion/Base/DataList.h>

namespace ImFusion
{

	AnnotationHandleAlgorithm::AnnotationHandleAlgorithm()
	{
		p_stepSize.setLabel("Step Size");
		p_stepSize.setRange(1, 20);
		p_stepSize.setAttribute("step", "1");
		p_stepSize.setAttribute("withSlider", "true");
	}

	bool AnnotationHandleAlgorithm::createCompatible(const DataList& /*data*/, Algorithm** a)
	{
		if (a)
		{
			*a = new AnnotationHandleAlgorithm();
		}
		return true;
	}
}