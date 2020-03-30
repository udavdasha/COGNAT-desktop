#include <QtAlgorithms>
#include <QRegExp>
#include "protein.h"

Protein::Protein(const QStringList &fields, const QString &line)
{
    //>gi|15833017|ref|NP_311790.1|-1|3773333..3774355,3774357..3774431|ECs3763|1|peptide chain release factor 2|NC_002695
    gi = fields.value(1);
    ref = fields.value(3);
    strand = fields.value(4);
    coordinates = fields.value(5);
    locus = fields.value(6);
    frame = fields.value(7);
    description = fields.value(8);
    accession = fields.value(9);
    sequence = line;

    domains = QVector<Domain>();
    COGs = QVector<Domain>();

    QStringList list = coordinates.split(QRegExp("\\D+"));
    // If coordinates == "", list = [""].
    // If coordinates == "0", list = ["0"].
    // If coordinates == "1", list = ["1"].
    // If coordinates == "1..2", list = ["1", "2"].
    // If coordinates == "3..4,1", list = ["3", "4", "1"].
    if (!list.value(0).isEmpty()) {
        start = list.value(0).toInt(); // 0, 0, 1, 1, 3
    } else {
        start = list.value(1).toInt();
    }
    if (!list.value(list.size() - 1).isEmpty()) {
        end = list.value(list.size() - 1).toInt(); // 0, 0, 1, 2, 1
    } else {
        end = list.value(list.size() - 2).toInt();
    }
    if (start > end) {
        int index = list.indexOf("1"); // 2
        int separator = list.value(index - 1).toInt(); // 4
        geneLength = end - start + 1 + separator; // 3=1-3+1+4
    } else {
        geneLength = end - start + 1; // 2=2-1+1, 1=1-1+1, 1=0-0+1, 1=0-0+1
    }
    helices = 0;

    examining = false;
}

void Protein::setExamining(bool checked)
{
    examining = checked;
}

void Protein::setHelices(const QString &number)
{
    helices = number.toInt();
}

void Protein::setDomains(const QVector<Domain> &vector)
{
    domains = vector;
}

void Protein::setCOGs(const QVector<Domain> &vector)
{
    COGs = vector;
}

QString Protein::getGi() const
{
    return gi;
}

QString Protein::getId() const
{
    return ref;
}

QString Protein::getStrand() const
{
    return strand;
}

QString Protein::getCoordinates() const
{
    return coordinates;
}

QString Protein::getLocus() const
{
    return locus;
}

QString Protein::getFrame() const
{
    return frame;
}

QString Protein::getDescription() const
{
    return description;
}

QString Protein::getAccession() const
{
    return accession;
}

QString Protein::getSequence() const
{
    return sequence;
}

QString Protein::getDomainArchitecture(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const
{
    QList<Domain> architecture = getDomains(minscore, minlength, maxoverlap, trustscore);
    QString key;
    foreach (const Domain &domain, architecture) {
        key.append(QString("%1|").arg(domain.getAccession()));
    }
    if (!key.isEmpty()) {
        key.remove(key.size() - 1, 1);
    }
    return key;
}

QString Protein::getCOGArchitecture(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const
{
    QList<Domain> architecture = getCOGs(minscore, minlength, maxoverlap, trustscore);
    QString key;
    foreach (const Domain &domain, architecture) {
        key.append(QString("%1|").arg(domain.getAccession()));
    }
    if (!key.isEmpty()) {
        key.remove(key.size() - 1, 1);
    }
    return key;
}

int Protein::getStart() const
{
    return start;
}

int Protein::getEnd() const
{
    return end;
}

int Protein::getGeneLength() const
{
    return geneLength; // geneLength > 0
}

int Protein::getLength() const
{
    return sequence.size();
}

int Protein::getHelices() const
{
    return helices;
}

bool Protein::isExamining() const
{
    return examining;
}

bool Protein::isMembrane() const
{
    if (helices > 0) {
        return true;
    }
    return false;
}

QList<Domain> Protein::getDomains(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const
{
    int j = 0;
    QList<Domain> architecture;
    foreach (const Domain &domain, domains) {
        bool valid = false;
        if (domain.getBitScore() >= minscore && domain.getRelativeLength() >= minlength) {
            valid = true;
            for (int i = 0; i < domains.size(); ++i) {
                if (i == j) {
                    continue;
                }
                Domain neighbor = domains.value(i);
                if (domain.getStart() <= neighbor.getEnd() && domain.getEnd() >= neighbor.getStart()) { // Domains overlap.
                    valid = false;
                    if (domain.getBitScore() >= neighbor.getBitScore()) {
                        valid = true;
                    } else {
                        QList<int> list;
                        list << domain.getStart() << domain.getEnd()
                        << neighbor.getStart() << neighbor.getEnd();
                        qSort(list.begin(), list.end());
                        int overlapLength = list.value(2) - list.value(1) + 1;
                        int percent = overlapLength * 100 / domain.getLength();
                        if (percent <= maxoverlap) {
                            valid = true;
                        } else if (domain.getBitScore() >= trustscore) {
                            valid = true;
                        }
                    }
                }
                if (!valid) {
                    break;
                }
            }
        }
        if (valid) {
            bool inserted = false;
            int i = 0;
            QList<Domain>::iterator iterator;
            for (iterator = architecture.begin(); iterator != architecture.end(); ++iterator) {
                if (architecture.value(i).getStart() > domain.getStart()) {
                    architecture.insert(iterator, domain);
                    inserted = true;
                    break;
                }
                ++i;
            }
            if (!inserted) {
                architecture.append(domain);
            }
        }
        ++j;
    }
    return architecture;
}

QList<Domain> Protein::getCOGs(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const
{
    int j = 0;
    QList<Domain> architecture;
    foreach (const Domain &domain, COGs) {
        bool valid = false;
        if (domain.getBitScore() >= minscore && domain.getRelativeLength() >= minlength) {
            valid = true;
            for (int i = 0; i < COGs.size(); ++i) {
                if (i == j) {
                    continue;
                }
                Domain neighbor = COGs.value(i);
                if (domain.getStart() <= neighbor.getEnd() && domain.getEnd() >= neighbor.getStart()) { // COGs overlap.
                    valid = false;
                    if (domain.getBitScore() >= neighbor.getBitScore()) {
                        valid = true;
                    } else {
                        QList<int> list;
                        list << domain.getStart() << domain.getEnd()
                        << neighbor.getStart() << neighbor.getEnd();
                        qSort(list.begin(), list.end());
                        int overlapLength = list.value(2) - list.value(1) + 1;
                        int percent = overlapLength * 100 / domain.getLength();
                        if (percent <= maxoverlap) {
                            valid = true;
                        } else if (domain.getBitScore() >= trustscore) {
                            valid = true;
                        }
                    }
                }
                if (!valid) {
                    break;
                }
            }
        }
        if (valid) {
            bool inserted = false;
            int i = 0;
            QList<Domain>::iterator iterator;
            for (iterator = architecture.begin(); iterator != architecture.end(); ++iterator) {
                if (architecture.value(i).getStart() > domain.getStart()) {
                    architecture.insert(iterator, domain);
                    inserted = true;
                    break;
                }
                ++i;
            }
            if (!inserted) {
                architecture.append(domain);
            }
        }
        ++j;
    }
    return architecture;
}
