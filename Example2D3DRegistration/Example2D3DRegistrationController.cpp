#include "Example2D3DRegistrationController.h"

#include "Example2D3DRegistrationAlgorithm.h"

#include <ImFusion/Base/DataModel.h>
#include <ImFusion/Base/FactoryRegistry.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Core/Log.h>
#include <ImFusion/CT/GUI/XRay2D3DRegistrationAlgorithmController.h>
#include <ImFusion/CT/GUI/XRay2D3DRegistrationInitializationController.h>
#include <ImFusion/CT/GUI/XRay2D3DRegistrationInitializationKeyPointsController.h>
#include <ImFusion/CT/XRay2D3DRegistrationAlgorithm.h>
#include <ImFusion/GUI/MainWindowBase.h>

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>

// The following sets the log category for this file to "Example2D3DRegistrationController"
#undef IMFUSION_LOG_DEFAULT_CATEGORY
#define IMFUSION_LOG_DEFAULT_CATEGORY "Example2D3DRegistrationController"

// This class implements the GUI controller for the Example2D3DRegistrationAlgorithm.
namespace ImFusion
{

	Example2D3DRegistrationController::Example2D3DRegistrationController(Example2D3DRegistrationAlgorithm* algorithm)
		: AlgorithmController(algorithm)
		, m_alg(algorithm)
	{
		// Adds a button with which to launch the algorithm
		m_computeButton = new QPushButton("Compute projections, launch registration");
		this->setLayout(new QHBoxLayout);
		this->layout()->addWidget(m_computeButton);
		connect(m_computeButton, SIGNAL(clicked()), this, SLOT(onCompute()));
	}

	void Example2D3DRegistrationController::onCompute()
	{
		// Disable the button created in constructor.
		if (m_computeButton)
			m_computeButton->setEnabled(false);

		// Call compute on the algorithm
		m_alg->compute();

		// Initialize the controller for the XRay2D3DRegistrationAlgorithm created in m_alg->compute() above 
		m_regAlgCtrl.reset(dynamic_cast<CT::XRay2D3DRegistrationAlgorithmController*>(FactoryRegistry::get().createAlgorithmController(m_alg->regAlg())));
		m_main->addController(m_regAlgCtrl.get());

		// Add controller for the custom initialization method. Since the custom initialization method internally
		// uses the KeyPoints initialization method, we simply use the controller from that method.
		// Since the key points are fixed, we hide the widget. This means the widget only draws annotations onto
		// the 2D and 3D views.
		// Warning: the widget will become visible if the initialization mode is changed in the controller.
		// you can taylor your own class derived from XRay2D3DRegistrationInitializationController to fit more specific requirements.
		Custom2D3DRegistrationInitialization* customInit = m_alg->customInit();
		CT::XRay2D3DRegistrationInitializationController* kpCtrl = new CT::XRay2D3DRegistrationInitializationKeyPointsController(
			nullptr, *m_regAlgCtrl->shotsView(), *m_regAlgCtrl->regViewGroup(), *customInit->kpAlg(), *m_main);
		m_regAlgCtrl->setCustomInitializationController(std::unique_ptr<CT::XRay2D3DRegistrationInitializationController>(kpCtrl));
		kpCtrl->setVisible(false);
	}

	Example2D3DRegistrationController::~Example2D3DRegistrationController() {}

	void Example2D3DRegistrationController::init() {}
}