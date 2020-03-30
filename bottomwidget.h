#ifndef BOTTOMWIDGET_H
#define BOTTOMWIDGET_H

#include <QColor>
#include <QGraphicsView>
#include <QMap>
#include <QString>

class QGraphicsScene;
class Database;
class MainWidget;

class BottomWidget : public QGraphicsView
{
    Q_OBJECT

public:
    BottomWidget(MainWidget *parent);

    void setSchemeOutput(Database *database);
    void setRowHeight(const int &height);
    void setArchitectureWidth(const QString &architecture, const int &width);
    void setArchitectureColor(const QString &architecture, const QColor &color);
    void increaseArchitectureQuantity(const QString &architecture);
    void createColorDialog(const QString &architecture, const QColor &initial) const;
    void clear();

    QGraphicsScene *getSchemeScene() const;

private:
    int rowHeight;
    int architectureMaxWidth;

    QMap<QString, int> widthMap;
    QMap<QString, QColor> colorMap;
    QMap<QString, int> quantityMap;

    QGraphicsScene *schemeScene;

    MainWidget *parent;
};

#endif
