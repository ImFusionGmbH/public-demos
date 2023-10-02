#ifndef __qSlicerImFusionAlgorithmExampleModuleWidget_h
#define __qSlicerImFusionAlgorithmExampleModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerImFusionAlgorithmExampleModuleExport.h"

class qSlicerImFusionAlgorithmExampleModuleWidgetPrivate;
class vtkMRMLNode;
class vtkSlicerImFusionAlgorithmExampleLogic;
class vtkMRMLScene;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_IMFUSIONALGORITHMEXAMPLE_EXPORT qSlicerImFusionAlgorithmExampleModuleWidget : public qSlicerAbstractModuleWidget
{
	Q_OBJECT

public:
	typedef qSlicerAbstractModuleWidget Superclass;
	qSlicerImFusionAlgorithmExampleModuleWidget(QWidget* parent = 0);
	virtual ~qSlicerImFusionAlgorithmExampleModuleWidget();
public slots:
	void onNodeAdded(vtkObject* scene, vtkObject* node);
	void onNodeRemoved(vtkObject* scene, vtkObject* node);
	void setMRMLScene(vtkMRMLScene* scene) override;

protected:
	QScopedPointer<qSlicerImFusionAlgorithmExampleModuleWidgetPrivate> d_ptr;

	void setup() override;

private:
	Q_DECLARE_PRIVATE(qSlicerImFusionAlgorithmExampleModuleWidget);
	Q_DISABLE_COPY(qSlicerImFusionAlgorithmExampleModuleWidget);
};

#endif
