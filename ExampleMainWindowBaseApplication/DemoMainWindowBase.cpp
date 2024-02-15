#include "DemoMainWindowBase.h"

#include <ImFusion/Base/BasicImageProcessing.h>
#include <ImFusion/Base/DataModel.h>
#include <ImFusion/Core/Platform.h>
#include <ImFusion/Dicom/DicomLoader.h>
#include <ImFusion/GL/SharedImageSet.h>
#include <ImFusion/GUI/GlContextQt.h>
#include <ImFusion/GUI/InteractiveView.h>
#include <ImFusion/GUI/ViewGroup.h>

#include <QApplication>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

// main entry point
int main(int argn, char** argv)
{
	QApplication app(argn, argv);
	ImFusion::DemoMainWindowBase ex;
	ex.show();
	app.exec();
}


// MainWindowBaseApplication implementation
namespace ImFusion
{

	DemoMainWindowBase::DemoMainWindowBase()
		// Construct the ApplicationController with a Qt OpenGL context so that we can use a DisplayWidget later on.
		: MainWindowBase(
			  []() {
				  ImFusion::Framework::InitConfig initConfig;
				  initConfig.glContext = std::make_unique<ImFusion::GlContextQt>();
				  return initConfig;
			  }(),
			  "ImFusion GmbH",
			  "DemoMainWindowBase")
	{
		// Load ImFusion plugins. Adjust path to your local machine if needed!
#ifdef _WIN32
#	ifdef _DEBUG
		loadPlugins({"C:/Program Files/ImFusion/ImFusion Suite/SuiteDev/plugins"});
#	else
		loadPlugins({"C:/Program Files/ImFusion/ImFusion Suite/Suite/plugins"});
#	endif
#else
		loadPlugins({Platform::libraryPath("ImFusionLib").parentPath() / "../lib/ImFusionLib/plugins"});
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

		this->setMinimumSize(800, 600);
		setupGUI();

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

		readSettings();
		setupWidgets();

		// Use the DicomLoader to load DICOM data from the disk
		DicomLoader dicomLoader("C:/path/to/your/DICOM/data");
		std::vector<std::unique_ptr<SharedImageSet>> images = dicomLoader.loadImages();
		if (images.size() == 0)
			return;

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

		// Add an algorithm to algorithmDock
		auto demoAlg = std::make_unique<BasicImageProcessing>(dl.getImage(Data::UNKNOWN));
		this->addAlgorithm((std::move(demoAlg)));

		// align controller widgets
		auto spacer = new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
		m_algorithmDock->addSpacerItem(spacer);
	}


	DemoMainWindowBase::~DemoMainWindowBase()
	{
		// clean up before the ImFusion SDK is deinitalized
		for (auto v : m_display->views())
			v->setVisibleData({});
	}

	void DemoMainWindowBase::setupGUI()
	{
		// Setup display widgets and layout
		// 2D/3D view
		auto dispWrapper = QWidget::createWindowContainer(m_display.get(), this);
		dispWrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		dispWrapper->setMinimumSize(QSize(64, 64));
		m_display->setContainerWidget(dispWrapper);

		// main display widget
		auto centralwidget = new QWidget(this);
		centralwidget->setObjectName(QString::fromUtf8("centralWidget"));
		centralwidget->setProperty("lightStyle", QVariant(true));
		this->setCentralWidget(centralwidget);

		// layout of main display widget
		auto gridLayout = new QGridLayout(centralwidget);
		gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
		gridLayout->setHorizontalSpacing(3);
		gridLayout->setVerticalSpacing(1);
		gridLayout->setContentsMargins(4, 4, 4, 4);

		// layout for 2D/3D view
		m_verticalLayout = new QVBoxLayout();
		m_verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
		gridLayout->addLayout(m_verticalLayout, 0, 3, -1, 1);
		gridLayout->addWidget(dispWrapper, 0, 4, 1, 1);

		// DataList and algorithm dock layout
		auto verticalLayout = new QVBoxLayout();
		verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
		verticalLayout->setContentsMargins(0, 0, 0, 0);
		gridLayout->addLayout(verticalLayout, 0, 0, 2, 2);

		// parent widget containing algorithm controllers.
		auto scrollAreaWidget = new QWidget();
		scrollAreaWidget->setObjectName(QString::fromUtf8("scrollAreaWidget"));
		scrollAreaWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		scrollAreaWidget->setProperty("lightStyle", QVariant(true));

		// algorithm controllers layout
		m_algorithmDock = new QVBoxLayout();
		m_algorithmDock->setSpacing(3);
		m_algorithmDock->setObjectName(QString::fromUtf8("algorithmDock"));
		m_algorithmDock->setContentsMargins(0, 0, 0, 0);
		scrollAreaWidget->setLayout(m_algorithmDock);


		// scroll area for algorithm dock
		auto scrollArea = new QScrollArea();
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea->setWidgetResizable(true);
		scrollArea->setWidget(scrollAreaWidget);
		verticalLayout->insertWidget(0, scrollArea, 1);
	}
}
