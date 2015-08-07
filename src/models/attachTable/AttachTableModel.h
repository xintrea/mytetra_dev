#ifndef __ATTACHTABLEMODEL_H__
#define __ATTACHTABLEMODEL_H__

#include <QObject>
#include <QAbstractTableModel>

#define ATTACH_TABLE_DATA_ROLE Qt::UserRole+50
#define ATTACH_ROLE Qt::UserRole+51
#define ATTACH_ID_ROLE  Qt::UserRole+52

class AttachTableData;


class AttachTableModel : public QAbstractTableModel
{
public:
  AttachTableModel(QObject *parent);
  virtual ~AttachTableModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
  QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

  /*
  void addAttach(Attach& newAttach);
  void delAttach(const QModelIndex & index);
  Attach& getAttach(const QModelIndex & index) const;
  */

private:
  AttachTableData *table;
  QVariant getCell(int row, int column) const;

};

#endif // __ATTACHTABLEMODEL_H__
