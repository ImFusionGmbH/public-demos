#pragma once

#include <ImFusion/GUI/DisplayWidgetMulti.h>
#include <ImFusion/GUI/MainWindowBase.h>

#include <memory>


namespace ImFusion
{
	// Minimalistic demo application to show a DICOM data set.
	// We inherit from ImFusion::MainWindowBase to get a GUI setup resembling a standard radiology
	// workstation out-of-the-box. This will contain GUI elements such as the DataWidget and SelectionWidget,
	// as well as scroll area where on-demand UI elements (e.g. ImFusion::AlgorithmControllers) can be hosted.
	//
	// The appearance will be very similar to the one of the ImFusion Suite. In case you want more control over
	// the GUI layout and build everything from scratch, you can inherit from the ImFusion::ApplicationController
	// base interface instead.
	class DemoMainWindowBase : public MainWindowBase
	{
	public:
		DemoMainWindowBase();
		virtual ~DemoMainWindowBase();

		// implement layout setting to display algorithm controllers in the algorithm dock
		// for example: one can set m_algorihtmDock layout for a container widget
		// the widget can later host algorithm controllers in a scroll area
		QBoxLayout* algorithmDock() const override { return m_algorithmDock; };

		// implement layout setting to display 2D/3D views of image data
		// and offer a srollbar if there are multiple 2D image frames in display
		QBoxLayout* scrollBarLayout() const override { return m_verticalLayout; };

		// display image data in the view and take care of arranging the views as well as user interaction
		DisplayWidgetMulti* display() const override { return m_display.get(); };

	private:
		void setupGUI();
		std::unique_ptr<DisplayWidgetMulti> m_display;    ///< Widget for displaying the image data
		QBoxLayout* m_algorithmDock;
		QBoxLayout* m_verticalLayout;
	};
}
