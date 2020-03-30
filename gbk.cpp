#include <QRegExp>
#include "gbk.h"

Gbk::Gbk(const QString &string, const QVector<QString> &vector)
{
    circular = false;
    complete = false;

    accession = string;
    source = QString();
    taxonomy = QString();
    definition = QString();

    giVector = vector;
}

void Gbk::setSource(const QString &string)
{
    source = string;
}

void Gbk::setTaxonomy(const QString &string)
{
    taxonomy = string;
    if (taxonomy.startsWith("Archaea") || taxonomy.startsWith("Bacteria")) {
        circular = true;
    }
}

void Gbk::setDefinition(const QString &string)
{
    definition = string;
    QRegExp completeness(".*complete.*");
    QRegExp incompleteness(".*complete cds.*");
    if (completeness.indexIn(definition) >= 0 && incompleteness.indexIn(definition) < 0) {
        complete = true;
    }
}

bool Gbk::isCircular() const
{
    return circular;
}

bool Gbk::isComplete() const
{
    return complete;
}

QString Gbk::getAccession() const
{
    return accession;
}

QString Gbk::getDomain() const
{
    return taxonomy.section("; ", 0, 0);
}

QString Gbk::getPhylum() const
{
    return taxonomy.section("; ", 1, 1);
}

QString Gbk::getClass() const
{
    return taxonomy.section("; ", 2, 2);
}

QString Gbk::getOrder() const
{
    return taxonomy.section("; ", 3, 3);
}

QString Gbk::getFamily() const
{
    return taxonomy.section("; ", 4, 4);
}

QString Gbk::getGenus() const
{
    if (source.startsWith("Candidatus")) {
        return source.section(" ", 1, 1);
    } else if (source.startsWith("candidate division")) {
        return source.section(" ", 2, 2);
    }
    return source.section(" ", 0, 0);
}

QString Gbk::getSource() const
{
    return source;
}

QString Gbk::getTaxonomy() const
{
    return taxonomy;
}

QString Gbk::getDefinition() const
{
    return definition;
}

QVector<QString> Gbk::neighborhoodOf(const QString &gi, const int &radius) const
{
    int baseIndex = giVector.indexOf(gi);
    int startIndex = baseIndex - radius;
    int endIndex = baseIndex + radius;
    QVector<QString> vector;
    if (isCircular() && isComplete()) {
        if (giVector.size() <= radius * 2 + 1) {
            int avaliableRadius = giVector.size() / 2;
            if (baseIndex == avaliableRadius || giVector.size() < 3) { // baseIndex=2, avaliableRadius=2, giVector=| | |*| |
                return giVector;
            } else if (baseIndex < avaliableRadius) { // baseIndex=1, avaliableRadius=2, giVector=| |*| | |
                startIndex = baseIndex - avaliableRadius + giVector.size();
                endIndex = startIndex - 1;
            } else if (baseIndex > avaliableRadius) { // baseIndex=3, avaliableRadius=2, giVector=| | | |*|
                startIndex = baseIndex - avaliableRadius;
                endIndex = startIndex - 1;
            }
            for (int i = startIndex; i < giVector.size(); ++i) {
                vector.append(giVector.value(i));
            }
            for (int i = 0; i <= endIndex; ++i) {
                vector.append(giVector.value(i));
            }
            return vector;
        } else {
            if (startIndex >= 0 && endIndex < giVector.size()) {
                for (int i = startIndex; i <= endIndex; ++i) {
                    vector.append(giVector.value(i));
                }
                return vector;
            } else if (startIndex < 0) {
                startIndex = baseIndex - radius + giVector.size();
                endIndex = baseIndex + radius;
            } else if (endIndex >= giVector.size()) {
                startIndex = baseIndex - radius;
                endIndex = baseIndex + radius - giVector.size();
            }
            for (int i = startIndex; i < giVector.size(); ++i) {
                vector.append(giVector.value(i));
            }
            for (int i = 0; i <= endIndex; ++i) {
                vector.append(giVector.value(i));
            }
            return vector;
        }
    } else {
        if (startIndex < 0) {
            startIndex = 0;
        }
        if (endIndex >= giVector.size()) {
            endIndex = giVector.size() - 1;
        }
        for (int i = startIndex; i <= endIndex; ++i) {
            vector.append(giVector.value(i));
        }
        return vector;
    }
    return giVector;
}
