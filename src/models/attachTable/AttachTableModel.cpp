#include <QDebug>

#include "main.h"
#include "AttachTableModel.h"
#include "Attach.h"
#include "AttachTableData.h"


AttachTableModel::AttachTableModel(QObject *parent) : QAbstractTableModel(parent)
{
  table=NULL;
}


AttachTableModel::~AttachTableModel()
{
  delete table;
}


// Число столбцов
int AttachTableModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);

  return ATTACH_COLUMNS; // Имя файла и размер
}


// Число файлов
int AttachTableModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);

  if(table==NULL)
    return 0;

  return table->size();
}


// Получение данных
QVariant AttachTableModel::data(const QModelIndex& index, int role) const
{
  if (index.isValid() && role == Qt::DisplayRole)
    return getCell(index.row(), index.column());

  if(role==ATTACH_TABLE_DATA_ROLE)
  {
    if(table==NULL)
      return QVariant(0);
    else
    {
      QVariant variant = QVariant::fromValue(table);
      return variant;
    }
  }

  return QVariant();
}


// Получение значения ячейки, защищенный метод
QVariant AttachTableModel::getCell(int row, int column) const
{
  switch (column) {
    case ATTACH_COLUMN_FILENAME:
      return QVariant(table->getFileName(row));
    case ATTACH_COLUMN_FILESIZE:
      return QVariant(table->getFileSize(row));
    default:
      return QVariant();
  }
}


// Сохранение вводимых данных по указанному индексу
bool AttachTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if(role==ATTACH_COMMAND_RESET_MODEL)
  {
    beginResetModel();
    endResetModel();
    return true;
  }

  if(role==ATTACH_COMMAND_BEGIN_RESET_MODEL)
  {
    beginResetModel();
    return true;
  }

  if(role==ATTACH_COMMAND_END_RESET_MODEL)
  {
    endResetModel();
    return true;
  }

  // Если индекс недопустимый
  if(!index.isValid() && role!=ATTACH_TABLE_DATA_ROLE)
    return false;

  // Если происходит установка ссылки на таблицу с данными
  if(role==ATTACH_TABLE_DATA_ROLE)
  {
    beginResetModel();

    // Устанавливается данные переданной по указателю таблицы
    table=value.value<AttachTableDataPointer>();

    if(table!=NULL)
      qDebug() << "Set new data to AttachTableModel with rows: " << table->size();
    else
      qDebug() << "Set new AttachTableModel to NULL data";

    endResetModel();

    return true;
  }

  // Если таблица данных не создана
  if(table==NULL)
    return false;

  // Если таблица пустая
  if(table->size()==0)
    return false;

  // Если происходит редактирование
  if(role==Qt::EditRole)
  {
    // todo: Дописать эту часть

    emit dataChanged(index,index); // Посылается сигнал что данные были изменены

    return true;
  }

  // Во всех остальных случаях
  return false;
}


QVariant AttachTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();
  if (orientation == Qt::Vertical)
    return QVariant(section + 1);
  else
    switch (section) {
      case 0:
        return QVariant(tr("File name"));
      case 1:
        return QVariant(tr("File size"));

      default:
        return QVariant();
    }
}


/*
void AttachTableModel::addAttach(Attach& newAttach)
{
   beginInsertRows(QModelIndex(), attachList->size(), attachList->size());
   attachList->append(newAttach);
   endInsertRows();
}


void AttachTableModel::delAttach(const QModelIndex & index) {
   beginRemoveRows(QModelIndex(), index.row(), index.row());
   attachList->removeAt(index.row());
   endRemoveRows();
}


Attach& AttachTableModel::getAttach(const QModelIndex & index) const
{
  return (*attachList)[index.row()];
}
*/
