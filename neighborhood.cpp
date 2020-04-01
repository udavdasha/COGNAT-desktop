#include <QPainter>
#include <QSettings>
#include <QStyleOptionGraphicsItem>
#include <QVector>
#include "bottomwidget.h"
#include "database.h"
#include "gene.h"
#include "graphicsscene.h"
#include "intergene.h"
#include "neighborhood.h"

Neighborhood::Neighborhood(const QString &gi, const int &height, Database *database, GraphicsScene *parent)
    :QGraphicsItem(NULL)
{
    this->baseGi = gi;
    this->height = height;
    this->minX = 0;
    this->minVisibleX = 0;
    this->maxVisibleX = 0;
    this->database = database;
    this->parent = parent;

    QVector<QString> neighborhood = database->maxNeighborhoodOf(baseGi);
    int baseStrand = database->strandOf(baseGi);
    if (baseStrand > 0) {

        int x = 0;
        int space = 0;
        int prevEnd = 0;
        int intergeneStart = 0;
        int z = neighborhood.size();
        for (int i = 0; i < neighborhood.size(); ++i) {

            QString gi = neighborhood.value(i);
            int end = database->endOf(gi) / 20;
            if (end == 0) {
                end = 1;
            }
            int width = database->geneLengthOf(gi) / 20;
            int strand = database->strandOf(gi);

            if (prevEnd == 0) {
                prevEnd = end;
                intergeneStart = database->endOf(gi) + 1;
            } else {
                if (prevEnd < end) { // A normal case.
                    space = end - prevEnd - width;
                } else {
                    int start = database->startOf(gi) / 20;
                    if (start > prevEnd) {
                        space = start - prevEnd;
                    } else {
                        space = 0;
                    }
                }
                if (space > 0) {
                    Intergene *intergene = new Intergene(space, height, this);
                    intergene->setData(Gene::Gi, gi);
                    intergene->setData(Gene::IsIntergene, true);
                    intergene->setData(Gene::Start, intergeneStart);
                    intergene->setData(Gene::End, database->startOf(gi) - 1);
                    intergene->setPos(x, - height * 2.5 / 6);
                    intergene->setVisible(false);
                }
                prevEnd = end;
                intergeneStart = database->endOf(gi) + 1;
            }
            x += space;

            if (width > 0) { // Division into 20 could give 0.
                Gene *gene = new Gene(width, height, strand, this);
                gene->setData(Gene::Gi, gi);
                gene->setData(Gene::IsGene, true);
                gene->setData(Gene::IsMarked, false);
                gene->setData(Gene::Color, QColor(Qt::lightGray));
                gene->setPos(x, 0);
                gene->setVisible(false);
                gene->setZValue(z);
                --z;
            }
            if (gi == baseGi) {
                minX = - x;
            }
            x += width;
        }
    } else if (baseStrand < 0) {

        int x = 0;
        int space = 0;
        int prevStart = 0;
        int intergeneEnd = 0;
        int z = neighborhood.size();
        for (int i = neighborhood.size() - 1; i > - 1; --i) {

            QString gi = neighborhood.value(i);
            int start = database->startOf(gi) / 20;
            if (start == 0) {
                start = 1;
            }
            int width = database->geneLengthOf(gi) / 20;
            int strand = database->strandOf(gi);

            if (strand < 0) {
                strand = 1;
            } else if (strand > 0) {
                strand = - 1;
            }

            if (prevStart == 0) {
                prevStart = start;
                intergeneEnd = database->startOf(gi) - 1;
            } else {
                if (start < prevStart) { // A normal case.
                    space = prevStart - start - width;
                } else {
                    int end = database->endOf(gi) / 20;
                    if (end < prevStart) {
                        space = prevStart - end;
                    } else {
                        space = 0;
                    }
                }
                if (space > 0) {
                    Intergene *intergene = new Intergene(space, height, this);
                    intergene->setData(Gene::Gi, gi);
                    intergene->setData(Gene::IsIntergene, true);
                    intergene->setData(Gene::Start, database->endOf(gi) + 1);
                    intergene->setData(Gene::End, intergeneEnd);
                    intergene->setPos(x, - height * 2.5 / 6);
                    intergene->setVisible(false);
                }
                prevStart = start;
                intergeneEnd = database->startOf(gi) - 1;
            }
            x += space;

            if (width > 0) { // Division into 20 could give 0.
                Gene *gene = new Gene(width, height, strand, this);
                gene->setData(Gene::Gi, gi);
                gene->setData(Gene::IsGene, true);
                gene->setData(Gene::IsMarked, false);
                gene->setData(Gene::Color, QColor(Qt::lightGray));
                gene->setPos(x, 0);
                gene->setVisible(false);
                gene->setZValue(z);
                --z;
            }
            if (gi == baseGi) {
                minX = - x;
            }
            x += width;
        }
    }
}

QRectF Neighborhood::boundingRect() const
{
    return QRectF();
}

void Neighborhood::paint(QPainter * /*painter*/, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
}

void Neighborhood::markGenesOfMembraneProteins(bool checked) // 3.2 ###
{
    QList<QGraphicsItem *> list = childItems();
    foreach (QGraphicsItem *item, list) {
        if (!item->data(Gene::IsIntergene).toBool()) {
            QString gi = item->data(Gene::Gi).toString();
            if (database->isMembrane(gi)) {
                item->setData(Gene::IsMarked, checked);
            }
        }
    }
}

void Neighborhood::showIntergenes(bool checked, const int &radius) // 3.4 ###
{
    QVector<QString> neighborhood = database->neighborhoodOf(baseGi, radius);
    QList<QGraphicsItem *> list = childItems();
    foreach (QGraphicsItem *item, list) {
        if (item->data(Gene::IsIntergene).toBool()) {
            QString gi = item->data(Gene::Gi).toString();
            bool first = false;
            if (neighborhood.indexOf(gi) == 0) {
                if (database->strandOf(baseGi) > 0) {
                    first = true;
                }
            }
            if (neighborhood.indexOf(gi) == neighborhood.size() - 1) {
                if (database->strandOf(baseGi) < 0) {
                    first = true;
                }
            }
            if (neighborhood.contains(gi) && !first) {
                item->setVisible(checked);
            } else {
                item->setVisible(false);
            }
        }
    }
}

void Neighborhood::colorByDomainArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.1
{
    bottomWidget->setRowHeight(height);
    QVector<QString> neighborhood = database->neighborhoodOf(baseGi, radius); //
    QList<QString> architectureList = database->getArchitectureList();
    QList<QGraphicsItem *> list = childItems();
    foreach (QGraphicsItem *item, list) {
        if (item->data(Gene::IsGene).toBool()) {
            QString gi = item->data(Gene::Gi).toString();
            QString architecture = database->domainArchitectureOf(gi, minscore, minlength, maxoverlap, trustscore);
            if (architecture == database->lastRemovedArchitecture() && !architecture.isEmpty()) {
                database->setExamining(gi, false);
            }
            if (architectureList.contains(architecture)) {
                database->setExamining(gi, true);
            }
            QColor color;
            if (database->isExamining(gi) && !architecture.isEmpty()) {
                color = readColorFromSettings(architecture);
                if (color == QColor()) {
                    color = randomColor();
                    writeColorToSettings(architecture, color);
                }
            } else {
                color = Qt::lightGray;
            }
            item->setData(Gene::Color, color);
            if (color != Qt::lightGray && neighborhood.contains(gi)) {
                int width = database->geneLengthOf(gi) / 20;
                bottomWidget->setArchitectureWidth(architecture, width);
                bottomWidget->setArchitectureColor(architecture, color);
                bottomWidget->increaseArchitectureQuantity(architecture);
            }
        } else if (item->data(Gene::IsDomain).toBool()) { //
            scene()->removeItem(item); //
            delete item; //
        } //
    }
}

void Neighborhood::colorByCOGArchitectures(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.2
{
    bottomWidget->setRowHeight(height);
    QVector<QString> neighborhood = database->neighborhoodOf(baseGi, radius); //
    QList<QString> architectureList = database->getArchitectureList();
    QList<QGraphicsItem *> list = childItems();
    foreach (QGraphicsItem *item, list) {
        if (item->data(Gene::IsGene).toBool()) {
            QString gi = item->data(Gene::Gi).toString();
            QString architecture = database->COGArchitectureOf(gi, minscore, minlength, maxoverlap, trustscore);
            if (architecture == database->lastRemovedArchitecture() && !architecture.isEmpty()) {
                database->setExamining(gi, false);
            }
            if (architectureList.contains(architecture)) {
                database->setExamining(gi, true);
            }
            QColor color;
            if (database->isExamining(gi) && !architecture.isEmpty()) {
                color = readColorFromSettings(architecture);
                if (color == QColor()) {
                    color = randomColor();
                    writeColorToSettings(architecture, color);
                }
            } else {
                color = Qt::lightGray;
            }
            item->setData(Gene::Color, color);
            if (color != Qt::lightGray && neighborhood.contains(gi)) {
                int width = database->geneLengthOf(gi) / 20;
                bottomWidget->setArchitectureWidth(architecture, width);
                bottomWidget->setArchitectureColor(architecture, color);
                bottomWidget->increaseArchitectureQuantity(architecture);
            }
        } else if (item->data(Gene::IsDomain).toBool()) { //
            scene()->removeItem(item); //
            delete item; //
        } //
    }
}

void Neighborhood::colorByDomains(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.3
{
    bottomWidget->setRowHeight(height);
    QVector<QString> neighborhood = database->neighborhoodOf(baseGi, radius); //
    QList<QString> architectureList = database->getArchitectureList();
    QList<QGraphicsItem *> list = childItems();
    foreach (QGraphicsItem *item, list) {
        if (item->data(Gene::IsGene).toBool()) {
            QString gi = item->data(Gene::Gi).toString();
            QString architecture = database->domainArchitectureOf(gi, minscore, minlength, maxoverlap, trustscore);
            if (architecture == database->lastRemovedArchitecture() && !architecture.isEmpty()) {
                database->setExamining(gi, false);
            }
            if (architectureList.contains(architecture)) {
                database->setExamining(gi, true);
            }
            QColor color;
            if (database->isExamining(gi) && !architecture.isEmpty()) {
                QList<Domain> domains = database->domainsOf(gi, minscore, minlength, maxoverlap, trustscore); //
                if (domains.size() < 2) { //
                    color = readColorFromSettings(architecture);
                    if (color == QColor()) {
                        color = randomColor();
                        writeColorToSettings(architecture, color);
                    }
                } else { //
                    color = Qt::lightGray; //
                    if (neighborhood.contains(gi)) { //
                        createArchitecture(domains, item, bottomWidget); //
                    } //
                } //
            } else {
                color = Qt::lightGray;
            }
            item->setData(Gene::Color, color);
            if (color != Qt::lightGray && neighborhood.contains(gi)) {
                int width = database->geneLengthOf(gi) / 20;
                bottomWidget->setArchitectureWidth(architecture, width);
                bottomWidget->setArchitectureColor(architecture, color);
                bottomWidget->increaseArchitectureQuantity(architecture);
            }
        } else if (item->data(Gene::IsDomain).toBool()) { //
            scene()->removeItem(item); //
            delete item; //
        } //
    }
}

void Neighborhood::colorByCOGs(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &radius, BottomWidget *bottomWidget) // 3.5.4
{
    bottomWidget->setRowHeight(height);
    QVector<QString> neighborhood = database->neighborhoodOf(baseGi, radius); //
    QList<QString> architectureList = database->getArchitectureList();
    QList<QGraphicsItem *> list = childItems();
    foreach (QGraphicsItem *item, list) {
        if (item->data(Gene::IsGene).toBool()) {
            QString gi = item->data(Gene::Gi).toString();
            QString architecture = database->COGArchitectureOf(gi, minscore, minlength, maxoverlap, trustscore);
            if (architecture == database->lastRemovedArchitecture() && !architecture.isEmpty()) {
                database->setExamining(gi, false);
            }
            if (architectureList.contains(architecture)) {
                database->setExamining(gi, true);
            }
            QColor color;
            if (database->isExamining(gi) && !architecture.isEmpty()) {
                QList<Domain> COGs = database->COGsOf(gi, minscore, minlength, maxoverlap, trustscore); //
                if (COGs.size() < 2) { //
                    color = readColorFromSettings(architecture);
                    if (color == QColor()) {
                        color = randomColor();
                        writeColorToSettings(architecture, color);
                    }
                } else { //
                    color = Qt::lightGray; //
                    if (neighborhood.contains(gi)) { //
                        createArchitecture(COGs, item, bottomWidget); //
                    } //
                } //
            } else {
                color = Qt::lightGray;
            }
            item->setData(Gene::Color, color);
            if (color != Qt::lightGray && neighborhood.contains(gi)) {
                int width = database->geneLengthOf(gi) / 20;
                bottomWidget->setArchitectureWidth(architecture, width);
                bottomWidget->setArchitectureColor(architecture, color);
                bottomWidget->increaseArchitectureQuantity(architecture);
            }
        } else if (item->data(Gene::IsDomain).toBool()) { //
            scene()->removeItem(item); //
            delete item; //
        } //
    }
}

void Neighborhood::setRadius(const int &radius) // 3.8
{
    int relativeMinVisibleX = 0;
    int relativeMaxVisibleX = 0;
    int relativeZeroX = 0;

    QVector<QString> neighborhood = database->neighborhoodOf(baseGi, radius);
    QList<QGraphicsItem *> list = childItems();
    foreach (QGraphicsItem *item, list) {
        if (item->data(Gene::IsGene).toBool()) {
            QString gi = item->data(Gene::Gi).toString();
            if (neighborhood.contains(gi)) {
                item->setVisible(true);
                if (neighborhood.indexOf(gi) == 0) {
                    if (database->strandOf(baseGi) > 0) {
                        relativeMinVisibleX = item->pos().x();
                    }
                    if (database->strandOf(baseGi) < 0) {
                        relativeMaxVisibleX = item->pos().x();
                        relativeMaxVisibleX += database->geneLengthOf(gi) / 20;
                    }
                }
                if (neighborhood.indexOf(gi) == neighborhood.size() - 1) {
                    if (database->strandOf(baseGi) < 0) {
                        relativeMinVisibleX = item->pos().x();
                    }
                    if (database->strandOf(baseGi) > 0) {
                        relativeMaxVisibleX = item->pos().x();
                        relativeMaxVisibleX += database->geneLengthOf(gi) / 20;
                    }
                }
                if (gi == baseGi) {
                    relativeZeroX = item->pos().x();
                }
            } else {
                item->setVisible(false);
            }
        }
    }

    minVisibleX = relativeMinVisibleX - relativeZeroX; // A negative value.
    maxVisibleX = relativeMaxVisibleX - relativeZeroX; // A positive value.
}

void Neighborhood::annotateGene(const QString &gi, const int &y) const
{
    parent->annotateGene(gi, y);
}

void Neighborhood::annotateDomain(const QString &gi, const int &number, const int &y) const
{
    parent->annotateDomain(gi, number, y);
}

void Neighborhood::annotateIntergene(const QString &gi, const int &start, const int &end) const
{
    parent->annotateIntergene(gi, start, end);
}

void Neighborhood::createContextMenu(const QPoint &point) const
{
    parent->createContextMenu(point);
}

int Neighborhood::getMinX() const
{
    return minX;
}

int Neighborhood::getMinVisibleX() const
{
    return minVisibleX;
}

int Neighborhood::getMaxVisibleX() const
{
    return maxVisibleX;
}

QString Neighborhood::getBaseGi() const
{
    return baseGi;
}

void Neighborhood::createArchitecture(const QList<Domain> &domains, QGraphicsItem *item, BottomWidget *bottomWidget)
{
    QString gi = item->data(Gene::Gi).toString();

    int neighborhoodSize = database->getMaxRadius() * 2 + 1;
    int baseStrand = database->strandOf(baseGi);
    int strand = database->strandOf(gi);
    int proteinWidth = database->proteinLengthOf(gi);
    int geneWidth = database->geneLengthOf(gi) / 20; // geneWidth > 0

    if (baseStrand < 0) {
        if (strand < 0) {
            strand = 1;
        } else if (strand > 0) {
            strand = - 1;
        }
    }

    for (int i = 0; i < domains.size(); ++i) {

        if (i == 0) {

            int end = calculateEndOf(i, domains);
            int width = end * geneWidth / proteinWidth + 1;
            if (width > 0) { // Division into 100 could give 0.
                QColor color = manageColor(i, domains, width, bottomWidget);
                Gene *gene = new Gene(width, height, 0, this);
                gene->setData(Gene::Gi, gi);
                gene->setData(Gene::Number, i);
                gene->setData(Gene::IsDomain, true);
                gene->setData(Gene::Color, color);
                gene->setData(Gene::Y, item->scenePos().y());
                if (strand > 0) {
                    gene->setPos(item->pos().x(), 0);
                } else if (strand < 0) {
                    gene->setPos(item->pos().x() + geneWidth - width, 0);
                }
                gene->setZValue(neighborhoodSize + i + 1);
            }
        }

        else if (i < domains.size() - 1) {

            int start = calculateStartOf(i, domains);
            int end = calculateEndOf(i, domains);
            int width = (end - start + 1) * geneWidth / proteinWidth + 1;
            int x = start * geneWidth / proteinWidth + 1;
            if (width > 0) { // Division into 100 could give 0.
                QColor color = manageColor(i, domains, width, bottomWidget);
                Gene *gene = new Gene(width, height, 0, this);
                gene->setData(Gene::Gi, gi);
                gene->setData(Gene::Number, i);
                gene->setData(Gene::IsDomain, true);
                gene->setData(Gene::Color, color);
                gene->setData(Gene::Y, item->scenePos().y());
                if (strand > 0) {
                    gene->setPos(item->pos().x() + x - 1, 0);
                } else if (strand < 0) {
                    gene->setPos(item->pos().x() + geneWidth - width - x + 1, 0);
                }
                gene->setZValue(neighborhoodSize + i + 1);
            }
        }

        else {

            int start = calculateStartOf(i, domains);
            int width = (proteinWidth - start + 1) * geneWidth / proteinWidth + 1;
            if (width > 0) { // Division into 100 could give 0.
                QColor color = manageColor(i, domains, width, bottomWidget);
                Gene *gene = new Gene(width, height, strand, this);
                gene->setData(Gene::Gi, gi);
                gene->setData(Gene::Number, i);
                gene->setData(Gene::IsDomain, true);
                gene->setData(Gene::Color, color);
                gene->setData(Gene::Y, item->scenePos().y());
                if (strand > 0) {
                    gene->setPos(item->pos().x() + geneWidth - width, 0);
                } else if (strand < 0) {
                    gene->setPos(item->pos().x(), 0);
                }
                gene->setZValue(neighborhoodSize + i + 1);
            }
        }
    }
}

void Neighborhood::writeColorToSettings(const QString &architecture, const QColor &color)
{
    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    settings.setValue(architecture, color);
}

int Neighborhood::calculateStartOf(const int &i, const QList<Domain> &domains) const
{
    Domain baseDomain = domains.value(i);
    Domain prevDomain = domains.value(i - 1); // prevDomain exists, because baseDomain is the last, and domains.size() >= 2.
    int baseStart = baseDomain.getStart();
    int prevEnd = prevDomain.getEnd();
    int interdomain = baseStart - prevEnd - 1;
    baseStart -= interdomain / 2;
    return baseStart;
}

int Neighborhood::calculateEndOf(const int &i, const QList<Domain> &domains) const
{
    Domain baseDomain = domains.value(i);
    Domain nextDomain = domains.value(i + 1); // nextDomain exists, because baseDomain is the first, and domains.size() >= 2.
    int baseEnd = baseDomain.getEnd();
    int nextStart = nextDomain.getStart();
    int interdomain = nextStart - baseEnd - 1;
    baseEnd += interdomain / 2;
    return baseEnd;
}

QColor Neighborhood::manageColor(const int &i, const QList<Domain> &domains, const int &width, BottomWidget *bottomWidget)
{
    Domain baseDomain = domains.value(i);
    QString accession = baseDomain.getAccession();
    QColor color = readColorFromSettings(accession);
    if (color == QColor()) {
        color = randomColor();
        writeColorToSettings(accession, color);
    }
    bottomWidget->setArchitectureWidth(accession, width);
    bottomWidget->setArchitectureColor(accession, color);
    bottomWidget->increaseArchitectureQuantity(accession);
    return color;
}

QColor Neighborhood::readColorFromSettings(const QString &architecture)
{
    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    return settings.value(architecture).value<QColor>();
}

QColor Neighborhood::randomColor() const
{
    int red = std::rand() % 255;
    int green = std::rand() % 255;
    int blue = std::rand() % 255;
    return QColor(red, green, blue);
}
