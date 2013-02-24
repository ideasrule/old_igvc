#include "imagelabel.h"

ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent)
{
    ui.setupUi(this);
}

void ImageLabel::setImage(QImage image)
{
    QPixmap pixmap = QPixmap::fromImage(image, 0);
    this->setPixmap(pixmap);
    this->resize(image.width(), image.height());
}

ImageLabel::~ImageLabel()
{
}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    // emit row, col
    emit mouseOver(event->y(), event->x());
}
