#ifndef GBK_H
#define GBK_H

#include <QString>
#include <QVector>

class Gbk
{
public:
    Gbk(const QString &string = QString(), const QVector<QString> &vector = QVector<QString>());

    void setSource(const QString &string);
    void setTaxonomy(const QString &string);
    void setDefinition(const QString &string);

    bool isCircular() const; // by default = false
    bool isComplete() const; // by default = false

    QString getAccession() const;
    QString getDomain() const; // Not const &! (a local variable)
    QString getPhylum() const; // Not const &! (a local variable)
    QString getClass() const; // Not const &! (a local variable)
    QString getOrder() const; // Not const &! (a local variable)
    QString getFamily() const; // Not const &! (a local variable)
    QString getGenus() const; // Not const &! (a local variable)
    QString getSource() const; // by default = QString()
    QString getTaxonomy() const; // by default = QString()
    QString getDefinition() const; // by default = QString()

    QVector<QString> neighborhoodOf(const QString &gi, const int &radius) const; // Not const &! (a local variable)

private:
    bool circular;
    bool complete;

    QString accession;
    QString source;
    QString taxonomy;
    QString definition;

    QVector<QString> giVector;
};

#endif
