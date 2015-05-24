#ifndef __RECORDTABLEPROXYMODEL_H__
#define __RECORDTABLEPROXYMODEL_H__

#include <QSortFilterProxyModel>


class RecordTableProxyModel : public QSortFilterProxyModel
{
public:
  RecordTableProxyModel(QObject *pobj);
  ~RecordTableProxyModel();

};

#endif // __RECORDTABLEPROXYMODEL_H__
