#pragma once

#include <ImFusion/GL/SharedImageSet.h>

#include <QObject>
#include <QOpenGLContext>
#include <memory>
#include <vector>


/// Singleton class for managing ImFusion SDK initialization and holding the loaded data.
/// 
/// This class uses the qtQuickOpenglContextCreated slot to initialize the ImFusion SDK in a
/// way that shares the QML OpenGL context. This allows rendering into an FBO (using
/// the ImFusionFboView), which is then blitted into the Qt Quick scene graph.
/// Furthermore, this class provides a convenience function for loading images from disk.
class ImFusionClient : public QObject
{
	Q_OBJECT

public:
	/// Return the Singleton instance.
	static ImFusionClient& instance();

	/// Use the ImFusion IO factories to load a file from disk.
	bool loadImage(const QString& filename);

	/// Access all loaded images.
	const std::vector<std::unique_ptr<ImFusion::SharedImageSet>>& images() { return m_images; }


public slots:
	void qtQuickOpenglContextCreated(QOpenGLContext* context);
	void qtQuickOpenglContextAboutToBeDestroyed();

private:
	ImFusionClient();
	std::vector<std::unique_ptr<ImFusion::SharedImageSet>> m_images;    ///< List of loaded images
};
