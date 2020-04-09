#include <QApplication>
#include <QVBoxLayout>
#include <QFontMetricsF>
#include <QHeaderView>
#include <QList>
#include <QScrollBar>
#include <QSettings>
#include <QStringList>
#include <QTableWidget>
#include "database.h"
#include "domain.h"
#include "mainwidget.h"
#include "rightwidget.h"
#include "textbrowser.h"

RightWidget::RightWidget(MainWidget *parent)
    : QWidget(parent)
{
    currentY = 0;
    currentGi = QString();
    currentDomainArchitecture = QString();
    currentCOGArchitecture = QString();

    defaultFont = qApp->font();
    sequenceFont.setFamily("Courier New");

    textFormat.setBackground(Qt::lightGray);

    headerWidget = new QTableWidget(0, 1, this);
    headerWidget->setHorizontalHeaderLabels(QStringList() << tr("Annotation"));
    headerWidget->horizontalHeader()->setStretchLastSection(true);
    headerWidget->horizontalHeader()->setSectionsClickable(false);
    headerWidget->setMaximumHeight(headerWidget->horizontalHeader()->height());

    QString tip = tr("Here can be displayed an annotation of genes and intergenic spaces.");

    QFontMetricsF metrics(qApp->font());
    int minimumWidth = metrics.boundingRect("\t\t\t\t\t\t").width();
    int verticalScrollBarWidth = headerWidget->verticalScrollBar()->sizeHint().width();

    textBrowser = new TextBrowser(this);
    textBrowser->setOpenExternalLinks(true);
    textBrowser->setLineWrapMode(QTextEdit::FixedPixelWidth);
    textBrowser->setLineWrapColumnOrWidth(minimumWidth);
    textBrowser->append(tip);
    textBrowser->scrollToAnchor(tr("Here"));
    connect(textBrowser->horizontalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(saveSliderState(int)));
    connect(textBrowser->verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(saveSliderState(int)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(textBrowser);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    setMaximumWidth(minimumWidth + verticalScrollBarWidth * 1.5);

    this->parent = parent;
}

void RightWidget::annotateGene(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const int &y, Database *database)
{
    currentY = y;
    currentGi = gi;
    currentDomainArchitecture = database->domainArchitectureOf(gi, minscore, minlength, maxoverlap, trustscore);
    currentCOGArchitecture = database->COGArchitectureOf(gi, minscore, minlength, maxoverlap, trustscore);

    QString domain = database->domainOf(gi); // 1
    QString phylum = database->phylumOf(gi); // 2
    QString class_ = database->classOf(gi); // 3
    QString order = database->orderOf(gi); // 4
    QString family = database->familyOf(gi); // 5
    QString accession = database->accessionOf(gi); // 6
    QString definition = database->definitionOf(gi); // 7

    QString id = database->idOf(gi); // 9
    QString locus = database->locusOf(gi); // 10
    QString description = database->descriptionOf(gi); // 11

    QString frame = database->frameOf(gi); // 12
    QString coordinates = database->coordinatesOf(gi); // 13
    QString proteinLength = QString::number(database->proteinLengthOf(gi)); // 14
    int strand = database->strandOf(gi); // 15

    int helices = database->helicesOf(gi); // 16

    QList<Domain> domains = database->domainsOf(gi, minscore, minlength, maxoverlap, trustscore); // 17

    QList<Domain> COGs = database->COGsOf(gi, minscore, minlength, maxoverlap, trustscore); // 18

    QString proteinSequence = database->proteinSequenceOf(gi); // 19

    QString nucleotideSequence = database->nucleotideSequenceOf(gi); // 20

    textBrowser->clear();

    textBrowser->append(tr("Accession:\t%1").arg(accession)); // 6
    textBrowser->append(tr("Domain:\t%1").arg(domain)); // 1
    textBrowser->append(tr("Phylum:\t%1").arg(phylum)); // 2
    textBrowser->append(tr("Class:\t%1").arg(class_)); // 3
    textBrowser->append(tr("Order:\t%1").arg(order)); // 4
    textBrowser->append(tr("Family:\t%1").arg(family)); // 5
    textBrowser->append(tr("Definition:\t%1").arg(definition)); // 7
    textBrowser->append("");

    // if QStringList "uniprotAc" and/or "uniprotId" isEmpty: print nothing!
    textBrowser->append(tr("Gi:\t%1").arg(gi)); // 8
    textBrowser->append(tr("Id:\t%1").arg(id)); // 9
    textBrowser->append(tr("Locus:\t%1").arg(locus)); // 10
    textBrowser->append(tr("Description:\t%1").arg(description)); // 11
    textBrowser->append("");

    textBrowser->append(tr("Frame:\t%1").arg(frame)); // 12
    if (strand > 0) {
        textBrowser->append(tr("Strand:\t\"+\" (5'-3')")); // 15
    } else if (strand < 0) {
        textBrowser->append(tr("Strand:\t\"-\" (complement)")); // 15
    }
    textBrowser->append(tr("Coords:\t%1").arg(coordinates)); // 13
    textBrowser->append(tr("Length:\t%1 amino acids").arg(proteinLength)); // 14
    textBrowser->append("");

    if (helices == 1) {
        textBrowser->append(tr("TMHMM predicted one transmembrane helix in the protein.")); // 16
        textBrowser->append("");
    } else if (helices > 1) {
        textBrowser->append(tr("TMHMM predicted %1 transmembrane helices in the protein.").arg(QString::number(helices))); // 16
        textBrowser->append("");
    }

    if (database->annotationWasLoaded(Database::COGs)) {
        textBrowser->append(tr("COG Annotation:")); // 17
        if (!COGs.isEmpty()) {
            textBrowser->append("\tE-value\tScore\tStart\tEnd\tProfile %");
            foreach (const Domain &COG, COGs) {
                QString hyperlink = QString("<a href=https://ftp.ncbi.nih.gov/pub/COG/COG2014/static/byCOG/%1.html>%1</a>").arg(COG.getAccession());
                QString evalue = COG.getEValue();
                QString bitScore = QString::number(COG.getBitScore());
                QString start = QString::number(COG.getStart());
                QString end = QString::number(COG.getEnd());
                QString relativeLength = QString::number(COG.getRelativeLength());
                textBrowser->append(hyperlink);
                textBrowser->insertPlainText(QString("\t%1\t%2\t%3\t%4\t%5").arg(evalue).arg(bitScore).arg(start).arg(end).arg(relativeLength));
            }
        } else {
            textBrowser->append(tr("No significant COGs matches."));
        }
        textBrowser->append("");
    }

    if (database->annotationWasLoaded(Database::Domains)) {
        textBrowser->append(tr("Domain Annotation:")); // 17
        if (!domains.isEmpty()) {
            textBrowser->append("\tE-value\tScore\tStart\tEnd\tProfile %");
            foreach (const Domain &domain, domains) {
                QString hyperlink = QString("<a href=http://pfam.xfam.org/family/%1>%1</a>").arg(domain.getAccession());
                QString evalue = domain.getEValue();
                QString bitScore = QString::number(domain.getBitScore());
                QString start = QString::number(domain.getStart());
                QString end = QString::number(domain.getEnd());
                QString relativeLength = QString::number(domain.getRelativeLength());
                textBrowser->append(hyperlink);
                textBrowser->insertPlainText(QString("\t%1\t%2\t%3\t%4\t%5").arg(evalue).arg(bitScore).arg(start).arg(end).arg(relativeLength));
            }
        } else {
            textBrowser->append(tr("No significant Pfam-A matches."));
        }
        textBrowser->append("");
    }

    textBrowser->append(tr("Amino Acid Sequence:")); // 19
    textBrowser->setCurrentFont(sequenceFont);
    textBrowser->append(tr(">%1 %2").arg(id).arg(description));
    textBrowser->append(proteinSequence);
    textBrowser->setCurrentFont(defaultFont);
    textBrowser->append("");

    textBrowser->append(tr("Nucleotide Sequence:")); // 20
    textBrowser->setCurrentFont(sequenceFont);
    textBrowser->append(tr(">%1_nucl %2").arg(id).arg(description));
    textBrowser->append(nucleotideSequence);
    textBrowser->setCurrentFont(defaultFont);

    textBrowser->scrollToAnchor("Domain");
}

void RightWidget::annotateDomain(const int &number)
{
    int start = 0;
    if (parent->mode == MainWidget::Domains || parent->mode == MainWidget::DomainArchitectures) {
        start = textBrowser->document()->find(QString("Domain Annotation:")).position() + 1;
    } else if (parent->mode == MainWidget::COGs || parent->mode == MainWidget::COGArchitectures) {
        start = textBrowser->document()->find(QString("COG Annotation:")).position() + 1;
    }
    for (int i = 0; i <= number; ++i) { // number >= 0
        start = textBrowser->toPlainText().indexOf(QString("\n"), start) + 9;
    }

    int end = textBrowser->toPlainText().indexOf(QString("\n"), start);

    QTextCursor cursor(textBrowser->document());
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    cursor.setCharFormat(textFormat);
}

void RightWidget::annotateIntergene(const QString &gi, const int &start, const int &end, Database *database)
{
    QString domain = database->domainOf(gi); // 1
    QString phylum = database->phylumOf(gi); // 2
    QString class_ = database->classOf(gi); // 3
    QString order = database->orderOf(gi); // 4
    QString family = database->familyOf(gi); // 5
    QString accession = database->accessionOf(gi); // 6
    QString definition = database->definitionOf(gi); // 7

    QString positiveSequence = database->forwardSequenceOf(gi, start, end); // 8

    QString negativeSequence = database->reverseComplementTo(positiveSequence); // 9

    textBrowser->clear();

    textBrowser->append(tr("Accession:\t%1").arg(accession)); // 6
    textBrowser->append(tr("Domain:\t%1").arg(domain)); // 1
    textBrowser->append(tr("Phylum:\t%1").arg(phylum)); // 2
    textBrowser->append(tr("Class:\t%1").arg(class_)); // 3
    textBrowser->append(tr("Order:\t%1").arg(order)); // 4
    textBrowser->append(tr("Family:\t%1").arg(family)); // 5
    textBrowser->append(tr("Definition:\t%1").arg(definition)); // 7
    textBrowser->append("");

    // NCBI format for such names is as follows:
    // >NC_000913.3:1175427 - 1176626 Escherichia coli str.K - 12 substr.MG1655, complete genome
    textBrowser->append(tr("Nucleotide Sequence (5'-3'):")); // 8
    textBrowser->setCurrentFont(sequenceFont);
    textBrowser->append(tr(">%1:%2 - %3").arg(accession).arg(start).arg(end));
    textBrowser->append(positiveSequence);
    textBrowser->setCurrentFont(defaultFont);
    textBrowser->append("");

    textBrowser->append(tr("Reverse complement sequence:")); // 8
    textBrowser->setCurrentFont(sequenceFont);
    textBrowser->append(tr(">%1(-1):%2 - %3").arg(accession).arg(start).arg(end));
    textBrowser->append(negativeSequence);
    textBrowser->setCurrentFont(defaultFont);

    textBrowser->scrollToAnchor("Domain");
}

void RightWidget::saveState()
{
    writeSettings();
}

void RightWidget::restoreState()
{
    readSettings();
}

int RightWidget::getCurrentY() const
{
    return currentY;
}

QString RightWidget::getCurrentGi() const
{
    return currentGi;
}

QString RightWidget::getCurrentDomainArchitecture() const
{
    return currentDomainArchitecture;
}

QString RightWidget::getCurrentCOGArchitecture() const
{
    return currentCOGArchitecture;
}

void RightWidget::saveSliderState(int /*value*/)
{
    writeSettings();
}

void RightWidget::readSettings()
{
    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    textBrowser->horizontalScrollBar()->setSliderPosition(settings.value("horizontalScrollBarValue").toInt());
    textBrowser->verticalScrollBar()->setSliderPosition(settings.value("verticalScrollBarValue").toInt());
}

void RightWidget::writeSettings()
{
    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    settings.setValue("horizontalScrollBarValue", textBrowser->horizontalScrollBar()->sliderPosition());
    settings.setValue("verticalScrollBarValue", textBrowser->verticalScrollBar()->sliderPosition());
}
