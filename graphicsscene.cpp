#include <QApplication>
#include <QFontMetricsF>
#include <QGraphicsSimpleTextItem>
#include <QModelIndex>
#include <QTableWidget>
#include <QTreeView>
#include "bottomwidget.h"
#include "database.h"
#include "graphicsscene.h"
#include "neighborhood.h"
#include "stretchitem.h"
#include "topwidget.h"

GraphicsScene::GraphicsScene(QTableWidget *tableWidget, Database *database, TopWidget *parent)
    :QGraphicsScene(parent)
{
    this->maxHeight = 0;
    this->tableWidget = tableWidget;
    this->database = database;
    this->parent = parent;

    int rowHeight = tableWidget->rowHeight(0);
    int padding = rowHeight / 12;
    int y = padding;

    StretchItem *topStretch = new StretchItem(padding);
    topStretch->setPos(0, y);
    addItem(topStretch);
    y += rowHeight * 5 / 6;

    QVector<QString> inputVector = database->getInputVector();
    foreach (const QString &gi, inputVector) {
        if (database->contains(gi)) {
            Neighborhood *neighborhood = new Neighborhood(gi, rowHeight, database, this);
            neighborhood->setPos(neighborhood->getMinX(), y);
            neighborhoods.append(neighborhood);
            addItem(neighborhood);
        } else {
            QGraphicsSimpleTextItem *unknown = new QGraphicsSimpleTextItem(gi);
            unknown->setBrush(Qt::darkGray);
            QFontMetricsF metrics = qApp->fontMetrics();
            unknown->setPos(0, y - rowHeight / 2 - metrics.boundingRect(gi).height() / 2 + padding / 2);
            addItem(unknown);
        }
        y += rowHeight;
    }
    y -= rowHeight;
    y += padding + tableWidget->contentsMargins().bottom();

    StretchItem *bottomStretch = new StretchItem(padding + tableWidget->contentsMargins().bottom());
    bottomStretch->setPos(0, y);
    addItem(bottomStretch);

    height = y;
    maxHeight = y;
    maxHeight += rowHeight;
    maxHeight += padding * 3;
    maxHeight += tableWidget->contentsMargins().top();
    maxHeight += tableWidget->contentsMargins().bottom();
}

GraphicsScene::GraphicsScene(const int &stretch, QTreeView *treeView, Database *database, TopWidget *parent)
    :QGraphicsScene(parent)
{
    this->maxHeight = 0;
    this->stretch = stretch;
    this->treeView = treeView;
    this->database = database;
    this->parent = parent;

    int rowHeight = 0;
    int collapsibleHeight = 0;
    int y = 0;

    QModelIndexList list = treeView->model()->match(QModelIndex(), Qt::UserRole, true, 1, Qt::MatchExactly);
    QModelIndex root = list.value(0);
    int domains = treeView->model()->rowCount(root);
    for (int i = 0; i < domains; ++i) {

        QModelIndex domain = treeView->model()->index(i, 0, root);
        if (i == 0) {
            rowHeight = treeView->sizeHintForIndex(domain).height();
            y += rowHeight - stretch;
            StretchItem *domainStretch = new StretchItem(rowHeight - stretch);
            domainStretch->setPos(0, y);
            addItem(domainStretch);
        } else {
            y += rowHeight;
            StretchItem *domainStretch = new StretchItem(rowHeight);
            domainStretch->setPos(0, y);
            addItem(domainStretch);
        }
        int phylums = treeView->model()->rowCount(domain);
        for (int j = 0; j < phylums; ++j) {

            QModelIndex phylum = treeView->model()->index(j, 0, domain);
            if (i == 0) {
                collapsibleHeight = treeView->sizeHintForIndex(phylum).height();
            }
            y += collapsibleHeight;
            StretchItem *phylumStretch = new StretchItem(collapsibleHeight);
            phylumStretch->setPos(0, y);
            addItem(phylumStretch);
            int classes = treeView->model()->rowCount(phylum);
            for (int k = 0; k < classes; ++k) {

                QModelIndex class_ = treeView->model()->index(k, 0, phylum);
                y += collapsibleHeight;
                StretchItem *classStretch = new StretchItem(collapsibleHeight);
                classStretch->setPos(0, y);
                addItem(classStretch);
                int sources = treeView->model()->rowCount(class_);
                for (int l = 0; l < sources; ++l) {

                    QModelIndex source = treeView->model()->index(l, 0, class_);
                    y += collapsibleHeight;
                    StretchItem *sourceStretch = new StretchItem(collapsibleHeight);
                    sourceStretch->setPos(0, y);
                    addItem(sourceStretch);
                    int gis = treeView->model()->rowCount(source);
                    for (int m = 0; m < gis; ++m) {

                        QModelIndex gene = treeView->model()->index(m, 0, source);
                        y += rowHeight;
                        QString gi = treeView->model()->data(gene, Qt::AccessibleTextRole).toString();
                        if (database->contains(gi)) {
                            Neighborhood *neighborhood = new Neighborhood(gi, rowHeight, database, this);
                            neighborhood->setPos(neighborhood->getMinX(), y);
                            neighborhoods.append(neighborhood);
                            addItem(neighborhood);
                        } else {
                            QGraphicsSimpleTextItem *unknown = new QGraphicsSimpleTextItem(gi);
                            unknown->setBrush(Qt::darkGray);
                            QFontMetricsF metrics = qApp->fontMetrics();
                            unknown->setPos(0, y - rowHeight / 2 - metrics.boundingRect(gi).height() / 2 + rowHeight / 24);
                            addItem(unknown);
                        }
                    }
                }
            }
        }
    }

    height = y;
    maxHeight = y;
    maxHeight += rowHeight;
    maxHeight += rowHeight / 12 * 3;
    maxHeight += treeView->contentsMargins().top();
    maxHeight += treeView->contentsMargins().bottom();
}

void GraphicsScene::markGenesOfMembraneProteins(bool checked) // 3.2 ###
{
    foreach (Neighborhood *neighborhood, neighborhoods) {
        neighborhood->markGenesOfMembraneProteins(checked);
    }
    update();
}

void GraphicsScene::showIntergenes(bool checked, const int &radius) // 3.4 ###
{
    foreach (Neighborhood *neighborhood, neighborhoods) {
        neighborhood->showIntergenes(checked, radius);
    }
}

void GraphicsScene::colorByDomainArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.1
{
    foreach (Neighborhood *neighborhood, neighborhoods) {
        neighborhood->colorByDomainArchitectures(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
    }
    update();
}

void GraphicsScene::colorByCOGArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.2
{
    foreach (Neighborhood *neighborhood, neighborhoods) {
        neighborhood->colorByCOGArchitectures(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
    }
    update();
}

void GraphicsScene::colorByDomains(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.3
{
    foreach (Neighborhood *neighborhood, neighborhoods) {
        neighborhood->colorByDomains(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
    }
    update();
}

void GraphicsScene::colorByCOGs(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.4
{
    foreach (Neighborhood *neighborhood, neighborhoods) {
        neighborhood->colorByCOGs(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
    }
    update();
}

void GraphicsScene::setNeighborhoodRadius(const int &radius, QTableWidget *tableWidget) // 3.8
{
    foreach (QGraphicsSimpleTextItem *label, labels) {
        removeItem(label);
    }

    minVisibleX = 0;
    maxVisibleX = 0;

    foreach (Neighborhood *neighborhood, neighborhoods) {
        neighborhood->setRadius(radius);
        if (neighborhood->getMinVisibleX() < minVisibleX) {
            minVisibleX = neighborhood->getMinVisibleX(); // A negative value.
        }
        if (neighborhood->getMaxVisibleX() > maxVisibleX) {
            maxVisibleX = neighborhood->getMaxVisibleX(); // A positive value.
        }
    }

    int maxSourceWidth = 0;
    int rowHeight = tableWidget->rowHeight(0);
    int padding = rowHeight / 12;
    int y = padding;
    y += rowHeight * 5 / 6;

    QVector<QString> inputVector = database->getInputVector();
    foreach (const QString &gi, inputVector) {
        if (database->contains(gi)) {
            QString source = database->sourceOf(gi);
            QGraphicsSimpleTextItem *label = new QGraphicsSimpleTextItem(source);
            label->setBrush(Qt::black);
            QFontMetricsF metrics = qApp->fontMetrics();
            label->setPos(maxVisibleX + 20, y - rowHeight / 2 - metrics.boundingRect(source).height() / 2 + padding / 2);
            labels.append(label);
            addItem(label);
            int sourceWidth = metrics.boundingRect(source).width();
            if (maxSourceWidth < sourceWidth) {
                maxSourceWidth = sourceWidth;
            }
        }
        y += rowHeight;
    }

    width = maxVisibleX - minVisibleX + maxSourceWidth;
    setSceneRect(minVisibleX - 20, 0, width + 60, height);
}

void GraphicsScene::setNeighborhoodRadius(const int &radius, QTreeView *treeView) // 3.8
{
    foreach (QGraphicsSimpleTextItem *label, labels) {
        removeItem(label);
    }

    minVisibleX = 0;
    maxVisibleX = 0;

    foreach (Neighborhood *neighborhood, neighborhoods) {
        neighborhood->setRadius(radius);
        if (neighborhood->getMinVisibleX() < minVisibleX) {
            minVisibleX = neighborhood->getMinVisibleX(); // A negative value.
        }
        if (neighborhood->getMaxVisibleX() > maxVisibleX) {
            maxVisibleX = neighborhood->getMaxVisibleX(); // A positive value.
        }
    }

    int maxSourceWidth = 0;
    int rowHeight = 0;
    int collapsibleHeight = 0;
    int y = 0;
    
    QModelIndexList list = treeView->model()->match(QModelIndex(), Qt::UserRole, true, 1, Qt::MatchExactly);
    QModelIndex root = list.value(0);
    int domains = treeView->model()->rowCount(root);
    for (int i = 0; i < domains; ++i) {
        
        QModelIndex domain = treeView->model()->index(i, 0, root);
        if (i == 0) {
            rowHeight = treeView->sizeHintForIndex(domain).height();
            y += rowHeight - stretch;
        } else {
            y += rowHeight;
        }
        int phylums = treeView->model()->rowCount(domain);
        for (int j = 0; j < phylums; ++j) {
            
            QModelIndex phylum = treeView->model()->index(j, 0, domain);
            if (i == 0) {
                collapsibleHeight = treeView->sizeHintForIndex(phylum).height();
            }
            y += collapsibleHeight;
            int classes = treeView->model()->rowCount(phylum);
            for (int k = 0; k < classes; ++k) {
                
                QModelIndex class_ = treeView->model()->index(k, 0, phylum);
                y += collapsibleHeight;
                int sources = treeView->model()->rowCount(class_);
                for (int l = 0; l < sources; ++l) {
                    
                    QModelIndex source = treeView->model()->index(l, 0, class_);
                    y += collapsibleHeight;
                    QString source_ = treeView->model()->data(source, Qt::AccessibleTextRole).toString();
                    QGraphicsSimpleTextItem *label = new QGraphicsSimpleTextItem(source_);
                    label->setBrush(Qt::black);
                    QFontMetricsF metrics = qApp->fontMetrics();
                    label->setPos(20, y - collapsibleHeight / 2 - metrics.boundingRect(source_).height() / 2);
                    labels.append(label);
                    addItem(label);
                    int sourceWidth = metrics.boundingRect(source_).width() - maxVisibleX + 20;
                    if (maxSourceWidth < sourceWidth) {
                        maxSourceWidth = sourceWidth;
                    }
                    int gis = treeView->model()->rowCount(source);
                    for (int m = 0; m < gis; ++m) {
                        
                        QModelIndex gene = treeView->model()->index(m, 0, source);
                        y += rowHeight;
                    }
                }
            }
        }
    }

    width = maxVisibleX - minVisibleX + maxSourceWidth;
    setSceneRect(minVisibleX - 20, 0, width + 40, height);
}

void GraphicsScene::annotateGene(const QString &gi, const int &y) const
{
    parent->annotateGene(gi, y);
}

void GraphicsScene::annotateDomain(const QString &gi, const int &number, const int &y) const
{
    parent->annotateDomain(gi, number, y);
}

void GraphicsScene::annotateIntergene(const QString &gi, const int &start, const int &end) const
{
    parent->annotateIntergene(gi, start, end);
}

void GraphicsScene::createContextMenu(const QPoint &point) const
{
    parent->createContextMenu(point);
}

int GraphicsScene::getMaxHeight() const
{
    return maxHeight;
}
