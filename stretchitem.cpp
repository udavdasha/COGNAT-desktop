#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "stretchitem.h"

StretchItem::StretchItem(const int &height, QGraphicsItem *parent)
    :QGraphicsItem(parent)
{
    this->height = height;
}

QRectF StretchItem::boundingRect() const
{
    return QRectF(0, - height, height, height);
}

void StretchItem::paint(QPainter * /*painter*/, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
}
