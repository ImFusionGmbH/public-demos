#include "ImFusionClient.h"

#include <ImFusion/Base/AlgorithmFactory.h>
#include <ImFusion/Base/FactoryRegistry.h>
#include <ImFusion/Base/Framework.h>
#include <ImFusion/Base/IoAlgorithm.h>
#include <ImFusion/Core/GL/ContextManager.h>
#include <ImFusion/Core/Log.h>
#include <ImFusion/GUI/GlContextQt.h>
#include <ImFusion/IO/FileLoader.h>

#include <QFileInfo>


using namespace ImFusion;


ImFusionClient& ImFusionClient::instance()
{
	static ImFusionClient singletonInstance;
	return singletonInstance;
}


bool ImFusionClient::loadImage(const QString& filename)
{
	m_images.clear();

	IO::FileLoader loader;
	std::optional<OwningDataList> dl = loader.load(filename.toStdString(), IO::FileLoader::Options::AllowSpawnGUI);
	if (dl && loader.status() == IO::FileLoader::Status::Success)
	{
		m_images = dl->extractAllImages();
	}
	else
	{
		LOG_ERROR("Could not find compatible IO algorithm to load file " << filename.toStdString() << ", error code: " << int(loader.status()));
	}

	return !m_images.empty();
}


void ImFusionClient::qtQuickOpenglContextCreated(QOpenGLContext* context)
{
	Framework::InitConfig initConfig;
	initConfig.glContext = std::make_unique<GlContextQt>(0, 0, false, context);
	Framework::init(std::move(initConfig));

	// Load ImFusion plugins found in the default plugin search paths.
	PluginManager::get().registerPlugins();
	PluginManager::get().initAllRegisteredPlugins();

	// Make sure to get notified *before* the QML context is destroyed, so that we can properly deinitialize ImFusionLib.
	connect(context, &QOpenGLContext::aboutToBeDestroyed, this, &ImFusionClient::qtQuickOpenglContextAboutToBeDestroyed);
}


void ImFusionClient::qtQuickOpenglContextAboutToBeDestroyed()
{
	// Delete everything that has references to OpenGL.
	GL::ContextManager::makeCurrent();
	m_images.clear();
	Framework::deinit();
}


ImFusionClient::ImFusionClient() = default;
