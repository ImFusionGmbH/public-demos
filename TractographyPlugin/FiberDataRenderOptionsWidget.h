#pragma once

#include "FiberDataRenderer.h"

#include <ImFusion/Core/Signal.h>

#include <QtWidgets/QWidget>

#include <memory>

class QCheckBox;
class QComboBox;

namespace ImFusion
{
	class DisplayWidgetMulti;
	class DoubleAdjusterWidget;

	/// Qt widget to configure a FiberDataRenderer::Options instance.
	/// This class is instantiated by FiberDataDisplayHandler::createDisplayOptionsWidget() and shown in
	/// any view currently displaying a FiberData instance when clicking on the "Display Options" button.
	class FiberDataRenderOptionsWidget : public QWidget, public SignalReceiver
	{
		Q_OBJECT

	public:
		explicit FiberDataRenderOptionsWidget(FiberDataRenderer::Options* renderOptions, DisplayWidgetMulti* display);
		virtual ~FiberDataRenderOptionsWidget();

		FiberDataRenderer::Options* renderOptions() const { return m_renderOptions; }
		void setRenderOptions(FiberDataRenderer::Options* renderOptions);

	private:
		void initGUI();
		void updateDisplay();

		DisplayWidgetMulti* m_display;
		FiberDataRenderer::Options* m_renderOptions;

		QComboBox* m_cbRenderMode = nullptr;
		QComboBox* m_cbColoringMode = nullptr;
		DoubleAdjusterWidget* m_awFiberSize = nullptr;
		DoubleAdjusterWidget* m_awLengthThreshold = nullptr;
		QCheckBox* m_cbColorCoding = nullptr;
	};
}
