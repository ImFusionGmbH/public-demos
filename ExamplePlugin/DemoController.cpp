#include "DemoController.h"

#include "DemoAlgorithm.h"

#include <ImFusion/Base/DataModel.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/GUI/MainWindowBase.h>

#include "ui_DemoController.h"


namespace ImFusion
{
	DemoController::DemoController(DemoAlgorithm* algorithm)
		: AlgorithmController(algorithm)
		, m_alg(algorithm)
	{
		m_ui = new Ui_DemoController();
		m_ui->setupUi(this);
		connect(m_ui->pushButtonApply, SIGNAL(clicked()), this, SLOT(onApply()));
	}


	DemoController::~DemoController() { delete m_ui; }


	void DemoController::init() { addToAlgorithmDock(); }


	void DemoController::onApply()
	{
		m_alg->setFactor(m_ui->spinBoxFactor->value());
		m_alg->compute();
		DataList d;
		m_alg->output(d);
		for (auto i : d.getImages(Data::UNKNOWN))
			m_main->dataModel()->add(i);
	}
}
