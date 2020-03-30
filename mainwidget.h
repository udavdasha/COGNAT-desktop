#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QColor>
#include <QPoint>
//#include <QSizeF>
#include <QString>
#include <QWidget>

class QSplitter;
class COGNAT;
class BottomWidget;
class Database;
class RightWidget;
class TopWidget;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(Database *database, COGNAT *parent);

    enum Mode { DomainArchitectures, COGArchitectures, Domains, COGs };

    Mode mode;

    void setTableTopWidget();
    void setTextTopWidget();
    void setTreeTopWidget();
    void setRadius(const int &radius);
    void setMinimumScore(const int &minscore);
    void setMinimumLength(const int &minlength);
    void setMaximumOverlapping(const int &maxoverlap);
    void setTrustworthyScore(const int &trustscore);
    void markGenesOfMembraneProteins(bool checked); // 3.2 ###
    void showIntergenes(bool checked); // 3.4 ###
    void colorByDomainArchitectures(); // 3.5.1
    void colorByCOGArchitectures(); // 3.5.2
    void colorByDomains(); // 3.5.3
    void colorByCOGs(); // 3.5.4
    void setNeighborhoodRadius(); // 3.8
    void annotateGene(const QString &gi, const int &y);
    void annotateDomain(const QString &gi, const int &number, const int &y);
    void annotateIntergene(const QString &gi, const int &start, const int &end);
    void deleteAnnotation();
    void createContextMenu(const QPoint &point) const;
    void createColorDialog(const QString &architecture, const QColor &initial) const;

    //bool savePdf(const QString &fileName, const QSizeF &size = QSizeF());
    bool savePdf(const QString &fileName);

    QString currentGi() const;
    QString currentArchitecture() const;

private slots:
    void saveSplitterState(int pos, int index);

private:
    void readSettings();
    void writeSettings();

    int radius;
    int minscore;
    int minlength;
    int maxoverlap;
    int trustscore;

    BottomWidget *bottomWidget;

    RightWidget *rightWidget;

    TopWidget *topWidget;

    QSplitter *leftSplitter;
    QSplitter *mainSplitter;

    Database *database;

    COGNAT *parent;
};

#endif
