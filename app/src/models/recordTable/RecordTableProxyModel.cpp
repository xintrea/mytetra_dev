#include <QSortFilterProxyModel>
#include <QDebug>

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
    qDebug() << Q_FUNC_INFO << " row: " << row;

    // Начало удаления строки, чтобы был оповещен вид
    // Так делать нельзя, так как внутри QSortFilterProxyModel::removeRow() вид вроде как оповещается
    // И если здесь вызывать beginRemoveRows/endRemoveRows, то вид покажет удаление двух записей
    // вместо одной
    // this->beginRemoveRows(QModelIndex(), row, row);

    bool result=QSortFilterProxyModel::removeRow(row, parent);

    // Завершение удаления строки, в этот момент должен быть оповещен вид
    // this->endRemoveRows();

    return result;
}

