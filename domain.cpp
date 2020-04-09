#include <QStringList>
#include "domain.h"

Domain::Domain(const QString &line)
{
    QStringList fields = line.split("\t");
    gi = fields.value(0);
    accession = fields.value(1);
    evalue = fields.value(2);
    bitScore = fields.value(3).section(".", 0, 0).toInt();
    profileLength = fields.value(4).toInt();
    start = fields.value(5).toInt();
    end = fields.value(6).toInt();
}

QString Domain::getGi() const
{
    return gi;
}

QString Domain::getAccession() const
{
    return accession;
}

QString Domain::getEValue() const
{
    return evalue;
}

int Domain::getBitScore() const
{
    return bitScore;
}

int Domain::getProfileLength() const
{
    return profileLength;
}

int Domain::getStart() const
{
    return start;
}

int Domain::getEnd() const
{
    return end;
}

int Domain::getLength() const
{
    return end - start + 1;
}

int Domain::getRelativeLength() const
{
    /* An error here was that <end> and <start> was taken in protein coordinates thus their dirrerence can slightly differ from length of profile
       (e.g. in case when a protein contains insertions, and profile covers it from start to end, the length of hit inside a protein could be 
       more than length of profile).
       Now data in the COGNAT database is changed and <profileLength> value becomes <profileCoverage>
    int relativeLength = (end - start + 1) * 100 / profileLength;
    if (relativeLength > 100) {
        relativeLength = 100;
    }
    return relativeLength;
    */
    return profileLength;
}
