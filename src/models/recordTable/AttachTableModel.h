#ifndef __ATTACHTABLEMODEL_H__
#define __ATTACHTABLEMODEL_H__

#include <QObject>
#include <QAbstractTableModel>

#include "Attach.h"


class AttachTableModel : public QAbstractTableModel
{
public:
  AttachTableModel(QObject *parent);
  virtual ~AttachTableModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

  void addAttach(Attach& newAttach);
  void delAttach(const QModelIndex & index);
  Attach& getAttach(const QModelIndex & index) const;

private:
  QList<Attach> *attachList;
  QVariant getData(int num, int position) const;

};

#endif // __ATTACHTABLEMODEL_H__
