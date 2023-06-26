#include "FiberDataRenderOptionsWidget.h"

#include "FiberData.h"
#include "FiberDataRenderer.h"

#include <ImFusion/GUI/CoreWidgets/AdjusterWidget.h>
#include <ImFusion/GUI/DisplayWidgetMulti.h>
#include <ImFusion/GUI/QtHelpers.h>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

namespace ImFusion
{
	FiberDataRenderOptionsWidget::FiberDataRenderOptionsWidget(FiberDataRenderer::Options* renderOptions, DisplayWidgetMulti* display)
		: m_display(display)
	{
		initGUI();
		setRenderOptions(renderOptions);
	}


	FiberDataRenderOptionsWidget::~FiberDataRenderOptionsWidget() = default;


	void FiberDataRenderOptionsWidget::setRenderOptions(FiberDataRenderer::Options* renderOptions)
	{
		m_renderOptions = renderOptions;
		m_cbRenderMode->setEnabled(m_renderOptions != nullptr);
		m_cbColoringMode->setEnabled(m_renderOptions != nullptr);
		m_awFiberSize->setEnabled(m_renderOptions != nullptr);
		m_awLengthThreshold->setEnabled(m_renderOptions != nullptr);
		m_cbColorCoding->setEnabled(m_renderOptions != nullptr);

		if (m_renderOptions)
		{
			QtHelpers::setSilent(m_cbRenderMode, static_cast<int>(m_renderOptions->p_renderMode.value()));
			QtHelpers::setSilent(m_cbColoringMode, int(m_renderOptions->p_coloringMode.value()));
			QtHelpers::setSilent(m_awFiberSize, m_renderOptions->p_fiberWidth.value());
			QtHelpers::setSilent(m_awLengthThreshold, m_renderOptions->p_lengthThreshold.value());
			QtHelpers::setSilent(m_cbColorCoding, m_renderOptions->p_enableAnisotropyColorcoding.value());
		}
	}


	void FiberDataRenderOptionsWidget::initGUI()
	{
		m_cbRenderMode = new QComboBox();
		m_cbRenderMode->addItem("Stripes");
		m_cbRenderMode->addItem("Streamtubes");
		connect(m_cbRenderMode,
				static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
				[this](int val)
				{
					if (m_renderOptions)
						m_renderOptions->p_renderMode = static_cast<FiberDataRenderer::Options::RenderMode>(val);
					updateDisplay();
				});

		m_cbColoringMode = new QComboBox();
		m_cbColoringMode->addItem("Local Tangent");
		m_cbColoringMode->addItem("Average Tangent");
		m_cbColoringMode->addItem("Center Tangent");
		m_cbColoringMode->addItem("HSV Projected Tangent");
		connect(m_cbColoringMode,
				static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
				[this](int val)
				{
					if (m_renderOptions)
						m_renderOptions->p_coloringMode = static_cast<FiberDataRenderer::Options::ColoringMode>(val);
					updateDisplay();
				});

		m_awFiberSize = new DoubleAdjusterWidget(0.01, 1.0, 0.01);
		m_awFiberSize->setValue(0.1);
		m_awFiberSize->spinBox()->setSuffix("mm");
		connect(m_awFiberSize,
				&DoubleAdjusterWidget::valueChanged,
				[this](double val)
				{
					if (m_renderOptions)
						m_renderOptions->p_fiberWidth = val;
					updateDisplay();
				});

		m_awLengthThreshold = new DoubleAdjusterWidget(0.0, 200.0, 0.5);
		m_awLengthThreshold->setValue(0.1);
		m_awLengthThreshold->spinBox()->setSuffix("mm");
		connect(m_awLengthThreshold,
				&DoubleAdjusterWidget::valueChanged,
				[this](double val)
				{
					if (m_renderOptions)
						m_renderOptions->p_lengthThreshold = val;
					updateDisplay();
				});

		m_cbColorCoding = new QCheckBox();
		m_cbColorCoding->setChecked(true);
		connect(m_cbColorCoding,
				&QCheckBox::stateChanged,
				[this]()
				{
					if (m_renderOptions)
						m_renderOptions->p_enableAnisotropyColorcoding = m_cbColorCoding->isChecked();
					updateDisplay();
				});

		auto layout = new QGridLayout(this);
		layout->addWidget(new QLabel(tr("Render mode:")), 5, 0);
		layout->addWidget(m_cbRenderMode, 5, 1);
		layout->addWidget(new QLabel(tr("Coloring mode:")), 7, 0);
		layout->addWidget(m_cbColoringMode, 7, 1);
		layout->addWidget(new QLabel(tr("Fiber width:")), 10, 0);
		layout->addWidget(m_awFiberSize, 10, 1);
		layout->addWidget(new QLabel(tr("Length threshold:")), 20, 0);
		layout->addWidget(m_awLengthThreshold, 20, 1);
		layout->addWidget(new QLabel(tr("Anisotropy color-coding:")), 30, 0);
		layout->addWidget(m_cbColorCoding, 30, 1);
	}


	void FiberDataRenderOptionsWidget::updateDisplay()
	{
		if (m_display)
			m_display->update();
	}
}
