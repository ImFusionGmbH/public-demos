#include "BrushApplication.h"

#include <ImFusion/Base/DataModel.h>
#include <ImFusion/Base/LabelDataComponent.h>
#include <ImFusion/Base/TypedImage.h>
#include <ImFusion/Core/GL/ContextManager.h>
#include <ImFusion/Dicom/DicomLoader.h>
#include <ImFusion/GL/SharedImageSet.h>
#include <ImFusion/GUI/DisplayWidgetMulti.h>
#include <ImFusion/GUI/GlContextQt.h>
#include <ImFusion/GUI/InteractiveView.h>
#include <ImFusion/GUI/QtHelpers.h>
#include <ImFusion/Seg/Brush.h>
#include <ImFusion/Seg/LabelPainter.h>
#include <ImFusion/Seg/UndoRecorder.h>

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QString>
#include <QVBoxLayout>

#undef IMFUSION_LOG_DEFAULT_CATEGORY
#define IMFUSION_LOG_DEFAULT_CATEGORY "BrushStandaloneApplication"

// main entry point
// you can pass via command-line a path to dicom data to be loaded
int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	try
	{
		ImFusion::BrushApplication ex{
			argc == 2 ? argv[1]
					  : ImFusion::QtHelpers::getOpenFilename(nullptr, "BrushApplication.InputDicomFile", "Select DICOM File", "*.dcm").toStdString()};

		ex.show();
		QApplication::exec();
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Caught exception: " << e.what());
		return 1;
	}
}

namespace ImFusion
{
	namespace
	{
		std::vector<std::unique_ptr<SharedImageSet>> loadData(const std::string& pathToDicomData)
		{    // Use the DicomLoader to load DICOM data from the disk
			DicomLoader dicomLoader(pathToDicomData);
			auto images = dicomLoader.loadImages();
			if (images.empty())
			{
				throw std::logic_error{"Could not load data from given path: " + pathToDicomData};
			}

			return images;
		}

		// Creates a "Label Map" with value `labelValue` and color (RGBA) `labelColor` for the corresponding SharedImageSet
		// A "Label Map" is a SharedImageSet with PixelType UByte, therefor the labelValue parameter has to be smaller than 256 and non-negative
		std::unique_ptr<SharedImageSet> createLabelMapFromImage(const SharedImageSet& image, int labelValue, const vec4& labelColor)
		{
			if (labelValue < 0 || labelValue > 255)
			{
				LOG_ERROR("LabelMap value must be between [0,255]");
				return nullptr;
			}

			auto labelMap = std::make_unique<SharedImageSet>();

			// Copy the descriptor from the image, set the PixelType to UByte
			ImageDescriptor desc = image.get()->descriptor();
			desc.pixelType = PixelType::UByte;

			// Create an empty image of type uint8_t
			labelMap->add(TypedImage<uint8_t>::zeros(desc), image.get()->matrix());
			labelMap->setModality(Data::LABEL);

			// We set the same matrix as the data for each image
			for (int i = 0; i < labelMap->size(); ++i)
			{
				labelMap->get(i)->setMatrix(image.get(i)->matrix());
			}

			// Add the LabelDataComponent with default configuration so that the label map is correctly visualized
			LabelDataComponent& ldc = labelMap->components().getOrCreate<LabelDataComponent>();
			// set initial configuration of labelmap (e.g. name, color, etc)
			const LabelDataComponent::LabelConfig config{.name = "LabelName", .color = labelColor, .isVisible2d = true, .isVisible3d = true};
			ldc.setLabelConfig(labelValue, config);

			return labelMap;
		}
	}

	BrushApplication::BrushApplication(const std::string& pathToDicomData)
		// Construct the `ApplicationController` with a Qt OpenGL context so that we can use a DisplayWidget later on.
		// Tell the framework that it should load the needed plugins by setting `loadPlugins = true` in `FrameWork::InitConfig`
		: ApplicationController(std::make_unique<ImFusion::DataModel>(), []() {
			ImFusion::Framework::InitConfig initConfig{.glContext = std::make_unique<ImFusion::GlContextQt>(), .loadPlugins = true};
			return initConfig;
		}())
	{
		setupViews();

		auto images = loadData(pathToDicomData);

		// Add images to data model
		for (auto& sis : images)
		{
			dataModel()->add(std::move(sis));
		}

		// Take the first image in the data model (the only one we have here) to create a label map to be drawn into
		SharedImageSet* selectedImage = dataModel()->getAll().getFirst<SharedImageSet>();
		if (!selectedImage)
		{
			throw std::logic_error{"Could not retrieve data from data-model"};
		}

		// Create a label map for the given data with some initial settings
		std::unique_ptr<SharedImageSet> labelMapUnique = createLabelMapFromImage(*selectedImage, m_currentLabelValue, m_currentLabelColor);
		m_labelMap = labelMapUnique.get();

		// Add the label-map to the data-model
		dataModel()->add(std::move(labelMapUnique));

		// Set the image and label map visible
		m_display->setVisibleData(DataList{selectedImage, m_labelMap});

		// Center the views
		for (auto v : m_display->views())
		{
			v->reset();
		}

		setupBrush(selectedImage);
		setupUndo();

		setupUI();
	}

	BrushApplication::~BrushApplication()
	{
		// some Qt versions may release the OpenGL context early, so we have to re-acquire it here
		GL::ContextManager::makeCurrent();
		// clean up before the ImFusion SDK is deinitalized
		for (auto v : m_display->views())
			v->setVisibleData({});
	}

	void BrushApplication::setupUI()
	{
		auto vLayout = new QVBoxLayout();
		auto controlsBox = new QGroupBox(this);
		controlsBox->setTitle("Brush Controls");
		controlsBox->setSizePolicy({QSizePolicy::Minimum, QSizePolicy::Fixed});


		// SpinBox to change the label-map value
		auto labelValueSpinBox = new QSpinBox(this);
		{
			auto hLayout = new QHBoxLayout();
			auto label = new QLabel("Label Value", this);
			labelValueSpinBox->setRange(0, 255);
			labelValueSpinBox->setValue(m_currentLabelValue);
			hLayout->addWidget(label);
			hLayout->addWidget(labelValueSpinBox);
			hLayout->setAlignment(Qt::AlignLeft);
			vLayout->addLayout(hLayout);
		}

		// Combo box to change the label-map color
		auto labelColorComboBox = new QComboBox(this);
		{
			QPixmap pMap{20, 20};
			pMap.fill(Qt::red);
			QIcon icon{};
			icon.addPixmap(pMap);
			labelColorComboBox->addItem(icon, "Red");

			pMap.fill(Qt::green);
			icon.addPixmap(pMap);
			labelColorComboBox->addItem(icon, "Green");

			pMap.fill(Qt::blue);
			icon.addPixmap(pMap);
			labelColorComboBox->addItem(icon, "Blue");

			labelColorComboBox->setCurrentText("Red");
			auto hLayout = new QHBoxLayout();
			auto label = new QLabel("Label Color", this);

			hLayout->addWidget(label);
			hLayout->addWidget(labelColorComboBox);
			hLayout->setAlignment(Qt::AlignLeft);
			vLayout->addLayout(hLayout);
		}

		// Slider to change the size of the brush cursor
		auto sizeSlider = new QSlider(Qt::Horizontal, this);
		auto sizeLabel = new QLabel("", this);
		{
			auto hLayout = new QHBoxLayout();
			hLayout->addWidget(new QLabel("Size", this));
			sizeSlider->setRange(0, 100);


			hLayout->addWidget(sizeSlider);
			hLayout->addWidget(sizeLabel);
			hLayout->setAlignment(Qt::AlignLeft);
			vLayout->addLayout(hLayout);
		}

		// Slider to change the adaptiveness of the brush
		auto adapSlider = new QSlider(Qt::Horizontal, this);
		auto adapLabel = new QLabel("", this);
		{
			auto hLayout = new QHBoxLayout();
			hLayout->addWidget(new QLabel("Adaptiveness", this));
			adapSlider->setRange(0, 100);
			hLayout->addWidget(adapSlider);
			hLayout->addWidget(adapLabel);
			hLayout->setAlignment(Qt::AlignLeft);

			vLayout->addLayout(hLayout);
		}

		// Slider to change the denoising magnitude
		auto denoiseSlider = new QSlider(Qt::Horizontal, this);
		auto denoiseLabel = new QLabel("", this);
		{
			auto hLayout = new QHBoxLayout();
			hLayout->addWidget(new QLabel("Image Noise Suppression", this));
			denoiseSlider->setRange(0, 100);
			denoiseSlider->setSingleStep(25);
			hLayout->addWidget(denoiseSlider);
			hLayout->addWidget(denoiseLabel);
			hLayout->setAlignment(Qt::AlignLeft);

			vLayout->addLayout(hLayout);
		}

		// Button to enable/disable the brush
		auto brushButton = new QPushButton(this);
		{
			brushButton->setCheckable(true);
			brushButton->setChecked(false);
			brushButton->setText("Brush");
			vLayout->addWidget(brushButton);
		}

		// Undo buttons
		auto undoButton = new QPushButton(this);
		auto redoButton = new QPushButton(this);
		{
			undoButton->setText("Undo");
			redoButton->setText("Redo");

			auto hLayout = new QHBoxLayout();
			hLayout->addWidget(undoButton);
			hLayout->addWidget(redoButton);
			vLayout->addLayout(hLayout);
		}

		controlsBox->setLayout(vLayout);

		auto externalLayout = new QVBoxLayout();
		externalLayout->addWidget(controlsBox);
		externalLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
		auto externalWidget = new QWidget();
		externalWidget->setLayout(externalLayout);

		QSplitter* splitter = new QSplitter(this);
		splitter->addWidget(externalWidget);
		splitter->addWidget(m_dispWrapper);
		splitter->setStretchFactor(0, 1);
		splitter->setStretchFactor(1, 10);
		this->setCentralWidget(splitter);

		this->setStyleSheet("background-color: #545454;");
		controlsBox->setStyleSheet("color: white;");

		// Setup the initial values of the UI elements
		sizeSlider->setValue(m_labelPainter->radiusMM());
		sizeLabel->setText(QString::number(m_labelPainter->radiusMM()) + " mm");

		denoiseSlider->setValue(m_labelPainter->filterInputIterations());
		denoiseLabel->setText(QString::number(m_labelPainter->filterInputIterations()));

		adapSlider->setValue(m_labelPainter->adaptiveness() * 100);
		adapLabel->setText(QString::number(m_labelPainter->adaptiveness()));

		// Setup connections to GUI elements
		QObject::connect(adapSlider, &QAbstractSlider::valueChanged, [this, adapLabel](int sliderVal) {
			adapLabel->setText(QString::number(sliderVal / 100.0));
			this->changeBrushAdaptiveness(sliderVal / 100.0);
			m_display->update();
		});
		QObject::connect(sizeSlider, &QAbstractSlider::valueChanged, [this, sizeLabel](int sizeMM) {
			sizeLabel->setText(QString::number(sizeMM) + " mm");
			this->changeBrushSize(sizeMM);
			m_display->update();
		});
		QObject::connect(labelColorComboBox, &QComboBox::currentTextChanged, this, &BrushApplication::changeLabelColor);
		QObject::connect(labelValueSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &BrushApplication::changeLabelValue);
		QObject::connect(denoiseSlider, &QAbstractSlider::valueChanged, [this, denoiseLabel](int imageFilterLevel) {
			denoiseLabel->setText(QString::number(imageFilterLevel));
			this->changeFilterLevel(imageFilterLevel);
			m_display->update();
		});
		QObject::connect(brushButton, &QPushButton::clicked, [this](bool checked) {
			if (checked)
				m_brush->enable();
			else
				m_brush->disable();
		});
		QObject::connect(undoButton, &QPushButton::clicked, [this](bool) { this->triggerUndo(); });
		QObject::connect(redoButton, &QPushButton::clicked, [this](bool) { this->triggerRedo(); });
	}

	void BrushApplication::triggerRedo()
	{
		if (!m_undoRecorder)
			return;
		if (m_currentUndoIndex + 1 == m_undoRecorder->data().size())
			return;
		++m_currentUndoIndex;

		const int newFocus = m_undoRecorder->restoreUndoLevel(m_currentUndoIndex);
		m_labelMap->setFocus(newFocus);

		m_display->update();
	}

	void BrushApplication::triggerUndo()
	{
		if (!m_undoRecorder)
			return;
		if (m_currentUndoIndex == 0)
			return;
		--m_currentUndoIndex;
		const int newFocus = m_undoRecorder->restoreUndoLevel(m_currentUndoIndex);
		m_labelMap->setFocus(newFocus);
		m_display->update();
	}

	void BrushApplication::setupViews()
	{    // create a new DisplayWidget and assign it to the QMainWindow
		// pass `false` to not initialize the DisplayWidget yet (we do this explicitly below)
		m_display = std::make_unique<DisplayWidgetMulti>(false);

		// Qt 5.9 changed some internals on how it creates a QWindow.
		// Older versions need to initialize the DisplayWidget *before* wrapping it in a QWidget,
		// otherwise there will be "CreateWindowEx failed (Cannot create a top-level child window)" errors.
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
		m_display->init();
#endif

		m_dispWrapper = QWidget::createWindowContainer(m_display.get(), this);
		m_dispWrapper->setSizePolicy({QSizePolicy::Maximum, QSizePolicy::Fixed});
		this->setMinimumSize(800, 600);

		// Qt 5.9 changed some internals on how it creates a QWindow.
		// Newer versions need to initialize the DisplayWidget *after* wrapping it in a QWidget,
		// otherwise there will be weird offsets in the rendering and in the mouse event positions.
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
		m_display->init();
#endif

		// add 2D default MPR and 3D view group and make them visible
		m_display->addViewGroup3D(false);
		for (auto v : m_display->views())
			v->setVisible(true);
	}

	void BrushApplication::setupBrush(SharedImageSet* selectedImage)
	{
		// Setup the painter and change some basic settings
		m_labelPainter = std::make_unique<Seg::LabelPainter>();
		m_labelPainter->setCurrent(selectedImage, m_labelMap);
		m_labelPainter->setLabel(m_currentLabelValue);
		m_labelPainter->setRadiusMM(20.0);
		m_labelPainter->setAdaptiveness(0.1f);

		// Setup the brush itself from the painter
		m_brush = std::make_unique<Seg::Brush>(*m_labelPainter, *m_display);
		m_brush->setShowCirclePreview(false);
		m_brush->setShowIsolinePreview(true);
		// use the same color for the preview (circle and isoline)
		m_brush->setPreviewColor(m_currentLabelColor);
	}

	void BrushApplication::setupUndo()
	{
		if (!m_labelMap)
			return;

		m_undoRecorder = std::make_unique<Seg::UndoRecorder>(*m_labelMap);

		// Trigger recording of an undo level when painting stopped
		m_brush->signalPaintingStopped.connect([this]() {
			++m_currentUndoIndex;
			m_undoRecorder->recordUndoLevel(
				"Paint action", std::chrono::system_clock::now().time_since_epoch().count(), {(size_t)m_labelMap->focus()});
		});

		// Record an initial undo level with the original label map, without any changes (empty in this case)
		m_undoRecorder->recordUndoLevel("Data Loaded", std::chrono::system_clock::now().time_since_epoch().count());
	}

	void BrushApplication::changeLabelValue(int value)
	{
		m_labelPainter->setLabel(value);
		m_currentLabelValue = value;
		if (auto component = m_labelMap->components().get<LabelDataComponent>())
		{
			component->setLabelColor(m_currentLabelValue, m_currentLabelColor);
		}

		m_display->update();
	}

	void BrushApplication::changeLabelColor(const QString& text)
	{
		vec4 color{};
		if (text == "Red")
			color = vec4{1, 0, 0, 1};
		else if (text == "Green")
			color = vec4{0, 1, 0, 1};
		else    // (text == "Blue")
			color = vec4{0, 0, 1, 1};

		m_brush->setPreviewColor(color);
		if (auto component = m_labelMap->components().get<LabelDataComponent>())
		{
			component->setLabelColor(m_currentLabelValue, color);
		}

		m_display->update();
	}

	void BrushApplication::changeBrushSize(int sizeMM) { m_labelPainter->setRadiusMM(sizeMM); }

	void BrushApplication::changeBrushAdaptiveness(float adaptiveness) { m_labelPainter->setAdaptiveness(adaptiveness); }

	void BrushApplication::changeFilterLevel(int imageFilterLevel)
	{
		m_labelPainter->setFilterInput(imageFilterLevel > 0);
		m_labelPainter->setFilterInputIterations(imageFilterLevel);
	}

}
