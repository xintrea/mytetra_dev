#include <QDebug>

#include "main.h"
#include "ActionLogModel.h"
#include "libraries/ActionLogger.h"


extern ActionLogger actionLogger;


ActionLogModel::ActionLogModel(QObject *parent) : QAbstractTableModel(parent)
{
  doc.setContent( actionLogger.getXml() );
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

  return doc.elementsByTagName("r").length();
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

  QDomNode node=doc.elementsByTagName("r").at(row);
  QDomElement element=node.toElement();

  if( element.isNull() )
    return QVariant( QString() );

  switch (column) {
    case ACTIONLOG_COLUMN_TIMESTAMP:
      return QVariant( element.attribute("t") );

    case ACTIONLOG_COLUMN_ACTION:
      return QVariant( actionLogger.getFullDescription(element) );

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

