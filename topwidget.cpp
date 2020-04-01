#include <QVBoxLayout>
#include <QFont>
#include <QGraphicsView>
#include <QGridLayout>
#include <QHeaderView>
#include <QList>
#include <QModelIndex>
#include <QMouseEvent>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QTreeView>
#include <QVector>
#include "bottomwidget.h"
#include "database.h"
#include "graphicsscene.h"
#include "mainwidget.h"
#include "taxonomymodel.h"
#include "topwidget.h"
#include "treedelegate.h"

TopWidget::TopWidget(MainWidget *parent)
    : QWidget(parent)
{
    this->tableWidget = NULL;
    this->treeView = NULL;
    this->graphicsScene = NULL;
    this->parent = parent;
}

void TopWidget::setTableOutput(Database *database)
{
    tableWidget = new QTableWidget(this);
    tableWidget->setRowCount(0);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Phylum") << tr("Genus"));
    addRows(database);

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    tableWidget->resizeColumnsToContents();
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableWidget->horizontalHeader()->setSectionsClickable(false);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableWidget->verticalHeader()->setSectionsClickable(false);

    tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setMaximumWidth(calculateTableWidth());
    tableWidget->setAlternatingRowColors(true);

    rightWidget = new QWidget(this);

    headerWidget = new QTableWidget(0, 1, this);
    headerWidget->setHorizontalHeaderLabels(QStringList() << tr("Gene Neighborhoods"));
    headerWidget->horizontalHeader()->setStretchLastSection(true);
    headerWidget->horizontalHeader()->setSectionsClickable(false);
    headerWidget->setMaximumHeight(headerWidget->horizontalHeader()->height());

    graphicsScene = new GraphicsScene(tableWidget, database, this);

    graphicsView = new QGraphicsView(this);
    graphicsView->setScene(graphicsScene);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    graphicsView->ensureVisible(0, 0, 0, 0);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(headerWidget);
    rightLayout->addWidget(graphicsView);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    rightWidget->setLayout(rightLayout);

    QScrollBar *tableScrollBar = tableWidget->verticalScrollBar();
    QScrollBar *graphicsScrollBar = graphicsView->verticalScrollBar();
    connect(tableScrollBar, SIGNAL(valueChanged(int)), graphicsScrollBar, SLOT(setValue(int)));
    connect(graphicsScrollBar, SIGNAL(valueChanged(int)), tableScrollBar, SLOT(setValue(int)));

    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(tableWidget);
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setChildrenCollapsible(false);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setSizes(QList<int>() << calculateTableWidth() << rightWidget->maximumWidth());

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(mainSplitter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
    setMaximumHeight(graphicsScene->getMaxHeight());
}

void TopWidget::setTextOutput(Database * /*database*/)
{
}

void TopWidget::setTreeOutput(Database *database)
{
    taxonomyModel = new TaxonomyModel(this);
    taxonomyModel->setTaxonomyData(database);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(taxonomyModel);

    treeView = new QTreeView(this);
    treeView->setModel(proxyModel);
    treeView->sortByColumn(0, Qt::AscendingOrder);
    treeView->setFocusPolicy(Qt::NoFocus);
    treeView->setSelectionMode(QAbstractItemView::NoSelection);
    treeView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    treeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    treeView->expandAll();
    connect(treeView, SIGNAL(collapsed(const QModelIndex &)), treeView, SLOT(expand(const QModelIndex &)));

    rightWidget = new QWidget(this);

    headerWidget = new QTableWidget(0, 1, this);
    headerWidget->setHorizontalHeaderLabels(QStringList() << tr("Gene Neighborhoods"));
    headerWidget->horizontalHeader()->setStretchLastSection(true);
    headerWidget->horizontalHeader()->setSectionsClickable(false);
    headerWidget->setMaximumHeight(headerWidget->horizontalHeader()->height());

    int oldHeaderHeight = treeView->header()->height();
    int newHeaderHeight = headerWidget->horizontalHeader()->height();
    int stretch = newHeaderHeight - oldHeaderHeight;
    treeView->header()->setMinimumHeight(newHeaderHeight);
    treeView->header()->setDefaultAlignment(Qt::AlignHCenter);
    treeView->setItemDelegate(new TreeDelegate(0, headerWidget->verticalHeader()->defaultSectionSize(), this));

    graphicsScene = new GraphicsScene(stretch, treeView, database, this);

    graphicsView = new QGraphicsView(this);
    graphicsView->setScene(graphicsScene);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    graphicsView->ensureVisible(0, 0, 0, 0);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(headerWidget);
    rightLayout->addWidget(graphicsView);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    rightWidget->setLayout(rightLayout);

    QScrollBar *treeScrollBar = treeView->verticalScrollBar();
    QScrollBar *graphicsScrollBar = graphicsView->verticalScrollBar();
    connect(treeScrollBar, SIGNAL(valueChanged(int)), graphicsScrollBar, SLOT(setValue(int)));
    connect(graphicsScrollBar, SIGNAL(valueChanged(int)), treeScrollBar, SLOT(setValue(int)));

    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(treeView);
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setChildrenCollapsible(false);
    mainSplitter->setStretchFactor(1, 1);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(mainSplitter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
    setMaximumHeight(graphicsScene->getMaxHeight());
}

void TopWidget::markGenesOfMembraneProteins(bool checked) // 3.2 ###
{
    if (graphicsScene == NULL) {
        return;
    }
    graphicsScene->markGenesOfMembraneProteins(checked);
}

void TopWidget::showIntergenes(bool checked, const int &radius) // 3.4 ###
{
    if (graphicsScene == NULL) {
        return;
    }
    graphicsScene->showIntergenes(checked, radius);
}

void TopWidget::colorByDomainArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.1
{
    if (graphicsScene == NULL) {
        return;
    }
    graphicsScene->colorByDomainArchitectures(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
}

void TopWidget::colorByCOGArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.2
{
    if (graphicsScene == NULL) {
        return;
    }
    graphicsScene->colorByCOGArchitectures(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
}

void TopWidget::colorByDomains(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.3
{
    if (graphicsScene == NULL) {
        return;
    }
    graphicsScene->colorByDomains(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
}

void TopWidget::colorByCOGs(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.4
{
    if (graphicsScene == NULL) {
        return;
    }
    graphicsScene->colorByCOGs(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
}

void TopWidget::setNeighborhoodRadius(const int &radius) // 3.8
{
    if (graphicsScene == NULL) {
        return;
    }
    if (tableWidget != NULL) {
        graphicsScene->setNeighborhoodRadius(radius, tableWidget);
    } else if (treeView != NULL) {
        graphicsScene->setNeighborhoodRadius(radius, treeView);
    }
}

void TopWidget::annotateGene(const QString &gi, const int &y) const
{
    parent->annotateGene(gi, y);
}

void TopWidget::annotateDomain(const QString &gi, const int &number, const int &y) const
{
    parent->annotateDomain(gi, number, y);
}

void TopWidget::annotateIntergene(const QString &gi, const int &start, const int &end) const
{
    parent->annotateIntergene(gi, start, end);
}

void TopWidget::createContextMenu(const QPoint &point) const
{
    parent->createContextMenu(point);
}

GraphicsScene *TopWidget::getGraphicsScene() const
{
    return graphicsScene;
}

void TopWidget::mousePressEvent(QMouseEvent * /*event*/)
{
    if (graphicsScene == NULL) {
        return;
    }
    if (graphicsScene->selectedItems().isEmpty()) {
        parent->deleteAnnotation();
    }
}

void TopWidget::addRows(Database *database)
{
    if (tableWidget == NULL) {
        return;
    }
    QFont font;
    font.setItalic(true);
    int row = tableWidget->rowCount();
    QVector<QString> inputVector = database->getInputVector();
    foreach (const QString &gi, inputVector) {

        tableWidget->insertRow(row);

        QString phylum = database->phylumOf(gi);
        if (phylum.startsWith("Candidatus")) {
            phylum = phylum.section(" ", 1, 1);
        } else if (phylum.startsWith("candidate division")) {
            phylum = phylum.section(" ", 2, 2);
        }
        QTableWidgetItem *item0 = new QTableWidgetItem;
        if (!phylum.isEmpty()) {
            item0->setText(phylum);
        } else {
            item0->setText("Unknown");
        }
        tableWidget->setItem(row, 0, item0);

        QString genus = database->genusOf(gi);
        QTableWidgetItem *item1 = new QTableWidgetItem;
        if (!genus.isEmpty()) {
            item1->setText(genus);
            item1->setFont(font);
        } else {
            item1->setText("Unknown");
        }
        tableWidget->setItem(row, 1, item1);

        ++row;
    }
}

int TopWidget::calculateTableWidth() const
{
    if (tableWidget == NULL) {
        return - 1;
    }
    int width = 0;
    int columns = tableWidget->columnCount();
    for (int i = 0; i < columns; ++i) {
        width += tableWidget->columnWidth(i);
    }
    width += tableWidget->verticalHeader()->width();
    width += tableWidget->contentsMargins().left();
    width += tableWidget->contentsMargins().right();
    return width;
}

int TopWidget::calculateTableHeight() const
{
    if (tableWidget == NULL) {
        return - 1;
    }
    int height = 0;
    int rows = tableWidget->rowCount();
    for (int i = 0; i < rows; ++i) {
        height += tableWidget->rowHeight(i);
    }
    height += tableWidget->horizontalHeader()->height();
    height += tableWidget->horizontalScrollBar()->height();
    height += tableWidget->contentsMargins().top();
    height += tableWidget->contentsMargins().bottom();
    return height;
}
