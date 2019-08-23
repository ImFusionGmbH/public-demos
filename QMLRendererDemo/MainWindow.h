#pragma once

#include <QtQuick/QQuickView>
#include <QtGui/QOpenGLContext>

class MainWindow : public QQuickView
{
	Q_OBJECT
	public:
		MainWindow(QWindow *parent = 0, QQmlEngine* engine = 0);
		
};

