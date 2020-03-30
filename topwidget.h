#ifndef TOPWIDGET_H
#define TOPWIDGET_H

#include <QPoint>
#include <QString>
#include <QWidget>

class QGraphicsView;
class QMouseEvent;
class QSortFilterProxyModel;
class QSplitter;
class QTableWidget;
class QTextBrowser;
class QTreeView;
class BottomWidget;
class Database;
class GraphicsScene;
class MainWidget;
class TaxonomyModel;

class TopWidget : public QWidget
{
    Q_OBJECT

public:
    TopWidget(MainWidget *parent);

    void setTableOutput(Database *database);
    void setTextOutput(Database *database);
    void setTreeOutput(Database *database);
    void markGenesOfMembraneProteins(bool checked); // 3.2 ###
    void showIntergenes(bool checked, const int &radius); // 3.4 ###
    void colorByDomainArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.1
    void colorByCOGArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.2
    void colorByDomains(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.3
    void colorByCOGs(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget); // 3.5.4
    void setNeighborhoodRadius(const int &radius); // 3.8
    void annotateGene(const QString &gi, const int &y) const;
    void annotateDomain(const QString &gi, const int &number, const int &y) const;
    void annotateIntergene(const QString &gi, const int &start, const int &end) const;
    void createContextMenu(const QPoint &point) const;

    GraphicsScene *getGraphicsScene() const;

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    void addRows(Database *database);

    int calculateTableWidth() const;
    int calculateTableHeight() const;

    QTableWidget *tableWidget;
    QTableWidget *headerWidget;

    QTextBrowser *textBrowser;

    TaxonomyModel *taxonomyModel;
    QSortFilterProxyModel *proxyModel;
    QTreeView *treeView;

    GraphicsScene *graphicsScene;
    QGraphicsView *graphicsView;

    QSplitter *mainSplitter;

    QWidget *rightWidget;

    MainWidget *parent;
};

#endif
