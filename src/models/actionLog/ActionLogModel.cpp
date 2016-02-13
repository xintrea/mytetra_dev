#include <QDebug>

#include "main.h"
#include "ActionLogModel.h"


ActionLogModel::ActionLogModel(QObject *parent) : QAbstractTableModel(parent)
{

}


ActionLogModel::~ActionLogModel()
{
}


// Число столбцов
int ActionLogModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);

  return ACTIONLOG_COLUMNS;
}


// Число строк
int ActionLogModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);

  return 10;
}


// Получение данных
QVariant ActionLogModel::data(const QModelIndex& index, int role) const
{
  if (index.isValid() && role == Qt::DisplayRole)
    return getCell(index.row(), index.column());

  return QVariant();
}


// Получение значения ячейки, защищенный метод
QVariant ActionLogModel::getCell(int row, int column) const
{
  Q_UNUSED(row);

  switch (column) {
    case ACTIONLOG_COLUMN_TIMESTAMP:
      return QVariant("01:01:01 01.01.2016");

    case ACTIONLOG_COLUMN_ACTION:
      return QVariant("Action");

    default:
      return QVariant();
  }
}


QVariant ActionLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Vertical)
    return QVariant(section + 1);
  else
    switch (section) {
      case 0:
        return QVariant(tr("Time"));

      case 1:
        return QVariant(tr("Action"));

      default:
        return QVariant();
    }
}

