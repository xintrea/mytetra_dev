#include <QSortFilterProxyModel>

#include "main.h"
#include "RecordTableProxyModel.h"


// Конструктор модели
RecordTableProxyModel::RecordTableProxyModel(QObject *pobj) : QSortFilterProxyModel(pobj)
{
  // this->setSourceModel(this);

}

// Деструктор модели
RecordTableProxyModel::~RecordTableProxyModel()
{
    return;
}


bool RecordTableProxyModel::removeRow(int row, const QModelIndex &parent)
{
    // Начало удаления строки, чтобы был оповещен вид
    this->beginRemoveRows(QModelIndex(), row, row);

    bool result=QSortFilterProxyModel::removeRow(row, parent);

    // Завершение удаления строки, в этот момент должен быть оповещен вид
    this->endRemoveRows();

    return result;
}

