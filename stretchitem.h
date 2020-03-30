#ifndef STRETCHITEM_H
#define STRETCHITEM_H

#include <QGraphicsItem>
#include <QRectF>

class QPainter;
class QStyleOptionGraphicsItem;

class StretchItem : public QGraphicsItem
{
public:
    StretchItem(const int &height = 0, QGraphicsItem *parent = NULL);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    int height;
};

#endif
