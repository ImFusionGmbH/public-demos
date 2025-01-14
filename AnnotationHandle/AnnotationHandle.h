/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/Core/GL/VertexBuffer.h>
#include <ImFusion/GL/GlObject.h>
#include <ImFusion/GUI/InteractiveObject.h>
#include <ImFusion/GUI/QtHelpers.h>

namespace ImFusion
{
	class GlPointBasedAnnotation;

	/// Implements the rendering of the overlay buttons
	class GlAnnotationHandle : public GlObject
	{
	public:
		/// Pass the GlPointBasedAnnotation for which the buttons should be rendered
		explicit GlAnnotationHandle(GlPointBasedAnnotation* pba);
		/// The actual rendering takes place here. The method is called during every draw call.
		void draw(const GlView& view) override;
		/// We return an empty (default-constructed) Bounds object because the GlOverlayHandle does not reside in world space.
		Bounds bounds() const override { return {}; }
		/// Return unique identifier for this object class used for serialization
		std::string typeName() const override { return "GlOverlayHandle"; }

	private:
		/// Cache the VertexBuffer to avoid expensive recreation for every draw call.
		GL::VertexBuffer m_vbo;
		/// The associated GlPointBasedAnnotation.
		GlPointBasedAnnotation* m_pba;
	};

	/// Implements the mouse interaction with the buttons
	class InteractiveAnnotationHandle : public InteractiveObject
	{
	public:
		/// Pass the GlOverlayHandle whose buttons' interactions shall be handled and the reference to the step size maintained by the controller.
		explicit InteractiveAnnotationHandle(GlPointBasedAnnotation* pba, const int& stepSize);

		/// The actual event handling takes place here. Called by the hosting InteractiveView to notify on user input events.
		bool sceneEvent(QEvent* event, InteractiveView* view) override;

	private:
		/// The associated GlPointBasedAnnotation.
		GlPointBasedAnnotation* m_pba;
		/// A reference to the step size maintained by the controller.
		const int& m_stepSize;
	};
}