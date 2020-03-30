#ifndef INTERGENE_H
#define INTERGENE_H

#include <QGraphicsItem>
#include <QRectF>

class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class Neighborhood;

class Intergene : public QGraphicsItem
{
public:
    Intergene(const int &width, const int &height, Neighborhood *parent);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    int width;
    int height;

    Neighborhood *parent;
};

#endif
