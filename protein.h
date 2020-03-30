#ifndef PROTEIN_H
#define PROTEIN_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>
#include "domain.h"

class Protein
{
public:
    Protein(const QStringList &fields = QStringList(), const QString &line = QString());

    void setExamining(bool checked);
    void setHelices(const QString &number);
    void setDomains(const QVector<Domain> &vector);
    void setCOGs(const QVector<Domain> &vector);

    QString getGi() const;
    QString getId() const;
    QString getStrand() const;
    QString getCoordinates() const;
    QString getLocus() const;
    QString getFrame() const;
    QString getDescription() const;
    QString getAccession() const;
    QString getSequence() const;
    QString getDomainArchitecture(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const; // by default = QString()
    QString getCOGArchitecture(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const; // by default = QString()

    int getStart() const;
    int getEnd() const;
    int getGeneLength() const;
    int getLength() const;
    int getHelices() const; // by default = 0

    bool isExamining() const; // by default = false
    bool isMembrane() const; // by default = false

    QList<Domain> getDomains(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const; // by default = QList<Domain>()
    QList<Domain> getCOGs(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const; // by default = QList<Domain>()

private:
    QString gi;
    QString ref;
    QString strand;
    QString coordinates;
    QString locus;
    QString frame;
    QString description;
    QString accession;
    QString sequence;

    QVector<Domain> domains;
    QVector<Domain> COGs;

    int start;
    int end;
    int geneLength;
    int helices;

    bool examining;
};

#endif
