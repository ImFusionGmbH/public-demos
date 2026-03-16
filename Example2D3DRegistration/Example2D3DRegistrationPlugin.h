/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/ImFusionLibPlugin.h>

namespace ImFusion
{
	// See also the ExamplePlugin for further documentation
	class Example2D3DRegistrationPlugin : public ImFusionLibPlugin
	{
	public:
		Example2D3DRegistrationPlugin();
		virtual ~Example2D3DRegistrationPlugin();

		static const char* id() { return "Demo.Example2D3DRegistration"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		Status init() override;
	};
}
