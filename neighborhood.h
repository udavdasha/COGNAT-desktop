#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include <QColor>
#include <QGraphicsItem>
#include <QList>
#include <QPoint>
#include <QRectF>
#include <QString>
#include "domain.h"

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class BottomWidget;
class Database;
class GraphicsScene;

class Neighborhood : public QGraphicsItem
{
public:
    Neighborhood(const QString &gi, const int &height, Database *database, GraphicsScene *parent);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void markGenesOfMembraneProteins(bool checked); // 3.2 ###
    void showIntergenes(bool checked, const int &radius); // 3.4 ###
    void colorByDomainArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.1
    void colorByCOGArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.2
    void colorByDomains(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.3
    void colorByCOGs(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.4
    void setRadius(const int &radius); // 3.8
    void annotateGene(const QString &gi, const int &y) const;
    void annotateDomain(const QString &gi, const int &number, const int &y) const;
    void annotateIntergene(const QString &gi, const int &start, const int &end) const;
    void createContextMenu(const QPoint &point) const;

    int getMinX() const;
    int getMinVisibleX() const;
    int getMaxVisibleX() const;

    QString getBaseGi() const;

private:
    void createArchitecture(const QList<Domain> &domains, QGraphicsItem *item, BottomWidget *bottomWidget);
    void writeColorToSettings(const QString &architecture, const QColor &color);

    int calculateStartOf(const int &i, const QList<Domain> &domains) const;
    int calculateEndOf(const int &i, const QList<Domain> &domains) const;

    QColor manageColor(const int &i, const QList<Domain> &domains, const int &width, BottomWidget *bottomWidget);
    QColor readColorFromSettings(const QString &architecture);
    QColor randomColor() const;

    QString baseGi;

    int height;
    int minX;
    int minVisibleX;
    int maxVisibleX;

    Database *database;

    GraphicsScene *parent;
};

#endif
