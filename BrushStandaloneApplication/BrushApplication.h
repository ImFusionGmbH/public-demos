#pragma once

#include <ImFusion/GUI/ApplicationController.h>

#include <QMainWindow>

class QWidget;
class QString;

namespace ImFusion
{
	class SharedImageSet;
	class DisplayWidgetMulti;

	namespace Seg
	{
		class Brush;
		class LabelPainter;
		class UndoRecorder;
	}

	// Minimalistic demo application that shows a given DICOM data set (passed via command line)
	// and allows the user to create a label map for the dataset via a semi-automatic brushing tool.
	// We inherit from ImFusion::ApplicationController to automatically take care of initializing
	// the ImFusion SDK as soon as this class is instantiated.
	class BrushApplication : public QMainWindow, public ApplicationController
	{
	public:
		explicit BrushApplication(const std::string& pathToDicomData);
		~BrushApplication() override;

	private:
		// Sets up the UI elements to control the brush (selection, sliders, etc)
		void setupUI();
		// Sets up the DisplayWidgetMulti object and its view layout
		void setupViews();
		// Sets up the brush object
		void setupBrush(SharedImageSet* selectedImage);
		// Sets up the UndoRecorder
		void setupUndo();

		// Undo/Redo
		void triggerRedo();
		void triggerUndo();

		// Change brush parameters
		void changeLabelValue(int value);
		void changeLabelColor(const QString& text);
		void changeBrushSize(int sizeMM);
		void changeBrushAdaptiveness(float adaptiveness);
		void changeFilterLevel(int imageFilterLevel);

		std::unique_ptr<DisplayWidgetMulti> m_display;    ///< Widget for displaying the image data
		QWidget* m_dispWrapper = nullptr;

		std::unique_ptr<Seg::Brush> m_brush;                  ///< High level class to easily manage brushing behavior
		std::unique_ptr<Seg::LabelPainter> m_labelPainter;    ///< Painter for label maps
		SharedImageSet* m_labelMap;                           ///< SharedImageSet representing a label-map with uint8 values
		int m_currentLabelValue = 1;                          ///< Current value of the label map ([0,255])
		vec4 m_currentLabelColor = vec4{1, 0, 0, 1};          ///< Current color of the label map

		std::unique_ptr<Seg::UndoRecorder> m_undoRecorder;    ///< Implements undo-recording for a SharedImageSet
		int m_currentUndoIndex = 0;                           ///< Current undo index
	};
}
