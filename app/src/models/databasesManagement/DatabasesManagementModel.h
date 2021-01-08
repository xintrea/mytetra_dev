#ifndef DATABASESMANAGEMENTMODEL_H
#define DATABASESMANAGEMENTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>

#define DBMANAGEMENT_COLUMNS 3
#define DBMANAGEMENT_COLUMN_SELECT 0
#define DBMANAGEMENT_COLUMN_DBPATH 1
#define DBMANAGEMENT_COLUMN_TRASHPATH 2


class DatabasesManagementModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DatabasesManagementModel(QObject *parent);
  virtual ~DatabasesManagementModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

private:

  QVariant getCell(int row, int column, int role) const;
};

#endif // DATABASESMANAGEMENTMODEL_H
