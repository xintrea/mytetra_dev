#include "main.h"
#include "RecordTableModel.h"
#include "RecordTableData.h"

#include "models/tree/TreeItem.h"
#include "models/tree/TreeModel.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"

// #include <QDom>


extern AppConfig mytetraconfig;


// Конструктор модели.
RecordTableModel::RecordTableModel(QObject *pobj) : QAbstractListModel(pobj)
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


// Предоставление данных модели
// В таблице конечных записей на экране перечисляются только имена записей
QVariant RecordTableModel::data(const QModelIndex &index, int nRole) const
{
 if(table==NULL) return QVariant();

 // Если индекс недопустимый, возвращается пустой объект
 if(!index.isValid()) return QVariant();
 
 // Если запрашивается текст строки для отрисовки или для редактирования
 if(nRole==Qt::DisplayRole || nRole==Qt::EditRole)
  return table->getField("name",index.row());
   
 // Во всех остальных случаях
 return QVariant();
}


// Сохранение вводимых данных по указанному индексу
bool RecordTableModel::setData(const QModelIndex &index, const QVariant &value, int nRole)
{
 if(table==NULL) return false;

 // Если индекс недопустимый, возвращается пустой объект
 if(!index.isValid()) return false;

 // Если запрашивается редактирование
 if(nRole==Qt::EditRole)
  {
   // Новое значение строки записывается в строковую переменную 
   QString lineName;
   lineName=value.value<QString>();

   // Изменяется поле в таблице конечных записей
   table->setField("name",lineName,index.row());
   
   emit dataChanged(index,index); // Посылается сигнал что данные были изменены

   return true;
  }

 // Во всех остальных случаях
 return false;
}



QVariant RecordTableModel::headerData(int section, Qt::Orientation orientation, int nRole) const
{
 QStringList availableFields=table->fieldNameAvailableList();

 QMap<QString, QString> mapFields=table->fieldDescription( QStringList );

 QString result;
 for(int i=0; i<=section; i++)
   result=mapFields









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

 return table->fieldNameAvailableList()->count();
}


QAbstractListModel *RecordTableModel::getModel(void)
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

