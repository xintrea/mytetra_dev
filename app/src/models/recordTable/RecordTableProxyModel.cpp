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

