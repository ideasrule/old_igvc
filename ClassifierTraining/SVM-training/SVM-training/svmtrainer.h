#ifndef MYCLASS_H
#define MYCLASS_H

#include <QtGui/QMainWindow>
#include "ui_myclass.h"
#include <cv.h>
#include <fstream>
#include "svm.h"

class SVMTrainer : public QMainWindow
{
	Q_OBJECT

public:
	SVMTrainer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SVMTrainer();

private slots:
	void next();
	void prev();
	void startTraining();
	void stopTraining();
	void mouseOverImage(int x, int y);
	void selectFolder();
	void newFile();
	void selectSVMTrainingFile();
	void svmPredictStateChanged(int state);
	void svmPredictOverlayStateChanged(int state);
	void addRegionButtonPressed(int x, int y, Qt::MouseButton button);

private:
	Ui::MyClassClass ui;

	int regionWidth;  //must be odd
	int regionHeight;

	QString svmDataFileName;
	QDir imageFolderDir;
	QStringList imageFileList;
	bool hasImageFolderDir;
	bool hasTrainedFilePath;
	bool isTraining;
	bool isPredicting;
	bool hasOverlay;
	int trainedCount;
	std::ofstream *trainedFile;
	struct svm_model *predictModel;

	int currImageIndex;
	cv::Mat_<cv::Vec3b> currImage;

	void loadImage(QString fileName);
	void addTrainingData(int x, int y, int attribute);
	double predictPixel(int r, int c);

};

#endif // MYCLASS_H
