#include "DemoExecutable.h"

#include <ImFusion/Core/Platform.h>
#include <ImFusion/Base/DataModel.h>
#include <ImFusion/Dicom/DicomLoader.h>
#include <ImFusion/GL/SharedImageSet.h>
#include <ImFusion/Core/GL/ContextManager.h>
#include <ImFusion/GUI/GlContextQt.h>
#include <ImFusion/GUI/InteractiveView.h>
#include <ImFusion/GUI/ViewGroup.h>

#include <QApplication>


// main entry point
int main(int argn, char** argv)
{
	QApplication app(argn, argv);
	ImFusion::DemoExecutable ex;
	ex.show();
	QApplication::exec();
}


// DemoExecutable implementation
namespace ImFusion
{
	DemoExecutable::DemoExecutable()
		// Construct the ApplicationController with a Qt OpenGL context so that we can use a DisplayWidget later on.
		: ApplicationController(std::make_unique<ImFusion::DataModel>(),
								[]()
								{
									ImFusion::Framework::InitConfig initConfig;
									initConfig.glContext = std::make_unique<ImFusion::GlContextQt>();
									return initConfig;
								}())
	{
		// Load ImFusion plugins. Adjust path to your local machine if needed!
#ifdef _WIN32
		loadPlugins({Platform::libraryPath("ImFusionLib").parentPath() / "plugins"});
#else
		loadPlugins({Platform::libraryPath("ImFusionLib").parentPath() / "ImFusionLib/plugins"});
#endif

		// create a new DisplayWidget and assign it to the QMainWindow
		// pass `false` to not initialize the DisplayWidget yet (we do this explicitly below)
		m_display = std::make_unique<DisplayWidgetMulti>(false);

		// Qt 5.9 changed some internals on how it creates a QWindow.
		// Older versions need to initialize the DisplayWidget *before* wrapping it in a QWidget,
		// otherwise there will be "CreateWindowEx failed (Cannot create a top-level child window)" errors.
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
		m_display->init();
#endif

		auto dispWrapper = QWidget::createWindowContainer(m_display.get(), this);
		this->setCentralWidget(dispWrapper);
		this->setMinimumSize(800, 600);

		// Qt 5.9 changed some internals on how it creates a QWindow.
		// Newer versions need to initialize the DisplayWidget *after* wrapping it in a QWidget,
		// otherwise there will be weird offsets in the rendering and in the mouse event positions.
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
		m_display->init();
#endif

		// add 2D view as well as default MPR and 3D view group and make them visible
		m_display->addView2D(false);
		m_display->addViewGroup3D(false);
		for (auto v : m_display->views())
			v->setVisible(true);

		// Use the DicomLoader to load DICOM data from the disk
		DicomLoader dicomLoader("C:/path/to/your/DICOM/data");
		std::vector<std::unique_ptr<SharedImageSet>> images = dicomLoader.loadImages();

		// Handle loaded images:
		// - Move them to the ApplicationController's DataModel (transfer ownership)
		// - Add them to the views of the DisplayWidget
		DataList dl;
		for (auto& sis : images)
		{
			dl.add(sis.get());
			dataModel()->add(std::move(sis));
		}

		// show the data, DisplayWidget takes care of distributing them to the compatible views
		m_display->setVisibleData(dl);
	}


	DemoExecutable::~DemoExecutable()
	{
		// some Qt versions may release the OpenGL context early, so we have to re-acquire it here
		GL::ContextManager::makeCurrent();
		// clean up before the ImFusion SDK is deinitalized
		for (auto v : m_display->views())
			v->setVisibleData({});
	}
}
