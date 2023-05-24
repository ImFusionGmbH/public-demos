/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <ImFusion/GUI/AlgorithmController.h>
#include <ImFusion/CT/GUI/XRay2D3DRegistrationAlgorithmController.h>

#include <QtWidgets/QWidget>

class QPushButton;

namespace ImFusion
{
	using namespace CT;

	class Example2D3DRegistrationAlgorithm;

	// This class implements the GUI controller for the Example2D3DRegistrationAlgorithm.
	class Example2D3DRegistrationController : public QWidget, public AlgorithmController
	{
		Q_OBJECT

	public:
		// Constructor with the algorithm instance
		Example2D3DRegistrationController(Example2D3DRegistrationAlgorithm* algorithm);

		// Destructor
		virtual ~Example2D3DRegistrationController();

		// Initializes the widget
		void init();

	public slots:
		void onCompute();

	protected:
		Example2D3DRegistrationAlgorithm* m_alg;                                   //< The algorithm instance
		std::unique_ptr<XRay2D3DRegistrationAlgorithmController> m_regAlgCtrl;    //< Controller for the registration algorithm
		QPushButton* m_computeButton = nullptr;                                   //< Button that is clicked to launch onCompute
	};
}
