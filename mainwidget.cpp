#include <QApplication>
#include <QCursor>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QList>
#include <QPainter>
#include <QPrinter>
#include <QSettings>
#include <QSplitter>
#include "bottomwidget.h"
#include "cognat.h"
#include "database.h"
#include "graphicsscene.h"
#include "mainwidget.h"
#include "rightwidget.h"
#include "topwidget.h"

MainWidget::MainWidget(Database *database, COGNAT *parent)
    : QWidget(parent)
{
    this->database = database;
    this->parent = parent;

    topWidget = new TopWidget(this);
    bottomWidget = new BottomWidget(this);
    rightWidget = new RightWidget(this);

    leftSplitter = new QSplitter(Qt::Vertical, this);
    leftSplitter->addWidget(topWidget);
    leftSplitter->addWidget(bottomWidget);
    leftSplitter->setChildrenCollapsible(false);
    //leftSplitter->setStretchFactor(0, 1); // This does not matter.
    connect(leftSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(saveSplitterState(int, int)));

    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setChildrenCollapsible(false);
    mainSplitter->setStretchFactor(0, 1);
    connect(mainSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(saveSplitterState(int, int)));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(mainSplitter);
    setLayout(mainLayout);
}

void MainWidget::setTableTopWidget()
{
    delete topWidget;
    topWidget = new TopWidget(this);
    topWidget->setTableOutput(database);
    leftSplitter->insertWidget(0,topWidget);
    //leftSplitter->setStretchFactor(0, 1); // This is ugly when a small file is loading.
    deleteAnnotation();
}

void MainWidget::setTextTopWidget()
{
    delete topWidget;
    topWidget = new TopWidget(this);
    topWidget->setTextOutput(database);
    leftSplitter->insertWidget(0,topWidget);
    //leftSplitter->setStretchFactor(0, 1); // This is ugly when a small file is loading.
    deleteAnnotation();
}

void MainWidget::setTreeTopWidget()
{
    delete topWidget;
    topWidget = new TopWidget(this);
    topWidget->setTreeOutput(database);
    leftSplitter->insertWidget(0,topWidget);
    //leftSplitter->setStretchFactor(0, 1); // This is ugly when a small file is loading.
    deleteAnnotation();
}

void MainWidget::setRadius(const int &radius)
{
    this->radius = radius;
}

void MainWidget::setMinimumScore(const int &minscore)
{
    this->minscore = minscore;
}

void MainWidget::setMinimumLength(const int &minlength)
{
    this->minlength = minlength;
}

void MainWidget::setMaximumOverlapping(const int &maxoverlap)
{
    this->maxoverlap = maxoverlap;
}

void MainWidget::setTrustworthyScore(const int &trustscore)
{
    this->trustscore = trustscore;
}

void MainWidget::markGenesOfMembraneProteins(bool checked) // 3.2 ###
{
    topWidget->markGenesOfMembraneProteins(checked);
}

void MainWidget::showIntergenes(bool checked) // 3.4 ###
{
    topWidget->showIntergenes(checked, radius);
}

void MainWidget::colorByDomainArchitectures() // 3.5.1
{
    mode = DomainArchitectures;
    bottomWidget->clear();
    topWidget->colorByDomainArchitectures(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
    bottomWidget->setSchemeOutput(database);
}

void MainWidget::colorByCOGArchitectures() // 3.5.2
{
    mode = COGArchitectures;
    bottomWidget->clear();
    topWidget->colorByCOGArchitectures(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
    bottomWidget->setSchemeOutput(database);
}

void MainWidget::colorByDomains() // 3.5.3
{
    mode = Domains;
    bottomWidget->clear();
    topWidget->colorByDomains(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
    bottomWidget->setSchemeOutput(database);
}

void MainWidget::colorByCOGs() // 3.5.4
{
    mode = COGs;
    bottomWidget->clear();
    topWidget->colorByCOGs(minscore, minlength, maxoverlap, trustscore, radius, bottomWidget);
    bottomWidget->setSchemeOutput(database);
}

void MainWidget::setNeighborhoodRadius() // 3.8
{
    topWidget->setNeighborhoodRadius(radius);
}

void MainWidget::annotateGene(const QString &gi, const int &y)
{
    deleteAnnotation();
    rightWidget->annotateGene(gi, minscore, minlength, maxoverlap, trustscore, y, database);
}

void MainWidget::annotateDomain(const QString &gi, const int &number, const int &y)
{
    bool currentGene = false;
    if (gi == rightWidget->getCurrentGi() && y == rightWidget->getCurrentY()) {
        currentGene = true;
    }
    deleteAnnotation();
    rightWidget->annotateGene(gi, minscore, minlength, maxoverlap, trustscore, y, database);
    rightWidget->annotateDomain(number);
    if (currentGene) {
        rightWidget->restoreState();
    } else {
        rightWidget->saveState();
    }
}

void MainWidget::annotateIntergene(const QString &gi, const int &start, const int &end)
{
    deleteAnnotation();
    rightWidget->annotateIntergene(gi, start, end, database);
}

void MainWidget::deleteAnnotation()
{
    topWidget->getGraphicsScene()->clearSelection();
    delete rightWidget;
    rightWidget = new RightWidget(this);
    mainSplitter->insertWidget(1, rightWidget);
    readSettings();
}

void MainWidget::createContextMenu(const QPoint &point) const
{
    parent->createContextMenu(point);
}

void MainWidget::createColorDialog(const QString &architecture, const QColor &initial) const
{
    parent->createColorDialog(architecture, initial);
}

bool MainWidget::savePdf(const QString &fileName)
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
#ifndef Q_OS_MAC
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
#endif

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    /*if (size != QSizeF()) {
        printer.setPaperSize(size, QPrinter::Point);
    }*/

    QPainter painter(&printer);

    GraphicsScene *graphicsScene = topWidget->getGraphicsScene();
    if (graphicsScene == NULL) {
        return false;
    }
    deleteAnnotation();
    graphicsScene->render(&painter);

    if (!painter.end()) {
        return false;
    }

    QString fileNameCS = fileName;
    fileNameCS.truncate(fileNameCS.lastIndexOf("."));
    fileNameCS += QString("_CS.pdf");

    QPrinter printerCS(QPrinter::HighResolution);
    printerCS.setOutputFormat(QPrinter::PdfFormat);
    printerCS.setOutputFileName(fileNameCS);

    QPainter painterCS(&printerCS);

    QGraphicsScene *schemeScene = bottomWidget->getSchemeScene();
    if (schemeScene != NULL) {
        schemeScene->render(&painterCS);
    }

#ifndef Q_OS_MAC
    qApp->restoreOverrideCursor();
#endif

    return true;
}

QString MainWidget::currentGi() const
{
    return rightWidget->getCurrentGi();
}

QString MainWidget::currentArchitecture() const
{
    switch (mode) {
    case DomainArchitectures:
        return rightWidget->getCurrentDomainArchitecture();
    case COGArchitectures:
        return rightWidget->getCurrentCOGArchitecture();
    case Domains:
        return rightWidget->getCurrentDomainArchitecture();
    case COGs:
        return rightWidget->getCurrentCOGArchitecture();
    default:
        return QString();
    }
}

void MainWidget::saveSplitterState(int /*pos*/, int /*index*/)
{
    writeSettings();
}

void MainWidget::readSettings()
{
    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    leftSplitter->restoreState(settings.value("leftSplitterPos").toByteArray());
    mainSplitter->restoreState(settings.value("mainSplitterPos").toByteArray());
}

void MainWidget::writeSettings()
{
    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    settings.setValue("leftSplitterPos", leftSplitter->saveState());
    settings.setValue("mainSplitterPos", mainSplitter->saveState());
}
