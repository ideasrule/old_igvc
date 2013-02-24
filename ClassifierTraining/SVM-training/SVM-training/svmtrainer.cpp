#include "svmtrainer.h"
#include <QFileDialog>
#include <string>
#include <vector>
#include <cv.h>
#include "highgui.h"
#include "trainData.h"

#include <fstream>
using std::vector;

SVMTrainer::SVMTrainer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	svmDataFileName = tr("");

	regionWidth = 7;     //how many dimensions you want your vectors to be (regionWidth * regionHeight) (x3 for rbg)
	regionHeight = 7;

	hasImageFolderDir = false;
	hasTrainedFilePath = false;
	isTraining = false;
	isPredicting = false;
	hasOverlay = false;
	predictModel = 0;
	currImageIndex = -1;
	trainedFile = 0;
	trainedCount = 0;

    this->connect(ui.nextButton, SIGNAL(clicked()), this, SLOT(next()));
    this->connect(ui.prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    this->connect(ui.startButton, SIGNAL(clicked()), this, SLOT(startTraining()));
    this->connect(ui.stopButton, SIGNAL(clicked()), this, SLOT(stopTraining()));
	this->connect(ui.pictureLabel, SIGNAL(mouseOver(int, int)), this, SLOT(mouseOverImage(int, int)));
	this->connect(ui.actionSelect_image_folder, SIGNAL(triggered()), this, SLOT(selectFolder()));
	this->connect(ui.actionNew_file, SIGNAL(triggered()), this, SLOT(newFile()));
	this->connect(ui.actionChoose_Data_File, SIGNAL(triggered()), this, SLOT(selectSVMTrainingFile()));
	this->connect(ui.pictureLabel, SIGNAL(clicked(int, int, Qt::MouseButton)), this,
		SLOT(addRegionButtonPressed(int, int , Qt::MouseButton)));
	this->connect(ui.svmPredictCheckBox, SIGNAL(stateChanged(int)), this, SLOT(svmPredictStateChanged(int)));
	this->connect(ui.svmPredictOverlayCheckBox, SIGNAL(stateChanged(int)), this, SLOT(svmPredictOverlayStateChanged(int)));

}

SVMTrainer::~SVMTrainer()
{
	if (trainedFile && trainedFile->is_open())
		trainedFile->close();
}

void SVMTrainer::next()
{
	currImageIndex++;
	if (currImageIndex == imageFileList.size()-1) {
		ui.nextButton->setEnabled(false);
	}
	ui.prevButton->setEnabled(true);

	loadImage(imageFileList.at(currImageIndex));

}

void SVMTrainer::prev()
{
	currImageIndex--;
	if (currImageIndex == 0) {
		ui.prevButton->setEnabled(false);
	}
	ui.nextButton->setEnabled(true);

	loadImage(imageFileList.at(currImageIndex));
}

void SVMTrainer::startTraining()
{
	if (!hasImageFolderDir || !hasTrainedFilePath)
		return;
	isTraining = true;
	ui.startButton->setEnabled(false);
	ui.stopButton->setEnabled(true);
}

void SVMTrainer::stopTraining()
{
	if (!isTraining)
		return;
	isTraining = false;
	ui.stopButton->setEnabled(false);
	ui.startButton->setEnabled(true);
}


void SVMTrainer::addTrainingData(int r, int c, int attribute)
{
	*trainedFile << attribute << '\t';
	for (int i = 0; i < regionHeight; ++i) {
		for (int j = 0; j < regionWidth; ++j) {
			int c2 = c - regionWidth/2 + j;
			int r2 = r - regionHeight/2 + i;

			int blue = currImage[r2][c2].val[0];
			int green = currImage[r2][c2].val[1];
			int red = currImage[r2][c2].val[2];

			*trainedFile << red << '\t' << blue << '\t' << green << '\t';
		}
	}
	//int red = currImage[r][c].val[2];
	//int blue = currImage[r][c].val[0];
	//int green = currImage[r][c].val[1];
	//*trainedFile << red << '\t' << blue << '\t' << green;
	*trainedFile << '\n';
	trainedFile->flush();
}
//

void SVMTrainer::addRegionButtonPressed(int r, int c, Qt::MouseButton button)
{
	if (!isTraining)
		return;

	if (c < regionWidth / 2 || c + regionWidth/2 >= currImage.cols) 
		return;
	
	if (r < regionHeight / 2 || r + regionHeight/2 >= currImage.rows) 
		return;

	if (button == Qt::LeftButton) {
		addTrainingData(r, c, 1);
		trainedCount++;
	} else if (button == Qt::RightButton) {
		addTrainingData(r, c, -1);
		trainedCount++;
	}

	char txt[10];
	sprintf_s(txt, 10, "%d\0", trainedCount);
	ui.trainedCountLabel->setText(txt);

	//if (button == Qt::LeftButton)
	//	*trainedFile << "left\n";
	//else if (button == Qt::RightButton)
	//	*trainedFile << "right\n";
}

void SVMTrainer::newFile()
{

	if (trainedFile) {
		if (trainedFile->is_open()) {
			trainedFile->close();
			delete trainedFile;
		}
		trainedFile = 0;
	}

	QString trainedFileName = QFileDialog::getSaveFileName(this, tr("Choose file to store training data"), "C:\\IGVC\\Logs",
		"*.dat");
	ui.writeToLabel->setText(trainedFileName);
	trainedFileName.replace(QString("/"), QString("\\"));
	trainedFile = new std::ofstream(trainedFileName.toLocal8Bit().data(), std::ios::out);
	hasTrainedFilePath = true;
}

void SVMTrainer::selectFolder()
{
	QString imageFolderName = QFileDialog::getExistingDirectory(this, tr("Choose Image Directory"), "C:\\IGVC\\Logs",
		QFileDialog::ShowDirsOnly);

	imageFolderDir.setPath(imageFolderName);
	hasImageFolderDir = true;

	QStringList filters;
	filters << "*.png" << "*.jpg";
	imageFileList = imageFolderDir.entryList(filters);

	ui.imagePathLabel->setText(imageFolderDir.path());
	
	currImageIndex = -1;
	if (imageFileList.size() > 0) {
		currImageIndex = 0;
		loadImage(imageFileList.at(currImageIndex));

		if (currImageIndex < imageFileList.size() - 1)
			ui.nextButton->setEnabled(true);
	} else {
		ui.nextButton->setEnabled(false);
	}
	ui.prevButton->setEnabled(false);

}

void SVMTrainer::selectSVMTrainingFile()
{
	QString dataFileName = QFileDialog::getOpenFileName(this, tr("Choose Data File"), "C:\\IGVC\\Logs", "Training Data (*.dat)");
	QString resultFileName = QFileDialog::getSaveFileName(this, tr("Choose file to store SVM result"), "C:\\IGVC\\Logs",
		"*.svm");

	ui.statusBar->showMessage(tr("Training started..."));

	SVM::train(dataFileName.toLocal8Bit().data(), resultFileName.toLocal8Bit().data());

	ui.statusBar->showMessage(tr("Training Complete."));
	return;
}

void SVMTrainer::loadImage(QString fileName)
{
	if (!hasImageFolderDir) 
		return;

	QString fullPath = imageFolderDir.path() + QString(tr("/")) + fileName;
	fullPath.replace(QString("/"), QString("\\\\"));

	std::string temp = fullPath.toLocal8Bit().data();	//toStdString function is buggy
	currImage = cv::imread(temp);

	cv::Mat_<cv::Vec3b> currImageOverlay(currImage.rows, currImage.cols);
	currImage.copyTo(currImageOverlay);

	if (hasOverlay) {
		for (int r = regionHeight/2; r < currImage.rows-regionHeight/2; ++r) {
			for (int c = regionWidth/2; c < currImage.cols-regionWidth/2; ++c) {
				double classification = predictPixel(r, c);
				if (classification > 0) {
					currImageOverlay[r][c] = cv::Vec3b(0, 255, 0);
				}
			}
		}
	}

	currImageOverlay.copyTo(currImage);

	// adapted from http://www.qtcentre.org/threads/11655-OpenCV-integration
	cv::Mat rgb = currImage;
	if (rgb.data == 0)	//image could not be read
		return;
	std::vector<cv::Mat> channels;
	cv::split(rgb, channels);  // split the image into r, g, b channels
	cv::Mat alpha = cv::Mat_<uchar>(currImage.size());
	alpha.setTo(cv::Scalar(255));
	channels.push_back(alpha);
	cv::Mat rgba = cv::Mat_<cv::Vec4b>(currImage.size());
	cv::merge(channels, rgba);  // add an alpha (so the image is r, g, b, a
	IplImage iplImage = (IplImage) rgba;  // get the raw bytes

    size_t len = iplImage.height*iplImage.widthStep + 1;
    unsigned char *imageCopy = new unsigned char[len];
    memcpy(imageCopy, iplImage.imageData, len);
	QImage qimage(imageCopy, iplImage.width, iplImage.height, iplImage.widthStep,
		QImage::Format_RGB32); // and convert to a QImage

	ui.pictureLabel->setImage(qimage);
}


static QString xyToString(int x, int y)
{
    return "(" + QString::number(x) + ", "
        + QString::number(y) + ")";
}

double SVMTrainer::predictPixel(int r, int c)
{
	vector<struct svm_node> nodeList;

	for (int i = 0; i < regionHeight; ++i) {
		for (int j = 0; j < regionWidth; ++j) {
			int c2 = c - regionWidth/2 + j;
			int r2 = r - regionHeight/2 + i;

			int blue = currImage[r2][c2].val[0];
			int green = currImage[r2][c2].val[1];
			int red = currImage[r2][c2].val[2];	

			struct svm_node nodeR;
			struct svm_node nodeB;
			struct svm_node nodeG;
			nodeR.index = nodeList.size()+1;
			nodeR.value = red;
			nodeB.index = nodeList.size()+2;
			nodeB.value = blue;
			nodeG.index = nodeList.size()+3;
			nodeG.value = green;
			nodeList.push_back(nodeR);
			nodeList.push_back(nodeB);
			nodeList.push_back(nodeG);
		}
	}	

	//struct svm_node nodeR;
	//nodeR.index = 1;
	//nodeR.value = currImage[r][c].val[2];
	//struct svm_node nodeB;
	//nodeB.index = 2;
	//nodeB.value = currImage[r][c].val[0];
	//struct svm_node nodeG;
	//nodeG.index = 3;
	//nodeG.value = currImage[r][c].val[1];
	//nodeList.push_back(nodeR);
	//nodeList.push_back(nodeB);
	//nodeList.push_back(nodeG);
	struct svm_node nodeNull;
	nodeNull.index = -1;
	nodeList.push_back(nodeNull);

	if (nodeList.size() != 148)  //on image edge
		return -1.0;

	return svm_predict(predictModel, &nodeList[0]);
}

void SVMTrainer::mouseOverImage(int r, int c)
{
	//ui.writeToLabel->setText(xyToString(r, c));

	const int blockWidth = 15;
	const int blockHeight = 15;

	ui.regionLabel->resize(blockWidth*regionWidth, blockHeight*regionHeight);

	if (c < regionWidth / 2 || c + regionWidth/2 >= currImage.cols) {
		return;
	}

	if (r < regionHeight / 2 || r + regionHeight/2 >= currImage.rows) {
		return;
	}
	
	ui.regionLabel->resize(blockWidth*regionWidth, blockHeight*regionHeight);
	QPixmap qpixmap(blockWidth*regionWidth, blockHeight*regionHeight);

	QPainter p(&qpixmap);

	for (int i = 0; i < regionHeight; ++i) {
		for (int j = 0; j < regionWidth; ++j) {
			int c2 = c - regionWidth/2 + j;
			int r2 = r - regionHeight/2 + i;

			int blue = currImage[r2][c2].val[0];
			int green = currImage[r2][c2].val[1];
			int red = currImage[r2][c2].val[2];

			QColor color = QColor(red, green, blue);
			p.setBrush(color);
			//p.drawRect(0, 0, 50, 50);

			p.drawRect(j*blockWidth, i*blockHeight, blockWidth, blockHeight);
		}
	}

	ui.regionLabel->setPixmap(qpixmap);

	//predict with SVM
	if (isPredicting) {
		double classification = predictPixel(r, c);
		if (classification > 0) 
			ui.svmResultLabel->setText("+1");
		else
			ui.svmResultLabel->setText("-1");
	}

}

void SVMTrainer::svmPredictStateChanged(int state)
{
	if (state == Qt::Checked) {
		//unchecked->checked
		QString svmModelFileName = QFileDialog::getOpenFileName(this, tr("Choose SVM Model File"), "C:\\IGVC\\Logs", "SVM Model (*.svm)");
		predictModel = svm_load_model(svmModelFileName.toLocal8Bit().data());
		if (!predictModel) {
			ui.svmPredictCheckBox->setCheckState(Qt::Unchecked);
			isPredicting = false;
			return;
		}
		ui.svmPredictFileLabel->setText(svmModelFileName);
		isPredicting = true;
		ui.svmPredictOverlayCheckBox->setEnabled(true);

	} else {
		//checked->unchecked
		if (predictModel)
			svm_free_model_content(predictModel);
		ui.svmPredictFileLabel->setText("");
		isPredicting = false;
		ui.svmPredictOverlayCheckBox->setEnabled(false);
	}
}

void SVMTrainer::svmPredictOverlayStateChanged(int state)
{
	if (state == Qt::Checked)
		hasOverlay = true;
	else
		hasOverlay = false;
}