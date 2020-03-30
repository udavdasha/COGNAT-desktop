#include <QBrush>
#include <QColor>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QPolygonF>
#include <QStyleOptionGraphicsItem>
#include "bottomwidget.h"
#include "coloritem.h"

ColorItem::ColorItem(const int &width, const int &height, BottomWidget *parent)
    :QGraphicsItem(NULL)
{
    this->width = width;
    this->unit = height / 6;
    this->parent = parent;

    setCursor(QCursor(Qt::PointingHandCursor));
}

QRectF ColorItem::boundingRect() const
{
    return QRectF(- 1, - unit * 5 - 1, width + 2, unit * 5 + 2);
}

void ColorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QPolygonF arrow;
    arrow << QPointF(0, - unit)
    << QPointF(width - unit * 1.5, - unit)
    << QPointF(width - unit * 1.5, 0)
    << QPointF(width, - unit * 2.5)
    << QPointF(width - unit * 1.5, - unit * 5)
    << QPointF(width - unit * 1.5, - unit * 4)
    << QPointF(0, - unit * 4)
    << QPointF(0, - unit);

    QPainterPath path;
    path.addPolygon(arrow);

    QColor color = data(Color).value<QColor>();

    QBrush brush(color);

    QPen pen(Qt::darkGray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    painter->fillPath(path, brush);
    painter->setPen(pen);
    painter->drawPath(path);
}

void ColorItem::mousePressEvent(QGraphicsSceneMouseEvent * /*event*/)
{
    parent->createColorDialog(data(Architecture).toString(), data(Color).value<QColor>());
}
