#include "ImFusionFboView.h"

#include "ImFusionClient.h"

#include <ImFusion/Base/Assert.h>
#include <ImFusion/Base/DisplayOptions3d.h>
#include <ImFusion/Base/Log.h>
#include <ImFusion/GL/GlContext.h>
#include <ImFusion/GL/GlFramebuffer.h>
#include <ImFusion/GL/GlSliceView.h>
#include <ImFusion/GL/GlUtils.h>
#include <ImFusion/GL/GlVolumeRendererGlobalIllum.h>
#include <ImFusion/GL/GlVolumeView.h>
#include <ImFusion/GL/SharedImageSet.h>
#include <ImFusion/GL/TransferFunction.h>
#include <ImFusion/GUI/Animations.h>
#include <ImFusion/GUI/ImageView2D.h>
#include <ImFusion/GUI/ImageView3D.h>
#include <ImFusion/GUI/QtHelpers.h>

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QQuickWindow>


// needed for GLCHECK macro
using ImFusion::GlContext;


// ================================================================================================


ImFusionViewRenderer::ImFusionViewRenderer(ImFusionFboView* parentView)
	: m_disp(nullptr)
	, m_parentView(parentView)
	, m_window(nullptr)
{
}


ImFusionViewRenderer::~ImFusionViewRenderer()
{
	// remove any data that might still be shown in one of the views.
	m_disp->showData(ImFusion::DataList());
}


QOpenGLFramebufferObject* ImFusionViewRenderer::createFramebufferObject(const QSize& size)
{
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	return new QOpenGLFramebufferObject(size, format);
}


void ImFusionViewRenderer::render()
{
	if (!m_disp)
	{
		m_disp = std::make_unique<ImFusion::DisplayWidgetMulti>(true);
		m_disp->setBackgroundColor(QColor::fromRgbF(0.1, 0.1, 0.15));

		// per default, create 3 MPR views + 1 3D Volume Renderer View
		// the individual views can be shown/hidden with setViewVisibility()
		m_disp->addViewGroup3D(false, true);
		IMFUSION_ASSERT(m_disp->numberSliceViews() == 3 && m_disp->number3DViews() == 1);

		m_disp->view3D()->view()->setVr(std::make_unique<ImFusion::GlVolumeRendererGlobalIllum>());
		for (auto* v : m_disp->views())
			v->setVisible(true);

		m_disp->installEventFilter(m_parentView);
	}

	// Called with the FBO bound and the viewport set.
	if (m_disp && m_window)
	{
		// perform the rendering
		try
		{
			ImFusion::GlUtils::resetOpenGlStateDefaults();

			// QML disables GL_POINT_SPRITE by default while the ImFusion SDK expects it to be enabled
			// Since older versions of resetOpenGlStateDefaults() do not do enable it, we do it here manually.
			QOpenGLFunctions* glFuncs = QOpenGLContext::currentContext()->functions();
			glFuncs->glEnable(GL_POINT_SPRITE);

			m_disp->render(false);
			GLCHECK;
		}
		catch (std::exception& e)
		{
			LOG_ERROR("Rendering error: " << e.what());
		}
	}
}


void ImFusionViewRenderer::synchronize(QQuickFramebufferObject* item)
{
	if (m_disp && (m_disp->width() != item->width() || m_disp->height() != item->height()))
	{
		// update viewport
		m_disp->setWidth(item->width());
		m_disp->setHeight(item->height());
		auto rect = QRect(0, 0, item->width(), item->height());
		if (rect != m_disp->viewport())
		{
			m_disp->setCustomViewport(rect);
		}
		m_window = item->window();
	}
}


ImFusion::DisplayWidgetMulti& ImFusionViewRenderer::disp()
{
	return *m_disp;
}


// ================================================================================================


ImFusionFboView::ImFusionFboView(QQuickItem* parent)
	: QQuickFramebufferObject(parent)
	, m_renderer(nullptr)
{
	setAcceptedMouseButtons(Qt::AllButtons);
	setAcceptHoverEvents(true);
	setFlag(QQuickItem::ItemHasContents, true);
	setMirrorVertically(true);
}


ImFusionFboView::~ImFusionFboView()
{
}


QQuickFramebufferObject::Renderer* ImFusionFboView::createRenderer() const
{
	const_cast<ImFusionFboView*>(this)->m_renderer = new ImFusionViewRenderer(const_cast<ImFusionFboView*>(this));
	return m_renderer;
}


bool ImFusionFboView::event(QEvent* e)
{
	if (m_renderer == nullptr)
		return false;

	if (e->type() == QEvent::HoverMove)
	{
		// transform QtQuick QHoverEvent into a QMouseEvent that QtWidgets on the ImFusion SDK side understands.
		IMFUSION_ASSERT(dynamic_cast<QHoverEvent*>(e) != nullptr);
		QHoverEvent* he = static_cast<QHoverEvent*>(e);
		QMouseEvent mouseEvent(QEvent::MouseMove, he->posF(), Qt::NoButton, Qt::NoButton, he->modifiers());
		return QApplication::sendEvent(&m_renderer->disp(), &mouseEvent);
	}
	else
	{
		return QApplication::sendEvent(&m_renderer->disp(), e);
	}
}


void ImFusionFboView::setVisibleData(const ImFusion::DataList& dataList)
{
	if (m_renderer == nullptr)
		return;

	m_renderer->disp().showData(dataList);
}


void ImFusionFboView::onLoadClicked()
{
	// remove any data that might still be shown in one of the views.
	setVisibleData({});

	QString filename = ImFusion::QtHelpers::getOpenFilename(nullptr, "LoadImage", "Load Image");
	if (!filename.isEmpty())
	{
		// Just a temporary solution to have this code here.
		// For me as a QML novice, this was the easiest way to implement some kind of load button callback functionality.
		ImFusionClient::instance().loadImage(filename);

		ImFusion::DataList dl;
		for (auto& sis : ImFusionClient::instance().images())
		{
			dl.add(sis.get());
		}
		setVisibleData(dl);


		// reinitialize views so that they are centered on the shown data.
		ImFusion::Animations::enableAnimations(false);
		for (auto& view : m_renderer->disp().views())
		{
			view->reset();
		}
		ImFusion::Animations::enableAnimations(true);
		update();
	}
}


void ImFusionFboView::onTFToggled()
{
	// small example how to change the transfer function used in the 3D view
	for (auto& sis : ImFusionClient::instance().images())
	{
		auto& dop3d = ImFusion::DisplayOptions3d::get(*sis);
		const std::string& currentPreset = dop3d.transferFunction().presetName();
		if (currentPreset == "CT Bone")
		{
			dop3d.transferFunction() = *ImFusion::TransferFunctionFactory::createMriDefaultPreset(*sis);
		}
		else
		{
			dop3d.transferFunction() = *ImFusion::TransferFunctionFactory::createCtBonePreset();
		}
	}
	update();
}


void ImFusionFboView::onViewsToggled()
{
	if (m_renderer == nullptr)
		return;

	ImFusion::DisplayWidgetMulti& display = m_renderer->disp();
	IMFUSION_ASSERT(display.numberSliceViews() == 3 && display.number3DViews() == 1);
	display.viewSlice(0)->setVisible(true);
	display.viewSlice(1)->setVisible(!display.viewSlice(1)->isVisible());
	display.viewSlice(2)->setVisible(!display.viewSlice(2)->isVisible());
	display.view3D(0)->setVisible(true);
	update();
}


bool ImFusionFboView::eventFilter(QObject* obj, QEvent* e)
{
	bool ok = QObject::eventFilter(obj, e);

	// update the quick view when the DisplayWidget was updated
	if (m_renderer != nullptr && obj == &m_renderer->disp() && e->type() == QEvent::UpdateRequest)
	{
		update();
	}

	return ok;
}
