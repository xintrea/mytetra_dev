#include "main.h"
#include "RecordTableModel.h"
#include "RecordTableData.h"

#include "models/tree/TreeItem.h"
#include "models/tree/TreeModel.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/FixedParameters.h"

extern FixedParameters fixedParameters;
extern AppConfig mytetraConfig;


// Конструктор модели.
RecordTableModel::RecordTableModel(QObject *pobj) : QAbstractTableModel(pobj)
{
 // При создании модели она должна брать данные как минимум из
 // пустой таблицы данных
 table=new RecordTableData();
 
 return;
}


// Деструктор модели.
RecordTableModel::~RecordTableModel()
{
 return;
}


// Предоставление данных табличной модели
QVariant RecordTableModel::data(const QModelIndex &index, int role) const
{
  // Если таблица данных не создана
  if(table==NULL)
    return QVariant();

  // Если таблица пустая
  if(table->size()==0)
    return QVariant();

  // Если индекс недопустимый, возвращается пустой объект
  if(!index.isValid())
    return QVariant();

  qDebug() << "RecordTableModel::data(), row:" << index.row() << " column " << index.column();

  // Если запрашивается текст строки для отрисовки или для редактирования
  if(role==Qt::DisplayRole || role==Qt::EditRole)
  {
    QStringList showFields=fixedParameters.recordFieldAvailableList(); // Заменить на показываемые поля

    qDebug() << "RecordTableModel::data(), Show field list: " << showFields;

    // Если длина списка показываемых столбцов меньше или равна номеру запрашиваемого столбца
    if( index.column() < showFields.size() )
    {
      QString fieldName=showFields.value( index.column() );

      qDebug() << "RecordTableModel::data(), fieldName: " << fieldName;

      return table->getField(fieldName, index.row());
    }
  }

   
  // Во всех остальных случаях
  return QVariant();
}


// Сохранение вводимых данных по указанному индексу
bool RecordTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Если таблица данных не создана
  if(table==NULL)
    return false;

  // Если таблица пустая
  if(table->size()==0)
    return false;

  // Если индекс недопустимый
  if(!index.isValid())
    return false;

  // Если запрашивается редактирование
  if(role==Qt::EditRole)
  {
    QStringList showFields=fixedParameters.recordFieldAvailableList(); // TODO: Заменить на показываемые поля

    // Если длина списка показываемых столбцов меньше или равна номеру запрашиваемого столбца
    if( index.column() < showFields.size() )
    {
      QString fieldName=showFields.value( index.column() );

      // Новое значение ячейки записывается в строковую переменную
      QString cellValue;
      cellValue=value.value<QString>();

      // Изменяется поле в таблице конечных записей
      table->setField(fieldName, cellValue, index.row());
   
      emit dataChanged(index,index); // Посылается сигнал что данные были изменены

      return true;
    }
  }

  // Во всех остальных случаях
  return false;
}


// Получение заголовков столбцов и строк
QVariant RecordTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
 QStringList showFields=fixedParameters.recordFieldAvailableList(); // TODO: Заменить на показываемые поля

 QMap<QString, QString> descriptionFields=fixedParameters.recordFieldDescription( showFields );

 // Если ни один столбец не показывается (чего, впринципе не может быть)
 if(showFields.size()==0)
   return QVariant();

 // Если запрашивается заголовок столбца
 if(orientation==Qt::Horizontal && role==Qt::DisplayRole)
 {
   // Если запрашиваемый номер столбца больше количества показываемых
   if(section>showFields.size())
     return QVariant();

   QString fieldName=showFields.value(section);

   return descriptionFields.value(fieldName);
 }


 // Если запрашивается заголовок строки
 if(orientation==Qt::Vertical && role==Qt::DisplayRole)
   return section+1;


 return QAbstractTableModel::headerData(section, orientation, role);
}


// Сколько записей в таблице
int RecordTableModel::rowCount(const QModelIndex &parent) const
{
 Q_UNUSED(parent);

 if(table==NULL)
  return 0;

 return table->size();
}


// Сколько столбцов в таблице
int RecordTableModel::columnCount(const QModelIndex &parent) const
{
 Q_UNUSED(parent);

 return fixedParameters.recordFieldAvailableList().size(); // TODO: Заменить на показываемые поля
}


QAbstractTableModel *RecordTableModel::getModel(void)
{
 // Возвращается ссылка на модель таблицы конечных записей
 return this;
}


void RecordTableModel::setTableData(RecordTableData *rtData)
{
 beginResetModel();

 table=rtData;

 endResetModel();
}


RecordTableData *RecordTableModel::getTableData(void)
{
 // Возвращается ссылка на данные, с которыми в данный момент работает модель
 return table;
}


// Добавление данных
// Функция возвращает позицию нового добавленного элемента
int RecordTableModel::addTableData(int mode,
                                   int pos,
                                   QMap<QString, QString> fields,
                                   QString text,
                                   QMap<QString, QByteArray> files)
{
 beginResetModel(); // Подумать, возможно нужно заменить на beginInsertRows

 // Вставка новых данных в таблицу конечных записей
 int selPos=table->insertNewRecord(mode,
                                   pos,
                                   fields,
                                   text,
                                   files);


 endResetModel(); // Подумать, возможно нужно заменить на endInsertRows

 return selPos;
}


// Удаление записей по указанному списку
void RecordTableModel::removeRowsByList(QVector<int> delIdx)
{
 if(table==NULL) return;

 // Массив сортируется так чтоб вначале были индексы с наибольшим номером
 // Это необходимо чтобы удаление было правильным
 qSort(delIdx.begin(), delIdx.end(), qGreater<int>());

 for(int i=0;i<delIdx.count();i++)
  {
   beginRemoveRows(QModelIndex(), delIdx[i], 1);
   table->deleteRecord(delIdx[i]);
   endRemoveRows();
  }
}

