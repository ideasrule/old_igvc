#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QtGui>
#include <QtGui/QMainWindow>
#include "ui_imageviewer.h"

#include "Memory.h"
#include <vector>
using std::vector;

#include "Frame.h"
using Pave_Libraries_Camera::Frame;

#include <cv.h>

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer(vector<shared_ptr<Frame>> frames,
        QWidget *parent = 0, Qt::WFlags flags = 0);
    ~ImageViewer();

private slots:
    void next();
    void prev();
    void mouseOverImage(int x, int y);

private:
    void setFrame(shared_ptr<Frame> frm);

    Ui::ImageViewerClass ui;

    vector<shared_ptr<Frame>> frames;
    int currentIndex;
};

#endif // IMAGEVIEWER_H
