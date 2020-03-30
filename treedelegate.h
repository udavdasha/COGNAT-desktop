#ifndef TREEDELEGATE_H
#define TREEDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QSize>
#include <QStyleOptionViewItem>

class TreeDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    TreeDelegate(const int &width, const int &height, QObject *parent = NULL);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    int width;
    int height;
};

#endif
