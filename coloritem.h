#ifndef COLORITEM_H
#define COLORITEM_H

#include <QGraphicsItem>
#include <QRectF>

class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class BottomWidget;

class ColorItem : public QGraphicsItem
{
public:
    ColorItem(const int &width, const int &height, BottomWidget *parent);

    enum Key { Architecture, Color };

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    int width;
    int unit;

    BottomWidget *parent;
};

#endif
