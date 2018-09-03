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
  // delete table; // Закомментировано - удалять здесь table нельзя, так как table существует в рамках коллекции таблиц конечных записей
}


// Число столбцов
int AttachTableModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);

  return ATTACHTABLE_COLUMNS; // Имя файла и размер
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

  // Указатель на связанные с моделью данные
  if(role==ATTACHTABLE_ROLE_TABLE_DATA)
  {
    if(table==NULL)
      return QVariant(0);
    else
      return QVariant::fromValue(table);
  }

  // Идентификатор аттача
  if(role==ATTACHTABLE_ROLE_ID)
  {
    int row=index.row();
    QString id=table->getIdByRow(row);

    return QVariant::fromValue(id);
  }

  // В колонке с именем файла отображается иконка файла или линка
  if(role==Qt::DecorationRole && index.column()==ATTACHTABLE_COLUMN_FILENAME)
  {
    int row=index.row();
    QString id=table->getIdByRow(row);
    QString attachType=table->getAttach(id).getField("type");

    if(attachType=="file")
      return QIcon(":/resource/pic/attach_is_file.svg");

    if(attachType=="link")
      return QIcon(":/resource/pic/attach_is_link.svg");
  }

  return QVariant();
}


// Получение значения ячейки, защищенный метод
QVariant AttachTableModel::getCell(int row, int column) const
{
  switch (column) {
    case ATTACHTABLE_COLUMN_FILENAME:
      return QVariant(table->getFileName(row));
    case ATTACHTABLE_COLUMN_FILESIZE:
      return QVariant(table->getFileSize(row));
    default:
      return QVariant();
  }
}


// Сохранение вводимых данных по указанному индексу
bool AttachTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if(role==ATTACHTABLE_COMMAND_RESET_MODEL)
  {
    beginResetModel();
    endResetModel();
    return true;
  }

  if(role==ATTACHTABLE_COMMAND_BEGIN_RESET_MODEL)
  {
    beginResetModel();
    return true;
  }

  if(role==ATTACHTABLE_COMMAND_END_RESET_MODEL)
  {
    endResetModel();
    return true;
  }

  if(role==ATTACHTABLE_COMMAND_BEGIN_REMOVE_ROW)
  {
    beginRemoveRows(QModelIndex(), value.toInt(), value.toInt());
    return true;
  }

  if(role==ATTACHTABLE_COMMAND_BEGIN_REMOVE_ROWS)
  {
    typeIntPair pair=value.value<typeIntPair>();
    beginRemoveRows(QModelIndex(), pair.first, pair.second);
    return true;
  }

  if(role==ATTACHTABLE_COMMAND_END_REMOVE_ROW || role==ATTACHTABLE_COMMAND_END_REMOVE_ROWS)
  {
    endRemoveRows();
    return true;
  }

  // Если индекс недопустимый
  if(!index.isValid() && role!=ATTACHTABLE_ROLE_TABLE_DATA)
    return false;

  // Если происходит установка ссылки на таблицу с данными
  if(role==ATTACHTABLE_ROLE_TABLE_DATA || role==ATTACHTABLE_ROLE_TABLE_DATA_ONLY)
  {
    beginResetModel();

    // Устанавливается данные переданной по указателю таблицы
    table=value.value<AttachTableDataPointer>();

    if(table!=NULL)
      qDebug() << "Set new data to AttachTableModel with rows: " << table->size();
    else
      qDebug() << "Set new AttachTableModel to NULL data";

    endResetModel();

    // Устанавливается перекрестная ссылка в связанных данных
    if(role==ATTACHTABLE_ROLE_TABLE_DATA && table!=NULL)
      table->setRelatedAttachTableModelOnly(this);

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
