#include "DemoExecutable.h"

#include <ImFusion/Base/DataModel.h>
#include <ImFusion/Dicom/DicomLoader.h>
#include <ImFusion/GL/SharedImageSet.h>
#include <ImFusion/GUI/GlQtContext.h>
#include <ImFusion/GUI/InteractiveView.h>
#include <ImFusion/GUI/ViewGroup.h>

#include <QApplication>


// main entry point
int main(int argn, char** argv)
{
	QApplication app(argn, argv);
	ImFusion::DemoExecutable ex;
	ex.show();
	app.exec();
}


// DemoExecutable implementation
namespace ImFusion
{
	DemoExecutable::DemoExecutable()
		// Construct the ApplicationController with a Qt OpenGL context so that we can use a DisplayWidget later on.
		: ApplicationController(std::make_unique<ImFusion::DataModel>(), std::make_unique<ImFusion::GlQtContext>())
	{
		// Load ImFusion plugins. Adjust path to your local machine if needed!
#ifdef _DEBUG
		loadPlugins({"C:/Program Files/ImFusion/ImFusion Suite/SuiteDev/plugins"});
#else
		loadPlugins({"C:/Program Files/ImFusion/ImFusion Suite/Suite/plugins"});
#endif

		// create a new DisplayWidget and assign it to the QMainWindow
		m_disp = std::make_unique<DisplayWidgetMulti>();
		auto dispWrapper = QWidget::createWindowContainer(m_disp.get(), this);
		this->setCentralWidget(dispWrapper);

		// add default MPR and 3D views and make them visible
		auto viewGroup = m_disp->addViewGroup3D(false);
		for (auto v : m_disp->views())
			v->setVisible(true);

		// Use the DicomLoader to load DICOM data from the disk
		DicomLoader dicomLoader("C:/path/to/your/DICOM/data");
		auto images = dicomLoader.loadImages();

		// Handle loaded images:
		// - Move them to the ApplicationController's DataModel (transfer ownership)
		// - Add them to the views of the DisplayWidget
		DataList dl;
		for (auto& sis : images)
		{
			dl.add(sis.get());
			dataModel()->add(sis.release());
		}
		viewGroup->setVisibleData(dl);
	}


	DemoExecutable::~DemoExecutable()
	{
		// clean up before the ImFusion SDK is deinitalized
		for (auto v : m_disp->views())
			v->setVisibleData({});
	}
}
