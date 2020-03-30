#include "treedelegate.h"

TreeDelegate::TreeDelegate(const int &width, const int &height, QObject *parent)
    : QItemDelegate(parent)
{
    this->width = width;
    this->height = height;
}

QSize TreeDelegate::sizeHint(const QStyleOptionViewItem & /*option*/, const QModelIndex &index) const
{
    bool DomainOrGi = index.model()->data(index, Qt::SizeHintRole).toBool();
    if (DomainOrGi) {
        return QSize(width, height);
    }
    return QSize(width, height * 0.55);
}
