#ifndef __ATTACHTABLEMODEL_H__
#define __ATTACHTABLEMODEL_H__

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>

#include "AttachTableData.h"

#define ATTACH_COLUMN_FILENAME 0
#define ATTACH_COLUMN_FILESIZE 1
#define ATTACH_COLUMNS 2

#define ATTACH_TABLE_DATA_ROLE           Qt::UserRole+50 // Установка/получение ссылки на таблицу с данными
#define ATTACH_ROLE                      Qt::UserRole+51 // Установка/получение объекта Attach
#define ATTACH_ID_ROLE                   Qt::UserRole+52 // Установка/получение идентификатора аттача
#define ATTACH_COMMAND_RESET_MODEL       Qt::UserRole+53 // Команда перечитывания модели. Используется при изменении данных в data-объекте
#define ATTACH_COMMAND_BEGIN_RESET_MODEL Qt::UserRole+54
#define ATTACH_COMMAND_END_RESET_MODEL   Qt::UserRole+55


// class AttachTableData;

class AttachTableModel : public QAbstractTableModel
{
  Q_OBJECT

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
