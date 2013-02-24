#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QtGui>
#include <QtGui/QLabel>

class QImageLabel : public QLabel
{
    Q_OBJECT

public:
    QImageLabel(QWidget *parent = 0);
    ~QImageLabel();

    void setImage(QImage image);

signals:
    void mouseOver(int row, int col);
	void clicked(int row, int col, Qt::MouseButton button);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);

};

#endif // IMAGELABEL_H
