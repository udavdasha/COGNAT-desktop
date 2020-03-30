#ifndef GENE_H
#define GENE_H

#include <QGraphicsItem>
#include <QRectF>

class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class Neighborhood;

class Gene : public QGraphicsItem
{
public:
    Gene(const int &width, const int &height, const int &strand, Neighborhood *parent);

    enum Key { Gi, Number, IsIntergene, IsGene, IsDomain, IsMarked, Color, Start, End, Y };

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    int width;
    int unit;
    int strand;

    Neighborhood *parent;
};

#endif
