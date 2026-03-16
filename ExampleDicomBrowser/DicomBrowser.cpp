#include "DicomBrowser.h"

#include <ImFusion/Base/Framework.h>

#include <QtWidgets/QApplication>

#include <iostream>

DicomBrowser::DicomBrowser(const QString& directory, QWidget* parent)
	: QDialog(parent)
{
	auto model = new ImFusion::DicomBrowserModel(nullptr, this);

	auto view = new QTreeView(this);
	view->setModel(model);

	auto progress = new QProgressBar(this);
	connect(model, &ImFusion::DicomBrowserModel::scanProgressRangeChanged, progress, &QProgressBar::setRange);
	connect(model, &ImFusion::DicomBrowserModel::scanProgressValueChanged, progress, &QProgressBar::setValue);

	// We scan the directory recursively, consider non DICOMDIR folders, and check dicom headers
	model->scanDirectoryAsync(directory, true, false, true);

	auto layout = new QVBoxLayout(this);
	layout->addWidget(view);
	layout->addWidget(progress);
	setLayout(layout);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "A folder path must be provided as argument" << std::endl;
		return 1;
	}

	// Initialize framework
	ImFusion::Framework::init();

	// Create QApplication
	QApplication app(argc, argv);

	// Initialize DicomBrowser
	DicomBrowser browser(argv[1]);
	browser.show();

	// Start application
	auto ret = QApplication::exec();

	// Deinitialize framework afterwards
	ImFusion::Framework::deinit();

	return ret;
}
