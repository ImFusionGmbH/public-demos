#include "DemoOpenGLPlugin.h"

#include "DemoOpenGLFactory.h"

#include <ImFusion/Core/Resource/FilesystemRepository.h>
#include <ImFusion/Core/Resource/Repository.h>

IMFUSION_REGISTER_PLUGIN(ImFusion::DemoOpenGLPlugin)


namespace ImFusion
{
	DemoOpenGLPlugin::DemoOpenGLPlugin() = default;


	DemoOpenGLPlugin::~DemoOpenGLPlugin() = default;

	PluginBase::Status DemoOpenGLPlugin::init()
	{
		// We will use a FilesystemRepository for simplicity. We also offer other repositories e.g. to compile resources into a binary file.
		m_shaderRepo.reset(new Resource::FilesystemRepository("OpenGLDemoRepository", {Filesystem::Path(__FILE__).parentDirectory()}));
		Resource::addRepository(m_shaderRepo.get());

		registerFactories(std::make_unique<DemoOpenGLAlgorithmFactory>(), std::make_unique<DemoOpenGLControllerFactory>(), nullptr);

		return Status::Success;
	}

}
