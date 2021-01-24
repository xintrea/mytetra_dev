#ifndef __RECORDTABLEPROXYMODEL_H__
#define __RECORDTABLEPROXYMODEL_H__

#include <QSortFilterProxyModel>


class RecordTableProxyModel : public QSortFilterProxyModel
{
public:
  RecordTableProxyModel(QObject *pobj=nullptr);
  ~RecordTableProxyModel();

  // Метод удаления строк переопределен, чтобы в нем вызывалось обновление вида
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());

};

#endif // __RECORDTABLEPROXYMODEL_H__
