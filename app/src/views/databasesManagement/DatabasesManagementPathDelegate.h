#ifndef DATABASESMANAGEMENTPATHDELEGATE_H
#define DATABASESMANAGEMENTPATHDELEGATE_H

#include <QStyledItemDelegate>

//! Делегат для отображения пути к директрии в таблице
//! Данный делегат всегда показывает конец строки а не ее начало,
//! чтобы в длинных путях было видно какая директория написана

class DatabasesManagementPathDelegate : public QStyledItemDelegate
{
public:

    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

#endif // DATABASESMANAGEMENTPATHDELEGATE_H
