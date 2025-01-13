/* Copyright (c) 2012-2024 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include "AnnotationHandle.h"
#include "AnnotationHandleAlgorithm.h"

#include <ImFusion/GUI/DefaultAlgorithmController.h>
#include <ImFusion/GUI/Interactive.h>

#include <unordered_map>

namespace ImFusion
{
	/// Responsible for maintaining an InteractiveAnnotationHandle for every GlPointBasedAnnotation.
	class AnnotationHandleController : public DefaultAlgorithmController
	{
		Q_OBJECT;

	public:
		/// Construct the AnnotationHandleController
		explicit AnnotationHandleController(AnnotationHandleAlgorithm* alg);
		/// Remove all InteractiveAnnotationHandles maintained by this controller from the views.
		~AnnotationHandleController() override;

		/// We create an InteractiveAnnotationHandle for every already existing GlPointBasedAnnotation and add them to the views.
		/// Note: We do two-step initialization because only from here DisplayWidgetMulti is available.
		void init() override;

	private:
		/// Creates an InteractiveAnnotationHandle for the GlPointBasedAnnotation passed and adds it to the views.
		void addInteractiveAnnotationHandle(DisplayWidgetMulti& display, GlPointBasedAnnotation& pba, InteractiveObject& interactiveObject);
		/// Removes the InteractiveAnnotationHandle passed from all views.
		void removeInteractiveAnnotationHandle(const DisplayWidgetMulti& display, InteractiveAnnotationHandle& interactiveAnnotationHandle);

		/// Take care of new annotations added.
		void onAnnotationAdded(InteractiveObject* annotation);
		/// Take care of annotations removed.
		void onAnnotationRemoved(InteractiveObject* annotation);
		/// Take care of annotations moved to different parent.
		void onAnnotationMoved(InteractiveObject* annotation);
		/// Reacts to a change of visibility of the annotation's parent data.
		void onVisibleDataChanged(const DataList& before, const DataList& now);

		/// Maps from the InteractiveObject (which contains the GlPointBasedAnnotation) to the InteractiveAnnotationHandle associated with it.
		std::unordered_map<InteractiveObject*, std::unique_ptr<InteractiveAnnotationHandle>> m_interactiveAnnotationHandles;
		/// The step size defining the distance the annotation point is moved upon a click on the handle.
		int m_stepSize;
	};
}
