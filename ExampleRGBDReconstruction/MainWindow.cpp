#include "MainWindow.h"

#include <ImFusion/Core/Log.h>
#include <ImFusion/GUI/QtHelpers.h>
#include <ImFusion/IO/MeshIO.h>
#include <ImFusion/Mesh/Mesh.h>
#include <ImFusion/Stream/StreamData.h>
#include <ImFusion/RGBD/RGBDIoAlgorithm.h>
#include <ImFusion/RGBD/SurfaceReconstructionData.h>
#include <ImFusion/RGBD/RGBDReconstructionAlgorithm.h>

#include <QtWidgets/QAction>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolBar>

using namespace ImFusion;

MainWindow::MainWindow()
	: m_reco(nullptr)
	, m_stream(nullptr)
	, m_imgLabel(new QLabel(this))
{	
	QHBoxLayout* l = new QHBoxLayout;
	l->addWidget(new QLabel());
	l->addWidget(m_imgLabel);

	QWidget* wt = new QWidget;
	wt->setLayout(l);
	setCentralWidget(wt);

   setWindowIcon(QIcon(":/appicon.ico"));

	QToolBar* toolbar = new QToolBar(this);
	addToolBar(toolbar);

	QAction* a;

	a = new QAction("Start reconstruction", this);
	a->setShortcut(QKeySequence("F5"));
	connect(a, SIGNAL(triggered()), this, SLOT(onStartReconstruction()));
	addAction(a);
	toolbar->addAction(a);

	a = new QAction("Stop reconstruction", this);
	a->setShortcut(QKeySequence("F6"));
	connect(a, SIGNAL(triggered()), this, SLOT(onStopReconstruction()));
	addAction(a);
	toolbar->addAction(a);
	
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinMaxButtonsHint);

	// Use RGBDIoAlgorithm to enumerate available RGB-D sensors
	RGBDIoAlgorithm io;
	io.compute();

	// Get first RGBDStream object
	auto rgbdStreams = io.takeOutput().extractAll<RGBDStream>();

	if (rgbdStreams.size())
	{
		LOG_INFO(std::to_string(rgbdStreams.size()) + " RGB-D sensor(s) found");
		// Use first found RGBDStream
		m_stream.reset(rgbdStreams[0].release());
		m_stream->signalStreamData.connect(this, &MainWindow::onStreamData);
	}
	else
	{
		LOG_ERROR("No RGB-D sensors found");
	}
	LOG_INFO("Press F5 to start reconstruction and F6 to stop reconstruction");
}


MainWindow::~MainWindow()
{
	if (m_reco)
		// Stop reconstruction
		m_reco->stop();

	if (m_stream)
	{
		// Stop stream and delete it
		m_stream->stop();
		m_stream->close();
	}
}


void MainWindow::onStartReconstruction()
{
	if (!m_reco)
	{
		if (m_stream)
		{
			// Create reconstruction object
			const std::vector<RGBDStream*> streams{m_stream.get()};
			m_reco = std::make_unique<RGBDReconstructionAlgorithm>(streams);
			m_reco->surfaceReconstructionData()->setRecoMethod(SurfaceReconstructionData::ReconstructionMethod::VolumetricFusion);
			// Start reconstruction with default settings
			bool ok = m_reco->start();
			if (!ok)
			{
				LOG_ERROR("Failed to start reconstruction");
				m_reco.reset();
			}
			else
			{
				LOG_INFO("Reconstruction started");
			}
		}
	}
	else
	{
		LOG_ERROR("Reconstruction is already running");
	}
}


void MainWindow::onStopReconstruction()
{
	if (m_reco)
	{
		// Stop reconstruction
		m_reco->stop();

		// Save mesh to disk
		if (auto meshOut = m_reco->getMesh())
		{
			QString fileName = QtHelpers::getSaveFilename(this, "saveReco", tr("Select output mesh file"), "PLY files (.ply)");
			if (!fileName.isEmpty())
			{
				bool ok = MeshIO::saveToPLY(*meshOut, fileName.toStdString() + ".ply");
				if (ok)
				{
					LOG_INFO("Mesh was saved to " + fileName.toStdString() + ".ply");
				}
				else
				{
					LOG_ERROR("Error saving mesh to " + fileName.toStdString() + ".ply");
				}
			}
			else
			{
				LOG_INFO("No file selected");
			}
		}
		else
		{
			LOG_ERROR("Unable to create a mesh");
		}

		//safely decompose the reconstruction, io algorithm instances
		m_reco.reset();
	}
	else
	{
		LOG_ERROR("Please start reconstruction first");
	}
}


void MainWindow::onStreamData(std::shared_ptr<const StreamData> streamData)
{
	// Cast StreamData to specialized type for RGB-D frames
	if (const RGBDFrame* imageStream = dynamic_cast<const RGBDFrame*>(streamData.get()))
	{
		try
		{
			// Show color image in GUI
			auto image = imageStream->colorImage();
			QImage qtImage(static_cast<const uchar*>(image->data()), image->width(), image->height(), QImage::Format_RGB888);
			m_imgLabel->setPixmap(
				QPixmap::fromImage(qtImage).scaled(image->width() , image->height() , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
			update();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR(e.what());
		}
	}
}
