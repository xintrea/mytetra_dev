#include <QDebug>
#include <QDateTime>
#include <QCommonStyle>

#include "main.h"
#include "ActionLogModel.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/ActionLogger.h"

extern AppConfig mytetraConfig;
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
  if (index.isValid() && (role == Qt::DisplayRole || role == Qt::DecorationRole))
    return getCell(index.row(), index.column(), role);

  return QVariant();
}


// Получение значения ячейки, защищенный метод
QVariant ActionLogModel::getCell(int row, int column, int role) const
{
  Q_UNUSED(row);

  QDomNode node=doc.elementsByTagName("r").at(row);
  QDomElement element=node.toElement();

  if( element.isNull() )
    return QVariant( QString() );

  switch (column) {
    case ACTIONLOG_COLUMN_TIMESTAMP:

      // Если вывод на экран, то происходит преобразование TIMESTAMP в человекочитаемый формат даты и времени
      if(role==Qt::DisplayRole)
      {
        QDateTime fieldDateTime=QDateTime::fromTime_t( element.attribute("t").toInt() );
        if(mytetraConfig.getEnableCustomDateTimeFormat()==false)
          return fieldDateTime.toString(Qt::SystemLocaleDate);
        else
          return fieldDateTime.toString( mytetraConfig.getCustomDateTimeFormat() );
      }
      else
        return QVariant( element.attribute("t") ); // Время передается в сыром виде (формат TIMESTAMP), чтобы была возможность сортировки

    case ACTIONLOG_COLUMN_ACTION:
      if(role==Qt::DisplayRole)
        return QVariant( actionLogger.getFullDescription(element) );

      // Вывод иконок
      if(role==Qt::DecorationRole)
      {
        if(element.attribute("a")=="syncroProcessError" || element.attribute("a")=="syncroError")
          return QCommonStyle().standardIcon(QStyle::SP_MessageBoxWarning);

        if(element.attribute("a")=="criticalError")
          return QCommonStyle().standardIcon(QStyle::SP_MessageBoxCritical);
      }

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

