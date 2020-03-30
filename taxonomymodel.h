#ifndef TAXONOMYMODEL_H
#define TAXONOMYMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class Database;
class Taxon;

class TaxonomyModel : public QAbstractItemModel
{
public:
    TaxonomyModel(QObject *parent = NULL);
    ~TaxonomyModel();

    void setTaxonomyData(Database *database) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    Taxon *taxonFromIndex(const QModelIndex &index) const;

    Taxon *rootTaxon;
};

#endif
