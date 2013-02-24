#include "ImageViewer.h"
#include <QtGui>

//#include <boost/thread.hpp>

#include <cv.h>
#include <highgui.h>

static int openWindow(const cv::Mat_<cv::Vec3b>& image)
{
	char *argv[1] = { 0 };
	int argc = 0;
	QApplication app(argc, argv);
	QWidget *window = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout;

	// adapted from http://www.qtcentre.org/threads/11655-OpenCV-integration
	cv::Mat rgb = image;
	std::vector<cv::Mat> channels;
	cv::split(rgb, channels);  // split the image into r, g, b channels
	cv::Mat alpha = cv::Mat_<uchar>(image.size());
	alpha.setTo(cv::Scalar(255));
	channels.push_back(alpha);
	cv::Mat rgba = cv::Mat_<cv::Vec4b>(image.size());
	cv::merge(channels, rgba);  // add an alpha (so the image is r, g, b, a
	IplImage iplImage = (IplImage) rgba;  // get the raw bytes
	const unsigned char *imageData = (unsigned char *)(iplImage.imageData);
	QImage qimage(imageData, iplImage.width, iplImage.height, iplImage.widthStep,
		QImage::Format_RGB32); // and convert to a QImage

	QLabel *imageFrame = new QLabel;
	imageFrame->setPixmap(QPixmap::fromImage(qimage, 0));

	QPushButton *quit = new QPushButton(QString("Quit"));
    quit->setGeometry(62, 40, 75, 30);
    quit->setFont(QFont("Times", 18, QFont::Bold));
	window->connect(quit, SIGNAL(clicked()), &app, SLOT(quit()));

	layout->addWidget(new QLabel("top"));
	layout->addWidget(imageFrame);
	layout->addWidget(new QLabel("bottom"));
	layout->addWidget(quit);

	window->setLayout(layout);
	window->setWindowTitle(QString("Qt Image Display"));
	window->show();
	return app.exec();
}

ImageViewer::ImageViewer(void)
{
}

ImageViewer::~ImageViewer(void)
{
}

void ImageViewer::giveImage(const cv::Mat_<cv::Vec3b>& image)
{
	openWindow(image);
}
