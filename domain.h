#ifndef DOMAIN_H
#define DOMAIN_H

#include <QString>

class Domain
{
public:
    Domain(const QString &line = QString());

    QString getGi() const;
    QString getAccession() const;
    QString getEValue() const;

    int getBitScore() const;
    int getProfileLength() const;
    int getStart() const;
    int getEnd() const;
    int getLength() const;
    int getRelativeLength() const;

private:
    QString gi;
    QString accession;
    QString evalue;

    int bitScore;
    int profileLength;
    int start;
    int end;
};

#endif
