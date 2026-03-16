/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Base/ImFusionLibPlugin.h>

namespace ImFusion
{
	/// Minimal example for defining a custom plugin for the ImFusion SDK
	class TotalSegmentatorDemoPlugin : public ImFusionLibPlugin
	{
	public:
		TotalSegmentatorDemoPlugin();
		~TotalSegmentatorDemoPlugin() override;

		static const char* id() { return "TotalSegmentatorDemo"; }

		std::string author() const override { return ""; }

		std::string description() const override { return ""; }

		Status init() override;
	};
}
