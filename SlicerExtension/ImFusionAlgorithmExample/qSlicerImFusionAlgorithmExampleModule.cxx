// ImFusionAlgorithmExample Logic includes
#include <vtkSlicerImFusionAlgorithmExampleLogic.h>

// ImFusionAlgorithmExample includes
#include "qSlicerImFusionAlgorithmExampleModule.h"
#include "qSlicerImFusionAlgorithmExampleModuleWidget.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerImFusionAlgorithmExampleModulePrivate
{
public:
	qSlicerImFusionAlgorithmExampleModulePrivate();
};

// qSlicerImFusionAlgorithmExampleModulePrivate methods

qSlicerImFusionAlgorithmExampleModulePrivate::qSlicerImFusionAlgorithmExampleModulePrivate() {}

// qSlicerImFusionAlgorithmExampleModule methods

qSlicerImFusionAlgorithmExampleModule::qSlicerImFusionAlgorithmExampleModule(QObject* _parent)
	: Superclass(_parent)
	, d_ptr(new qSlicerImFusionAlgorithmExampleModulePrivate)
{
}

qSlicerImFusionAlgorithmExampleModule::~qSlicerImFusionAlgorithmExampleModule() {}

QString qSlicerImFusionAlgorithmExampleModule::helpText() const { return ""; }

QString qSlicerImFusionAlgorithmExampleModule::acknowledgementText() const { return ""; }

QStringList qSlicerImFusionAlgorithmExampleModule::contributors() const
{
	QStringList moduleContributors;
	moduleContributors << QString("ImFusion Team");
	return moduleContributors;
}

QIcon qSlicerImFusionAlgorithmExampleModule::icon() const { return QIcon(":/Icons/ImFusionLogo.png"); }

QStringList qSlicerImFusionAlgorithmExampleModule::categories() const { return QStringList() << "ImFusion"; }

QStringList qSlicerImFusionAlgorithmExampleModule::dependencies() const { return QStringList(); }

void qSlicerImFusionAlgorithmExampleModule::setup() { this->Superclass::setup(); }

qSlicerAbstractModuleRepresentation* qSlicerImFusionAlgorithmExampleModule ::createWidgetRepresentation()
{
	return new qSlicerImFusionAlgorithmExampleModuleWidget;
}

vtkMRMLAbstractLogic* qSlicerImFusionAlgorithmExampleModule::createLogic() { return vtkSlicerImFusionAlgorithmExampleLogic::New(); }
