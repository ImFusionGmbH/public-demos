#pragma once

#include <ImFusion/GUI/DisplayWidgetMulti.h>
#include <ImFusion/GUI/MainWindowBase.h>

#include <memory>


namespace ImFusion
{
	// Minimalistic demo application to show a DICOM data set.
	// We inherit from ImFusion::ApplicationController to automatically take care of initializing
	// the ImFusion SDK as soon as this class is instantiated.
	class DemoExecutable : public QMainWindow, public ApplicationController
	{
	public:
		DemoExecutable();
		virtual ~DemoExecutable();

	private:
		std::unique_ptr<DisplayWidgetMulti> m_disp;    ///< Widget for displaying the image data
	};
}
