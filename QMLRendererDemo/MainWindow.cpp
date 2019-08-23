#include "MainWindow.h"
#include "ImFusionClient.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

MainWindow::MainWindow(QWindow *parent, QQmlEngine* qmlEngine)
	: QQuickView(qmlEngine, parent)
{
	setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
	setGeometry(40, 40, 1280, 720);
	setObjectName(QStringLiteral("theMainWindow"));
	setColor(QColor(0, 0, 0));
	setTitle(QCoreApplication::applicationName());

	setSource(QUrl("qrc:/main.qml"));
	if (status() == QQuickView::Error)
	{
		QList<QQmlError>errList = errors();
		for (QQmlError error : errList)
		{
			qDebug() << "QML: " << error.toString();
		}
	}

	connect(qmlEngine, &QQmlEngine::quit, QCoreApplication::instance(), &QCoreApplication::quit);
	connect(this, &QQuickWindow::openglContextCreated, &ImFusionClient::instance(), &ImFusionClient::qtQuickOpenglContextCreated);
}


