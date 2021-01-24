#include "MtStyledItemDelegate.h"

// Этот класс не используется, скорее всего будет удален

// Делегат, использующийся для QTableView чтобы стилизовать отступы,
// так как через QSS отступы для QTableView не стилизуются

MtStyledItemDelegate::MtStyledItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QSize MtStyledItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    size.rheight() = 45;

    return size;
}
