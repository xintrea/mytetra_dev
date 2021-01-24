#ifndef __ACTIONLOGMODEL_H__
#define __ACTIONLOGMODEL_H__

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QDomDocument>

#define ACTIONLOG_COLUMNS 2
#define ACTIONLOG_COLUMN_TIMESTAMP 0
#define ACTIONLOG_COLUMN_ACTION    1

class ActionLogModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  ActionLogModel(QObject *parent);
  virtual ~ActionLogModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

private:

  QVariant getCell(int row, int column, int role) const;

  QDomDocument doc;
};

#endif // __ACTIONLOGMODEL_H__
