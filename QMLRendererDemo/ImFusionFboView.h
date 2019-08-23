#pragma once

#include <ImFusion/GL/SharedImageSet.h>
#include <ImFusion/GUI/DisplayWidgetMulti.h>

#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickFramebufferObject>
#include <QtQuick/QQuickItem>

#include <memory>

class ImFusionFboView;


/// QQuickFramebufferObject::Renderer that uses an ImFusion::DisplayWidgetMulti to render ImFusion
/// views into a QtQuick-backed FBO.
class ImFusionViewRenderer : public QQuickFramebufferObject::Renderer
{
public:
	ImFusionViewRenderer(ImFusionFboView* parentView);
	~ImFusionViewRenderer();

	QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
	void render() override;
	void synchronize(QQuickFramebufferObject* item) override;

	ImFusion::DisplayWidgetMulti& disp();

private:
	std::unique_ptr<ImFusion::DisplayWidgetMulti> m_disp;
	ImFusionFboView* m_parentView;
	QQuickWindow* m_window;
};


/// QtQuick Wrapper around an ImFusion::DisplayWidgetMulti.
///
/// A DisplayWidgetMulti can have one or multiple 2D/MPR/3D views showing data. The views can be
/// freely arranged in layouts.
class ImFusionFboView : public QQuickFramebufferObject
{
	Q_OBJECT

public:
	ImFusionFboView(QQuickItem* parent = 0);
	virtual ~ImFusionFboView();

	QQuickFramebufferObject::Renderer* createRenderer() const override;
	bool event(QEvent* e) override;


public slots:
	void setVisibleData(const ImFusion::DataList& dataList);
	void onLoadClicked();
	void onTFToggled();
	void onViewsToggled();

private:
	bool eventFilter(QObject*, QEvent*) override;

	ImFusionViewRenderer* m_renderer;
};
