#include "imageviewer.h"

#include <cv.h>
#include "highgui.h"

ImageViewer::ImageViewer(vector<shared_ptr<Frame>> frames, 
                         QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags), frames(frames), currentIndex(0)
{
    ui.setupUi(this);

    setFrame(frames[currentIndex]);

    this->connect(ui.nextButton, SIGNAL(clicked()), this, SLOT(next()));
    this->connect(ui.prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    this->connect(ui.imageLabel, SIGNAL(mouseOver(int, int)), this, SLOT(mouseOverImage(int, int)));
}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::next()
{
    if(currentIndex < frames.size() - 1)
    {
        currentIndex++;
        setFrame(frames[currentIndex]);
    }
}

void ImageViewer::prev()
{
    if(currentIndex > 0)
    {
        currentIndex--;
        setFrame(frames[currentIndex]);
    }
}

void ImageViewer::setFrame(shared_ptr<Frame> frm)
{
    const cv::Mat_<cv::Vec3b>& image = frm->color;

    cv::Point3f pt = frm->transformedCloud(3, 3);

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

    size_t len = iplImage.height*iplImage.widthStep + 1;
    unsigned char *imageCopy = new unsigned char[len];
    memcpy(imageCopy, iplImage.imageData, len);
	QImage qimage(imageCopy, iplImage.width, iplImage.height, iplImage.widthStep,
		QImage::Format_RGB32); // and convert to a QImage

    ui.imageLabel->setImage(qimage);
}

static QString xyToString(int x, int y)
{
    return "(" + QString::number(x) + ", "
        + QString::number(y) + ")";
}

static QString xyzToString(float x, float y, float z)
{
    return "(" + QString::number(x, 'g') + ", "
        + QString::number(y, 'g') + ", "
        + QString::number(z, 'g') + ")";
}

void ImageViewer::mouseOverImage(int r, int c)
{
	//Parameters used for search triangles
	const float TAN_THETA = 0.84f; // 40 degrees
	//#define TAN_THETA 2.0 // 80 degrees
	const float MIN_DISTANCE = 1.0f;

	//Compatibility constraints
	const float HMIN = 0.3f;//0.3
	const float HMAX = 0.5f;//0.5
	const float DEPTHMAX = 0.2f;

    if(ui.imageLabel->underMouse())
    {
        ui.rowcolValue->setText(xyToString(r, c));

        cv::Mat_<cv::Point3f> transformedCloud = frames[currentIndex]->transformedCloud;
        if(0 <= c && c < transformedCloud.size().width
            && 0 <= r && r < transformedCloud.size().height)
        {
            cv::Point3f p = transformedCloud(r, c);
            ui.xyzValue->setText(xyzToString(p.x, p.y, p.z));
			
			double fl = frames[currentIndex]->focalLength;
			double searchHeight = fl * HMAX / p.y / 2.0; 
			
			searchHeight = std::min(searchHeight, (double)r);
			double searchMinHeight = fl * HMIN / p.y / 2.0; 
			double searchBase = 2.0 * searchHeight / TAN_THETA;	

			ui.triangleValue->setText(xyzToString(searchMinHeight, searchHeight, searchBase));
		}

    }
    else
    {
        ui.rowcolValue->setText("");
        ui.xyzValue->setText("");
    }
}
