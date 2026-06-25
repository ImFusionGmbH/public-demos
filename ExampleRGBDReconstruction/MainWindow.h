#pragma once

#include <ImFusion/RGBD/Data/RGBDStream.h>
#include <ImFusion/RGBD/RGBDReconstructionAlgorithm.h>

#include <QtWidgets/QMainWindow>

class QLabel;
using namespace ImFusion;
/// Main window of RGBDReconstruction application
class MainWindow : public QMainWindow, public SignalReceiver
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

public slots:
	void onStartReconstruction();
	void onStopReconstruction();

public:
	void onStreamData(std::shared_ptr<const StreamData> streamData);

private:
	QLabel* m_imgLabel;
	std::unique_ptr<RGBDReconstructionAlgorithm> m_reco;
	std::unique_ptr<RGBDStream> m_stream;
};
