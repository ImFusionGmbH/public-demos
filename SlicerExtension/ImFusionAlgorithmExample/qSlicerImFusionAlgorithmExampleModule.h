#ifndef __qSlicerImFusionAlgorithmExampleModule_h
#define __qSlicerImFusionAlgorithmExampleModule_h

// Slicer includes
#include "qSlicerImFusionAlgorithmExampleModuleExport.h"
#include "qSlicerLoadableModule.h"
#include "vtkSlicerImFusionAlgorithmExampleLogic.h"

class qSlicerImFusionAlgorithmExampleModulePrivate;
class vtkSlicerImFusionAlgorithmExampleLogic;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_IMFUSIONALGORITHMEXAMPLE_EXPORT qSlicerImFusionAlgorithmExampleModule : public qSlicerLoadableModule
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
	Q_INTERFACES(qSlicerLoadableModule);

public:
	typedef qSlicerLoadableModule Superclass;
	explicit qSlicerImFusionAlgorithmExampleModule(QObject* parent = nullptr);
	~qSlicerImFusionAlgorithmExampleModule() override;

	qSlicerGetTitleMacro(tr("RegistrationModule"));

	QString helpText() const override;
	QString acknowledgementText() const override;
	QStringList contributors() const override;

	QIcon icon() const override;

	QStringList categories() const override;
	QStringList dependencies() const override;

protected:
	/// Initialize the module. Register the volumes reader/writer
	void setup() override;

	/// Create and return the widget representation associated to this module
	qSlicerAbstractModuleRepresentation* createWidgetRepresentation() override;

	/// Create and return the logic associated to this module
	vtkMRMLAbstractLogic* createLogic() override;

protected:
	QScopedPointer<qSlicerImFusionAlgorithmExampleModulePrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qSlicerImFusionAlgorithmExampleModule);
	Q_DISABLE_COPY(qSlicerImFusionAlgorithmExampleModule);
};

#endif
