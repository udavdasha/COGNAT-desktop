#include <QApplication>
#include <QFontMetricsF>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QMapIterator>
#include <QMultiMap>
#include "bottomwidget.h"
#include "coloritem.h"
#include "database.h"
#include "mainwidget.h"

BottomWidget::BottomWidget(MainWidget *parent)
    : QGraphicsView(parent)
{
    rowHeight = 0;
    architectureMaxWidth = 0;

    schemeScene = new QGraphicsScene(this);
    setScene(schemeScene);
    setAlignment(Qt::AlignLeft);

    this->parent = parent;
}

void BottomWidget::setSchemeOutput(Database *database)
{
    QMultiMap<int, QString> architectureMultiMap;
    foreach (const QString &architecture, quantityMap.keys()) {
        architectureMultiMap.insert(quantityMap[architecture], architecture);
    }

    int y = rowHeight;

    QMapIterator<int, QString> iterator(architectureMultiMap);
    iterator.toBack();
    while (iterator.hasPrevious()) {

        iterator.previous();
        int quantity = iterator.key();
        QString architecture = iterator.value();

        QFontMetricsF metrics = qApp->font();
        QString number = QString("  #") + QString::number(quantity);
        int quantityWidth = metrics.boundingRect(number).width();
        int quantityHeight = metrics.boundingRect(number).height();
        int architectureWidth = widthMap[architecture];
        int x = architectureMaxWidth + metrics.boundingRect(architecture).width() + 50;
        QColor color = colorMap[architecture];
        QString description;
        if (parent->mode == MainWidget::Domains && database->descriptionWasLoaded(Database::Domains)) {
            description = database->descriptionOf(architecture, Database::Domains);
            if (description.isEmpty()) {
                description = QString("(no description)");
            } else {
                description += QString("  ");
            }
        } else if (parent->mode == MainWidget::COGs && database->descriptionWasLoaded(Database::COGs)) {
            description = database->descriptionOf(architecture, Database::COGs);
            if (description.isEmpty()) {
                description = QString("(no description)");
            } else {
                description += QString("  ");
            }
        }

        QGraphicsSimpleTextItem *quantityItem = new QGraphicsSimpleTextItem;
        quantityItem->setBrush(Qt::darkGray);
        quantityItem->setText(number);
        quantityItem->setPos(- quantityWidth, y - rowHeight / 2 - quantityHeight / 2);
        schemeScene->addItem(quantityItem);

        ColorItem *colorItem = new ColorItem(architectureWidth, rowHeight, this);
        colorItem->setData(ColorItem::Architecture, architecture);
        colorItem->setData(ColorItem::Color, color);
        colorItem->setPos(20, y);
        schemeScene->addItem(colorItem);

        QGraphicsSimpleTextItem *architectureItem = new QGraphicsSimpleTextItem;
        architectureItem->setText(architecture);
        architectureItem->setPos(architectureMaxWidth + 30, y - rowHeight / 2 - quantityHeight / 2);
        schemeScene->addItem(architectureItem);

        if (!description.isEmpty()) {
            QGraphicsSimpleTextItem *descriptionItem = new QGraphicsSimpleTextItem;
            descriptionItem->setBrush(Qt::darkGray);
            descriptionItem->setText(description);
            descriptionItem->setPos(x, y - rowHeight / 2 - quantityHeight / 2);
            schemeScene->addItem(descriptionItem);
        }

        y += rowHeight;
    }

    schemeScene->setSceneRect(schemeScene->itemsBoundingRect());
    ensureVisible(- 100500, 0, 0, 0);
}

void BottomWidget::setRowHeight(const int &height)
{
    rowHeight = height;
}

void BottomWidget::setArchitectureWidth(const QString &architecture, const int &width)
{
    if (!widthMap.contains(architecture)) {
        widthMap.insert(architecture, width);
    }
    if (width > architectureMaxWidth) {
        architectureMaxWidth = width;
    }
}

void BottomWidget::setArchitectureColor(const QString &architecture, const QColor &color)
{
    if (!colorMap.contains(architecture)) {
        colorMap.insert(architecture, color);
    }
}

void BottomWidget::increaseArchitectureQuantity(const QString &architecture)
{
    if (!quantityMap.contains(architecture)) {
        quantityMap.insert(architecture, 1);
    } else {
        ++quantityMap[architecture];
    }
}

void BottomWidget::createColorDialog(const QString &architecture, const QColor &initial) const
{
    parent->createColorDialog(architecture, initial);
}

void BottomWidget::clear()
{
    rowHeight = 0;
    architectureMaxWidth = 0;

    widthMap.clear();
    colorMap.clear();
    quantityMap.clear();

    schemeScene->clear();
}

QGraphicsScene *BottomWidget::getSchemeScene() const
{
    return schemeScene;
}
