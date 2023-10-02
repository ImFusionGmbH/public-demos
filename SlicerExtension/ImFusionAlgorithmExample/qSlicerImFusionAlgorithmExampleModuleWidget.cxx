// Qt includes
#include <QDebug>

// Slicer includes
#include "qSlicerImFusionAlgorithmExampleModuleWidget.h"
#include "vtkSlicerImFusionAlgorithmExampleLogic.h"

#include <vtkCollection.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSubjectHierarchyConstants.h>
#include <vtkMRMLSubjectHierarchyNode.h>
#include <vtkMRMLVolumeNode.h>

#include "ui_qSlicerImFusionAlgorithmExampleModuleWidget.h"

namespace
{
	std::string GetModality(vtkMRMLVolumeNode* node, vtkMRMLScene* scene)
	{
		vtkMRMLSubjectHierarchyNode* shNode = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNode(scene);
		if (shNode)
		{
			return shNode->GetItemAttribute(shNode->GetItemByDataNode(node), vtkMRMLSubjectHierarchyConstants::GetDICOMSeriesModalityAttributeName());
		}
		return {};
	}
}

/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerImFusionAlgorithmExampleModuleWidgetPrivate : public Ui_qSlicerImFusionAlgorithmExampleModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerImFusionAlgorithmExampleModuleWidget);

public:
	qSlicerImFusionAlgorithmExampleModuleWidgetPrivate(qSlicerImFusionAlgorithmExampleModuleWidget& object);
	vtkSlicerImFusionAlgorithmExampleLogic* logic() const;

protected:
	qSlicerImFusionAlgorithmExampleModuleWidget* const q_ptr;
};

// qSlicerImFusionAlgorithmExampleModuleWidgetPrivate methods

qSlicerImFusionAlgorithmExampleModuleWidgetPrivate::qSlicerImFusionAlgorithmExampleModuleWidgetPrivate(
	qSlicerImFusionAlgorithmExampleModuleWidget& object)
	: q_ptr(&object)
{
}

vtkSlicerImFusionAlgorithmExampleLogic* qSlicerImFusionAlgorithmExampleModuleWidgetPrivate::logic() const
{
	Q_Q(const qSlicerImFusionAlgorithmExampleModuleWidget);
	return vtkSlicerImFusionAlgorithmExampleLogic::SafeDownCast(q->logic());
}

// qSlicerImFusionAlgorithmExampleModuleWidget methods

qSlicerImFusionAlgorithmExampleModuleWidget::qSlicerImFusionAlgorithmExampleModuleWidget(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSlicerImFusionAlgorithmExampleModuleWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSlicerImFusionAlgorithmExampleModuleWidget::~qSlicerImFusionAlgorithmExampleModuleWidget() {}

void qSlicerImFusionAlgorithmExampleModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
	Superclass::setMRMLScene(scene);

	assert(scene);
	std::vector<vtkMRMLNode*> volumeNodes;
	scene->GetNodesByClass("vtkMRMLVolumeNode", volumeNodes);

	for (auto node : volumeNodes)
	{
		if (!node)
			continue;

		d_ptr->fixComboBox->addItem(node->GetName());
		d_ptr->moveComboBox->addItem(node->GetName());

		if (d_ptr->fixComboBox->currentIndex() == d_ptr->moveComboBox->currentIndex() && d_ptr->moveComboBox->count() > 1)
		{
			d_ptr->moveComboBox->setCurrentIndex((d_ptr->fixComboBox->currentIndex() + 1) % d_ptr->fixComboBox->count());
		}
	}

	auto oldScene = mrmlScene();

	qvtkReconnect(oldScene, scene, vtkMRMLScene::NodeAddedEvent, this, SLOT(onNodeAdded(vtkObject*, vtkObject*)));
	qvtkReconnect(oldScene, scene, vtkMRMLScene::NodeRemovedEvent, this, SLOT(onNodeRemoved(vtkObject*, vtkObject*)));
}

//-----------------------------------------------------------------------------
void qSlicerImFusionAlgorithmExampleModuleWidget::setup()
{
	Q_D(qSlicerImFusionAlgorithmExampleModuleWidget);
	d->setupUi(this);
	this->Superclass::setup();

	std::vector<std::string> similarities = {"SAD", "SSD", "NCC", "LNCC", "LC2", "MI"};
	for (const auto& sim : similarities)
		d->similarityComboBox->addItem(sim.c_str());

	std::vector<std::string> devices = {"GPU", "CPU"};
	for (const auto& dev : devices)
		d->deviceComboBox->addItem(dev.c_str());


	connect(d->applyButton, &QPushButton::clicked, this, [this, d, similarities, devices](bool) {
		d->applyButton->setDisabled(true);

		auto scene = mrmlScene();
		if (!scene)
			return;

		const auto& similarity = similarities[d->similarityComboBox->currentIndex()];
		const auto& device = devices[d->deviceComboBox->currentIndex()];

		qDebug() << "Registering: \n"
				 << "	Fixed - " << d_ptr->fixComboBox->currentText() << "\n"
				 << "	Moving - " << d_ptr->moveComboBox->currentText() << "\n"
				 << "	Similarity: " << similarity.c_str() << "\n"
				 << "	Device: " << device.c_str() << "\n";

		auto fixNode = dynamic_cast<vtkMRMLVolumeNode*>(scene->GetFirstNodeByName(d_ptr->fixComboBox->currentText().toStdString().c_str()));
		auto moveNode = dynamic_cast<vtkMRMLVolumeNode*>(scene->GetFirstNodeByName(d_ptr->moveComboBox->currentText().toStdString().c_str()));
		if (fixNode && moveNode)
		{
			d_ptr->logic()->registerVolumes(
				fixNode,
				moveNode,
				static_cast<vtkSlicerImFusionAlgorithmExampleLogic::SimilarityMeasure>(d->similarityComboBox->currentIndex()),
				static_cast<vtkSlicerImFusionAlgorithmExampleLogic::Device>(d->deviceComboBox->currentIndex()));
		}

		d->applyButton->setDisabled(false);
	});


	auto setSimilarityFromModality = [this](const QString& text) {
		auto fixNode = dynamic_cast<vtkMRMLVolumeNode*>(mrmlScene()->GetFirstNodeByName(d_ptr->fixComboBox->currentText().toStdString().c_str()));
		auto moveNode = dynamic_cast<vtkMRMLVolumeNode*>(mrmlScene()->GetFirstNodeByName(d_ptr->moveComboBox->currentText().toStdString().c_str()));

		const auto fixModality = GetModality(fixNode, mrmlScene());
		const auto moveModality = GetModality(moveNode, mrmlScene());

		if ((fixModality == "CT" && moveModality == "MR") || (fixModality == "MR" && moveModality == "CT"))
		{
			d_ptr->similarityComboBox->setCurrentIndex(static_cast<int>(vtkSlicerImFusionAlgorithmExampleLogic::SimilarityMeasure::MI));
		}
		else
		{
			d_ptr->similarityComboBox->setCurrentIndex(static_cast<int>(vtkSlicerImFusionAlgorithmExampleLogic::SimilarityMeasure::LC2));
		}
	};

	connect(d_ptr->fixComboBox, &QComboBox::currentTextChanged, setSimilarityFromModality);

	connect(d_ptr->moveComboBox, &QComboBox::currentTextChanged, setSimilarityFromModality);
}


void qSlicerImFusionAlgorithmExampleModuleWidget::onNodeAdded(vtkObject* vtkNotUsed(scene), vtkObject* node)
{
	if (auto volNode = dynamic_cast<vtkMRMLVolumeNode*>(node))
	{
		d_ptr->fixComboBox->addItem(volNode->GetName());
		d_ptr->moveComboBox->addItem(volNode->GetName());
	}

	// if the selected node is the same, set the moving one to the next one in the list
	auto current = d_ptr->fixComboBox->currentIndex();

	if (current == d_ptr->moveComboBox->currentIndex() && d_ptr->moveComboBox->count() > 1)
	{
		int next = (d_ptr->fixComboBox->currentIndex() + 1) % d_ptr->fixComboBox->count();
		d_ptr->moveComboBox->setCurrentIndex(next);
	}
}

void qSlicerImFusionAlgorithmExampleModuleWidget::onNodeRemoved(vtkObject* scene, vtkObject* node)
{
	if (auto volNode = dynamic_cast<vtkMRMLVolumeNode*>(node))
	{
		if (auto ind = d_ptr->fixComboBox->findText(volNode->GetName()); ind != -1)
			d_ptr->fixComboBox->removeItem(ind);

		if (auto ind = d_ptr->moveComboBox->findText(volNode->GetName()); ind != -1)
			d_ptr->moveComboBox->removeItem(ind);
	}
}
