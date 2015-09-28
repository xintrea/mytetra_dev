#ifndef __ATTACHTABLEMODEL_H__
#define __ATTACHTABLEMODEL_H__

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QPair>

#include "AttachTableData.h"

#define ATTACHTABLE_COLUMN_FILENAME 0
#define ATTACHTABLE_COLUMN_FILESIZE 1
#define ATTACHTABLE_COLUMNS 2

#define ATTACHTABLE_ROLE_TABLE_DATA           Qt::UserRole+50 // Установка/получение ссылки на таблицу с данными
#define ATTACHTABLE_ROLE_TABLE_DATA_ONLY      Qt::UserRole+51 // Установка ссылки на таблицу с данными без вызова прописывания перекрестной ссылки
#define ATTACHTABLE_ROLE_ATTACH               Qt::UserRole+52 // Установка/получение объекта Attach
#define ATTACHTABLE_ROLE_ID                   Qt::UserRole+53 // Установка/получение идентификатора аттача
#define ATTACHTABLE_COMMAND_RESET_MODEL       Qt::UserRole+54 // Команда перечитывания модели. Используется при изменении данных в data-объекте
#define ATTACHTABLE_COMMAND_BEGIN_RESET_MODEL Qt::UserRole+55
#define ATTACHTABLE_COMMAND_END_RESET_MODEL   Qt::UserRole+56
#define ATTACHTABLE_COMMAND_BEGIN_REMOVE_ROW  Qt::UserRole+57 // Подготовка модели перед удалением одной строки
#define ATTACHTABLE_COMMAND_BEGIN_REMOVE_ROWS Qt::UserRole+58 // Подготовка модели перед удалением набора строк
#define ATTACHTABLE_COMMAND_END_REMOVE_ROW    Qt::UserRole+59
#define ATTACHTABLE_COMMAND_END_REMOVE_ROWS   Qt::UserRole+60

typedef QPair<int, int> typeIntPair;
Q_DECLARE_METATYPE(typeIntPair);

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
  AttachTableData *table; // С какими данными работает модель
  QVariant getCell(int row, int column) const;

};

#endif // __ATTACHTABLEMODEL_H__
