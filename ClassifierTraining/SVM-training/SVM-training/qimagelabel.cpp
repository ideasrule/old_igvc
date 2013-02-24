#include "qimagelabel.h"

QImageLabel::QImageLabel(QWidget *parent)
    : QLabel(parent)
{
	this->setMouseTracking(true);
}

void QImageLabel::setImage(QImage image)
{
    QPixmap pixmap = QPixmap::fromImage(image, 0);
    this->setPixmap(pixmap);
    this->resize(image.width(), image.height());
}

QImageLabel::~QImageLabel()
{
}


void QImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    // emit row, col
    emit mouseOver(event->y(), event->x());
}


void QImageLabel::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton || event->button() == Qt::LeftButton)
		emit clicked(event->y(), event->x(), event->button());

}