#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QPoint>
#include <QString>
#include <QVector>

class QGraphicsSimpleTextItem;
class QTableWidget;
class QTreeView;
class BottomWidget;
class Database;
class Neighborhood;
class TopWidget;

class GraphicsScene : public QGraphicsScene
{
public:
    GraphicsScene(QTableWidget *tableWidget, Database *database, TopWidget *parent);
    GraphicsScene(const int &stretch, QTreeView *treeView, Database *database, TopWidget *parent);

    void markGenesOfMembraneProteins(bool checked); // 3.2 ###
    void showIntergenes(bool checked, const int &radius); // 3.4 ###
    void colorByDomainArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.1
    void colorByCOGArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.2
    void colorByDomains(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.3
    void colorByCOGs(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.4
    void setNeighborhoodRadius(const int &radius, QTableWidget *tableWidget); // 3.8
    void setNeighborhoodRadius(const int &radius, QTreeView *treeView); // 3.8
    void annotateGene(const QString &gi, const int &y) const;
    void annotateDomain(const QString &gi, const int &number, const int &y) const;
    void annotateIntergene(const QString &gi, const int &start, const int &end) const;
    void createContextMenu(const QPoint &point) const;

    int getMaxHeight() const;

private:
    int width;
    int height;
    int minVisibleX;
    int maxVisibleX;
    int maxHeight;
    int stretch;

    QVector<Neighborhood *> neighborhoods;
    QVector<QGraphicsSimpleTextItem *> labels;

    QTableWidget *tableWidget;

    QTreeView *treeView;

    Database *database;

    TopWidget *parent;
};

#endif
