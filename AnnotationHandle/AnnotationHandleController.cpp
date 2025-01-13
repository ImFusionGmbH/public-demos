#include "AnnotationHandleController.h"

#include "AnnotationHandle.h"

#include <ImFusion/GL/GlPointBasedAnnotation.h>
#include <ImFusion/GUI/AnnotationModel.h>
#include <ImFusion/GUI/DisplayWidgetMulti.h>
#include <ImFusion/GUI/ImageView2D.h>
#include <ImFusion/GUI/ImageView3D.h>
#include <ImFusion/GUI/MainWindowBase.h>

namespace ImFusion
{
	AnnotationHandleController::AnnotationHandleController(AnnotationHandleAlgorithm* alg)
		: DefaultAlgorithmController(alg, "", true)
	{
		if (alg)
		{
			// Set the initial step size to the value of the parameter and connect to the parameter's signal to react to changes of the step size.
			m_stepSize = alg->p_stepSize;
			alg->p_stepSize.signalValueChanged.connect(this, [this, alg]() { m_stepSize = alg->p_stepSize; });
		}
	}

	AnnotationHandleController::~AnnotationHandleController()
	{
		if (!m_main || !m_main->display())
			return;

		for (const auto& interactiveAnnotationHandle : m_interactiveAnnotationHandles)
		{
			if (interactiveAnnotationHandle.second)
				removeInteractiveAnnotationHandle(*m_main->display(), *interactiveAnnotationHandle.second.get());
		}
	}

	void AnnotationHandleController::addInteractiveAnnotationHandle(DisplayWidgetMulti& display,
																	GlPointBasedAnnotation& pba,
																	InteractiveObject& interactiveObject)
	{
		auto interactiveAnnotationHandlePtr = std::make_unique<InteractiveAnnotationHandle>(&pba, m_stepSize);
		const auto interactiveAnnotationHandleRaw = interactiveAnnotationHandlePtr.get();
		m_interactiveAnnotationHandles.emplace(&interactiveObject, std::move(interactiveAnnotationHandlePtr));
		// Add custom object to all MPR views and to the 3D view
		for (int i = 0; i < display.numberSliceViews(); ++i)
		{
			display.viewSlice(i)->addObject(interactiveAnnotationHandleRaw);
		}
		for (int i = 0; i < display.number3DViews(); ++i)
		{
			display.view3D(i)->addObject(interactiveAnnotationHandleRaw);
		}
	}

	void AnnotationHandleController::removeInteractiveAnnotationHandle(const DisplayWidgetMulti& display,
																	   InteractiveAnnotationHandle& interactiveAnnotationHandle)
	{
		// Remove custom object from all views
		for (const auto& view : display.views())
			view->removeObject(&interactiveAnnotationHandle);
	}

	void AnnotationHandleController::init()
	{
		DefaultAlgorithmController::init();
		// Hide the compute button, it is useless because compute() is empty.
		hideComputeButton(true);

		if (!m_main || !m_main->display() || !m_main->annotationModel())
			return;

		const AnnotationModel* annotationModel = m_main->annotationModel();
		// Listen to signals if annotations are added, removed, moved to different parent or the parent's visibility changed.
		connect(annotationModel, &AnnotationModel::annotationAdded, this, &AnnotationHandleController::onAnnotationAdded);
		connect(annotationModel, &AnnotationModel::annotationAboutToBeRemoved, this, &AnnotationHandleController::onAnnotationRemoved);
		connect(annotationModel, &AnnotationModel::annotationMoved, this, &AnnotationHandleController::onAnnotationMoved);
		connect(m_disp, &DisplayWidgetMulti::visibleDataChanged, this, &AnnotationHandleController::onVisibleDataChanged);

		// Add an interactive object for every already existing annotation
		for (const auto& interactiveObject : annotationModel->getAll())
		{
			if (interactiveObject)
			{
				if (auto* pba = dynamic_cast<GlPointBasedAnnotation*>(interactiveObject->gl()))
				{
					addInteractiveAnnotationHandle(*m_main->display(), *pba, *interactiveObject);
				}
			}
		}
	}

	void AnnotationHandleController::onVisibleDataChanged(const DataList& /*before*/, const DataList& now)
	{
		const AnnotationModel* annotationModel = m_main->annotationModel();

		// Iterate through all handles, get their parent data and check if its visibility changed. If so we change the visibility of our handle analogously.
		for (const auto& [interactiveObject, interactiveAnnotationHandle] : m_interactiveAnnotationHandles)
		{
			if (const auto data = annotationModel->getParentDataset(interactiveObject))
			{
				interactiveAnnotationHandle->gl()->setVisible(now.contains(data));
			}
		}
	}

	void AnnotationHandleController::onAnnotationAdded(InteractiveObject* annotation)
	{
		if (!annotation)
			return;

		if (auto* pba = dynamic_cast<GlPointBasedAnnotation*>(annotation->gl()); pba && m_main && m_main->display())
		{
			addInteractiveAnnotationHandle(*m_main->display(), *pba, *annotation);
		}
	}

	void AnnotationHandleController::onAnnotationRemoved(InteractiveObject* annotation)
	{
		if (!annotation)
			return;

		if (const auto* pba = dynamic_cast<GlPointBasedAnnotation*>(annotation->gl()); pba && m_main && m_main->display())
		{
			if (const auto iter = m_interactiveAnnotationHandles.find(annotation); iter != m_interactiveAnnotationHandles.end() && iter->second)
			{
				removeInteractiveAnnotationHandle(*m_main->display(), *iter->second.get());
				m_interactiveAnnotationHandles.erase(iter);
			}
		}
	}

	void AnnotationHandleController::onAnnotationMoved(InteractiveObject* annotation)
	{
		if (!annotation)
			return;

		if (const auto* pba = dynamic_cast<GlPointBasedAnnotation*>(annotation->gl()); pba && m_main && m_main->display())
		{
			if (const auto iter = m_interactiveAnnotationHandles.find(annotation); iter != m_interactiveAnnotationHandles.end() && iter->second)
			{
				const AnnotationModel* annotationModel = m_main->annotationModel();
				if (const auto data = annotationModel->getParentDataset(annotation))
				{
					// Set the visibility to the same as the new parent.
					iter->second->gl()->setVisible(m_disp->isVisible(data));
				}
				else
				{
					// The annotation is part of the global annotations, so we want to show it by default.
					iter->second->gl()->setVisible(true);
				}
			}
		}
	}
}
