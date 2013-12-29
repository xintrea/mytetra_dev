#include "main.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "AppConfig.h"
#include "MainWindow.h"
#include "RecordTableData.h"
#include "RecordTableModel.h"

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
  return table->getInfoField("name",index.row());
   
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


// Сколько записей в таблице
int RecordTableModel::rowCount(const QModelIndex &parent) const
{
 Q_UNUSED(parent);

 if(table==NULL)
  return 0;

 return table->size();
}


QAbstractListModel *RecordTableModel::getModel(void)
{
 // Возвращается ссылка на модель таблицы конечных записей
 return this;
}


void RecordTableModel::setTableData(RecordTableData *rtData)
{
 table=rtData;
 
 // Модель информирует о том, что данные были кардинально изменены
 reset();
}


RecordTableData *RecordTableModel::getTableData(void)
{
 // Возвращается ссылка на данные, с которыми в данный момент работает модель
 return table;
}


void RecordTableModel::update(void)
{
 qDebug() << "recordtablemodel::update() : Reset";
 
 // Модель информирует о том, что данные были изменены
 // При этом перестроится весь список на экране
 reset();
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

