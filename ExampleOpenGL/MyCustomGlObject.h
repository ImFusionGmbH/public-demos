/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once
 
#include <ImFusion/GL/GlObject.h>
 
class MyCustomGlObject : public ImFusion::GlObject
{
public:
	// This function implements the actual rendering code
	void draw(const ImFusion::GlView& view) override;
 
	// This function returns the world extent of the rendered object so that the
	// view architecture can automatically determine proper scaling etc.
	// For now we return an default (empty) extent.
	ImFusion::Geometry::AlignedBox bounds() const override
	{
		return {};
	}
 
	// Return a string ID of this class that can be used for serialization.
	std::string typeName() const override
	{
		return "MyCustomGlObject";
	}
};