#include "ImFusionClient.h"
#include "ImFusionFboView.h"
#include "MainWindow.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QApplication>

int main(int argc, char *argv[])
{
	// Force Qt Quick rendering in the main GUI thread.
	// This greatly simplifies the setup of the ImFusion integration since we do not need to care about threads.
	// Otherwise, we have to ensure that all ImFusionLib objects are accessed from the same thread that they were instantiated in.
	// see: https://doc.qt.io/qt-5/qtquick-visualcanvas-scenegraph.html#scene-graph-and-rendering
	qputenv("QSG_RENDER_LOOP", "basic");
	
	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	QApplication a(argc, argv);
	
	// Force first instantiation of the ImFusionClient singleton to ensure that everything of the
	// ImFusionLib is initialized correctly.
	ImFusionClient::instance();
	
	QQmlEngine engine;
	qmlRegisterType<ImFusionFboView>("ImFusion", 1, 0, "ImFusionFboView");
	
	MainWindow mainWindow(0, &engine);
	mainWindow.show();
	
	return a.exec();
}

