#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QLineF>
#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include "gene.h"
#include "intergene.h"
#include "neighborhood.h"

Intergene::Intergene(const int &width, const int &height, Neighborhood *parent)
    :QGraphicsItem(parent)
{
    this->width = width;
    this->height = height;
    this->parent = parent;

    setFlags(ItemIsSelectable);
    setCursor(QCursor(Qt::PointingHandCursor));
}

QRectF Intergene::boundingRect() const
{
    return QRectF(- 1, - height / 6 - 1, width + 2, height / 3 + 2);
}

void Intergene::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    QPen pen(Qt::darkGray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    if (isSelected()) {
        pen.setColor(Qt::darkRed);
        pen.setWidth(2);
        pen.setStyle(Qt::DotLine);
    }
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    QLineF line(0, 0, width, 0);
    painter->drawLine(line);
}

void Intergene::mousePressEvent(QGraphicsSceneMouseEvent * /*event*/)
{
    parent->annotateIntergene(data(Gene::Gi).toString(), data(Gene::Start).toInt(), data(Gene::End).toInt());
}
