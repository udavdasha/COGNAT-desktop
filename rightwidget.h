#ifndef RIGHTWIDGET_H
#define RIGHTWIDGET_H

#include <QFont>
#include <QString>
#include <QTextCharFormat>
#include <QWidget>

class QTableWidget;
class Database;
class TextBrowser;
class MainWidget;

class RightWidget : public QWidget
{
    Q_OBJECT

public:
    RightWidget(MainWidget *parent = NULL);

    void annotateGene(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &y, Database *database);
    void annotateDomain(const int &number);
    void annotateIntergene(const QString &gi, const int &start, const int &end, Database *database);
    void saveState();
    void restoreState();

    int getCurrentY() const;

    QString getCurrentGi() const;
    QString getCurrentDomainArchitecture() const;
    QString getCurrentCOGArchitecture() const;

private slots:
    void saveSliderState(int value);

private:
    void readSettings();
    void writeSettings();

    int currentY;

    QString currentGi;
    QString currentDomainArchitecture;
    QString currentCOGArchitecture;

    QFont defaultFont;
    QFont sequenceFont;

    QTextCharFormat textFormat;

    QTableWidget *headerWidget;

    TextBrowser *textBrowser;

    MainWidget *parent;
};

#endif
