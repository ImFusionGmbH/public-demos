/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/ImFusionLibPlugin.h>

namespace ImFusion
{
	namespace Resource
	{
		class Repository;
	}
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class DemoOpenGLPlugin : public ImFusionLibPlugin
	{
	public:
		DemoOpenGLPlugin();
		~DemoOpenGLPlugin() override;

		static const char* id() { return "Demo.OpenGL"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		PluginBase::Status init() override;

	private:
		std::unique_ptr<ImFusion::Resource::Repository> m_shaderRepo;
	};
}
