#include "taxon.h"

Taxon::Taxon(Rank rank, const QString &name, Taxon *parent)
{
    this->rank = rank;
    this->name = name;
    this->parent = parent;
    children = QList<Taxon *>();
}

Taxon::~Taxon()
{
    qDeleteAll(children);
}

void Taxon::appendChild(Taxon *child)
{
    children.append(child);
}

QString Taxon::getName() const
{
    return name;
}

QVector<QString> Taxon::getChildrenNames() const
{
    QVector<QString> names;
    foreach (Taxon *child, children) {
        names.append(child->getName());
    }
    return names;
}

Taxon *Taxon::getParent() const
{
    return parent;
}

Taxon *Taxon::getChild(const int &row) const
{
    return children.value(row);
}

Taxon *Taxon::getChildNamed(const QString &name) const
{
    foreach (Taxon *child, children) {
        if (child->getName() == name) {
            return child;
        }
    }
    return NULL;
}

int Taxon::childrenCount() const
{
    return children.size();
}

int Taxon::row() const
{
    if (parent) {
        return parent->children.indexOf(const_cast<Taxon *>(this));
    }
    return - 1;
}

bool Taxon::hasChildNamed(const QString &name) const
{
    foreach (Taxon *child, children) {
        if (child->getName() == name) {
            return true;
        }
    }
    return false;
}
