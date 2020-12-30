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

    bool result=QSortFilterProxyModel::removeRow(row, parent);

    return result;
}

