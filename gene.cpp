#include <QBrush>
#include <QColor>
#include <QCursor>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QPolygonF>
#include <QStyleOptionGraphicsItem>
#include "gene.h"
#include "graphicsscene.h"
#include "neighborhood.h"

Gene::Gene(const int &width, const int &height, const int &strand, Neighborhood *parent)
    :QGraphicsItem(parent)
{
    this->width = width;
    this->unit = height / 6;
    this->strand = strand;
    this->parent = parent;

    setFlags(ItemIsSelectable);
    setCursor(QCursor(Qt::PointingHandCursor));
}

QRectF Gene::boundingRect() const
{
    return QRectF(- 1, - unit * 5 - 1, width + 2, unit * 5 + 2);
}

void Gene::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QPolygonF polygon;
    if (strand > 0) {
        polygon << QPointF(0, - unit)
        << QPointF(width - unit * 1.5, - unit)
        << QPointF(width - unit * 1.5, 0)
        << QPointF(width, - unit * 2.5)
        << QPointF(width - unit * 1.5, - unit * 5)
        << QPointF(width - unit * 1.5, - unit * 4)
        << QPointF(0, - unit * 4)
        << QPointF(0, - unit);
    } else if (strand < 0) {
        polygon << QPointF(unit * 2, 0)
        << QPointF(unit * 2, - unit)
        << QPointF(width, - unit)
        << QPointF(width, - unit * 4)
        << QPointF(unit * 2, - unit * 4)
        << QPointF(unit * 2, - unit * 5)
        << QPointF(0, - unit * 2.5)
        << QPointF(unit * 2, 0);
    } else {
        polygon << QPointF(0, - unit)
        << QPointF(width, - unit)
        << QPointF(width, - unit * 4)
        << QPointF(0, - unit * 4)
        << QPointF(0, - unit);
    }

    QPainterPath path;
    path.addPolygon(polygon);

    QColor color = data(Color).value<QColor>();

    QBrush brush(color);
    if (data(IsMarked).toBool()) {
        brush.setStyle(Qt::Dense3Pattern);
    }

    QPen pen(Qt::darkGray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    if (isSelected()) {
        pen.setColor(Qt::darkRed);
        pen.setWidth(2);
        pen.setStyle(Qt::DotLine);
    }

    painter->fillPath(path, brush);
    painter->setPen(pen);
    painter->drawPath(path);
}

void Gene::mousePressEvent(QGraphicsSceneMouseEvent * /*event*/)
{
    if (data(IsGene).toBool()) {
        parent->annotateGene(data(Gi).toString(), data(Y).toInt());
    } else if (data(IsDomain).toBool()) {
        parent->annotateDomain(data(Gi).toString(), data(Number).toInt(), data(Y).toInt());
    }
}

void Gene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    parent->createContextMenu(event->screenPos());
}
