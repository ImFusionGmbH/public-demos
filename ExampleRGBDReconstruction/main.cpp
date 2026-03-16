#include "MainWindow.h"

#include <ImFusion/Core/Log.h>
#include <ImFusion/GUI/ConsoleController.h>

#include <QtWidgets/QApplication>

using namespace ImFusion;

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	// Create console controller instance to initialize ImFusion SDK and plugins.
	auto sdk = std::make_unique<ConsoleController>(Framework::InitConfig(), "ImFusion GmbH", "RGB-D Reconstruction Sample");

	// The Plugin needs to be loaded from the framework to be instantiated for usage of the resource system, which manages RGBDReconstruction related shaders
	PluginManager::get().registerPlugins();
	PluginManager::get().initAllRegisteredPlugins();

	MainWindow mw;
	mw.show();

	return app.exec();
}
