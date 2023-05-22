#include "ImFusionClient.h"

#include <ImFusion/Base/FactoryRegistry.h>
#include <ImFusion/Base/Framework.h>
#include <ImFusion/Base/IoAlgorithm.h>
#include <ImFusion/Base/IoAlgorithmFactory.h>
#include <ImFusion/Core/GL/ContextManager.h>
#include <ImFusion/Core/Log.h>
#include <ImFusion/GUI/GlContextQt.h>

#include <QFileInfo>


using namespace ImFusion;

namespace
{
	// Utility function to gather all available IoAlgorithmFactories
	// (each loaded ImFusion plugin can have their own)
	std::vector<const IoAlgorithmFactory*> getIoAlgorithmFactories()
	{
		std::vector<const IoAlgorithmFactory*> factories;
		for (auto f : FactoryRegistry::get().getAlgorithmFactories())
			if (const IoAlgorithmFactory* ioaf = dynamic_cast<const IoAlgorithmFactory*>(f))
				factories.push_back(ioaf);

		return factories;
	}
}


ImFusionClient& ImFusionClient::instance()
{
	static ImFusionClient singletonInstance;
	return singletonInstance;
}


bool ImFusionClient::loadImage(const QString& filename)
{
	m_images.clear();

	QFileInfo fi(filename);
	std::string extension = fi.suffix().toStdString();
	if (extension == "gz")
		extension = fi.completeSuffix().toStdString();

	auto iof = getIoAlgorithmFactories();
	std::string algorithmName;
	for (auto& f : iof)
	{
		algorithmName = f->idFromExtension(extension);
		if (!algorithmName.empty())
			break;
	}

	if (!algorithmName.empty())
	{
		DataList dl;
		std::unique_ptr<IoAlgorithm> alg(static_cast<IoAlgorithm*>(FactoryRegistry::get().createIoAlgorithm(algorithmName, dl)));
		alg->setLocation(filename.toStdString());
		alg->compute();
		m_images = alg->takeOutput().extractAllImages();
	}
	else
	{
		LOG_ERROR("Could not find compatible IO algorithm to load file " << filename.toStdString());
	}

	return !m_images.empty();
}


#define STRINGIFY2(s) #s
#define STRINGIFY(s) STRINGIFY2(s)

void ImFusionClient::qtQuickOpenglContextCreated(QOpenGLContext* context)
{
	Framework::InitConfig initConfig;
	initConfig.glContext = std::make_unique<GlContextQt>(0, 0, false, context);
	Framework::init(std::move(initConfig));

	// Load ImFusion plugins found in the search paths.
	// Note: you might need to adjust this for your platform
#ifdef _DEBUG
	Framework::loadPlugins({STRINGIFY(IMFUSIONLIB_DIR) "/../SuiteDev/plugins"});
#else
	Framework::loadPlugins({STRINGIFY(IMFUSIONLIB_DIR) "/../Suite/plugins"});
#endif

	// Make sure to get notified *before* the QML context is destroyed, so that we can properly deinit the ImFusionLib
	connect(context, &QOpenGLContext::aboutToBeDestroyed, this, &ImFusionClient::qtQuickOpenglContextAboutToBeDestroyed);
}

#undef STRINGIFY2
#undef STRINGIFY


void ImFusionClient::qtQuickOpenglContextAboutToBeDestroyed()
{
	// delete everything that has references to OpenGL.
	GL::ContextManager::makeCurrent();
	m_images.clear();
	Framework::deinit();
}


ImFusionClient::ImFusionClient() = default;
