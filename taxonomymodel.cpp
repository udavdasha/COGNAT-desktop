#include <QFont>
#include <QString>
#include <QVector>
#include "database.h"
#include "taxon.h"
#include "taxonomymodel.h"

TaxonomyModel::TaxonomyModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootTaxon = new Taxon(Taxon::Root, tr("Taxonomy Tree"));
}

TaxonomyModel::~TaxonomyModel()
{
    delete rootTaxon;
}

void TaxonomyModel::setTaxonomyData(Database *database) const
{
    QSet<QString> inputSet = database->getInputSet();
    foreach (const QString &gi, inputSet) {

        QString domain = database->domainOf(gi);
        QString phylum = database->phylumOf(gi);
        /*if (phylum.startsWith("Candidatus")) {
            phylum = phylum.section(" ", 1, 1);
        } else if (phylum.startsWith("candidate division")) {
            phylum = phylum.section(" ", 2, 2);
        }*/
        QString class_ = database->classOf(gi);
        /*if (class_.startsWith("Candidatus")) {
            class_ = class_.section(" ", 1, 1);
        } else if (class_.startsWith("candidate division")) {
            class_ = class_.section(" ", 2, 2);
        }*/
        QString source = database->sourceOf(gi);
        if (domain.isEmpty()) {
            domain = tr("unknown");
        }
        if (phylum.isEmpty()) {
            phylum = tr("unknown");
        }
        if (class_.isEmpty()) {
            class_ = tr("unknown");
        }
        if (source.isEmpty()) {
            source = tr("unknown");
        }

        if (!rootTaxon->hasChildNamed(domain)) {
            rootTaxon->appendChild(new Taxon(Taxon::Domain, domain, rootTaxon));
        }

        Taxon *domainTaxon = rootTaxon->getChildNamed(domain);
        if (!domainTaxon->hasChildNamed(phylum)) {
            domainTaxon->appendChild(new Taxon(Taxon::Phylum, phylum, domainTaxon));
        }

        Taxon *phylumTaxon = domainTaxon->getChildNamed(phylum);
        if (!phylumTaxon->hasChildNamed(class_)) {
            phylumTaxon->appendChild(new Taxon(Taxon::Class, class_, phylumTaxon));
        }

        Taxon *classTaxon = phylumTaxon->getChildNamed(class_);
        if (!classTaxon->hasChildNamed(source)) {
            classTaxon->appendChild(new Taxon(Taxon::Source, source, classTaxon));
        }

        Taxon *sourceTaxon = classTaxon->getChildNamed(source);
        if (!sourceTaxon->hasChildNamed(gi)) {
            bool childShouldBeAppended = true;
            QVector<QString> names = sourceTaxon->getChildrenNames(); // Here are appended gis.
            foreach (const QString &name, names) {
                if (database->maxNeighborhoodOf(name).contains(gi)) { // Checking whether a gi is related to an appended gi.
                    childShouldBeAppended = false;
                }
            }
            if (childShouldBeAppended) {
                sourceTaxon->appendChild(new Taxon(Taxon::Gene, gi, sourceTaxon));
            }
        }
    }
}

int TaxonomyModel::rowCount(const QModelIndex &parent) const
{
    Taxon *parentTaxon = taxonFromIndex(parent);
    return parentTaxon->childrenCount();
}

int TaxonomyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

QModelIndex TaxonomyModel::index(int row, int column, const QModelIndex &parent) const
{
    Taxon *parentTaxon = taxonFromIndex(parent);
    Taxon *childTaxon = parentTaxon->getChild(row);
    if (childTaxon) {
        return createIndex(row, column, childTaxon);
    }
    return QModelIndex();
}

QModelIndex TaxonomyModel::parent(const QModelIndex &index) const
{
    Taxon *taxon = taxonFromIndex(index);
    if (taxon != rootTaxon) {
        Taxon *parentTaxon = taxon->getParent();
        if (parentTaxon != rootTaxon) {
            return createIndex(parentTaxon->row(), 0, parentTaxon);
        }
    }
    return QModelIndex();
}

QVariant TaxonomyModel::data(const QModelIndex &index, int role) const
{
    Taxon *taxon = taxonFromIndex(index);
    if (role == Qt::AccessibleTextRole) {
        return taxon->getName();
    } else if (role == Qt::DisplayRole) {
        switch (taxon->rank) {
        case Taxon::Gene:
            return QString();
        default:
            return taxon->getName();
        }
    } else if (role == Qt::FontRole) {
        if (taxon->getName() != "unknown") {
            QFont font;
            switch (taxon->rank) {
            case Taxon::Domain:
                font.setBold(true);
                return font;
            case Taxon::Phylum:
                font.setBold(true);
                return font;
            case Taxon::Genus:
                font.setItalic(true);
                return font;
            case Taxon::Species:
                font.setItalic(true);
                return font;
            case Taxon::Source:
                font.setItalic(true);
                return font;
            default:
                return QFont();
            }
        }
    } else if (role == Qt::SizeHintRole) {
        switch (taxon->rank) {
        case Taxon::Domain:
            return true;
        case Taxon::Gene:
            return true;
        default:
            return false;
        }
    } else if (role == Qt::UserRole) {
        switch (taxon->rank) {
        case Taxon::Root:
            return true;
        default:
            return false;
        }
    }
    return QVariant();
}

QVariant TaxonomyModel::headerData(int /*section*/, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootTaxon->getName();
    }
    return QVariant();
}

Taxon *TaxonomyModel::taxonFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<Taxon *>(index.internalPointer());
    } else {
        return rootTaxon;
    }
}
