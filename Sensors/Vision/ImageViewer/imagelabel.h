#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QtGui>
#include "ui_imagelabel.h"

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    ImageLabel(QWidget *parent = 0);
    ~ImageLabel();

    void setImage(QImage image);

signals:
    void mouseOver(int row, int col);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::ImageLabel ui;
};

#endif // IMAGELABEL_H
