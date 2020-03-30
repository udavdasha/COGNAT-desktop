#ifndef TAXON_H
#define TAXON_H

#include <QList>
#include <QString>
#include <QVector>

class Taxon
{
public:
    enum Rank { Root, Domain, Phylum, Class, Order, Family, Genus, Species, Source, Gene };

    Taxon(Rank rank, const QString &name = QString(), Taxon *parent = NULL);
    ~Taxon();

    void appendChild(Taxon *child);

    QString getName() const;

    QVector<QString> getChildrenNames() const;

    Taxon *getParent() const;
    Taxon *getChild(const int &row) const;
    Taxon *getChildNamed(const QString &name) const;

    int childrenCount() const;
    int row() const;

    bool hasChildNamed(const QString &name) const;

    Rank rank;

private:
    QString name;

    Taxon *parent;

    QList<Taxon *> children;
};

#endif
