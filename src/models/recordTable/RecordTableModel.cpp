#include "main.h"
#include "Record.h"
#include "RecordTableModel.h"
#include "RecordTableData.h"

#include "models/tree/TreeItem.h"
#include "models/tree/TreeModel.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/FixedParameters.h"

extern FixedParameters fixedParameters;
extern AppConfig mytetraConfig;


// Конструктор модели
RecordTableModel::RecordTableModel(QObject *pobj) : QAbstractTableModel(pobj)
{
 // При создании модели она должна брать данные как минимум из
 // пустой таблицы данных
 table=new RecordTableData();
 
 return;
}


// Деструктор модели
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

  // qDebug() << "RecordTableModel::data(), row:" << index.row() << " column " << index.column();

  // Если запрашивается текст строки для отрисовки или для редактирования
  if(role==Qt::DisplayRole || role==Qt::EditRole || role==SORT_ROLE)
  {
    QStringList showFields=mytetraConfig.getRecordTableShowFields();

    // Если длина списка показываемых столбцов меньше или равна номеру запрашиваемого столбца
    if( index.column() < showFields.size() )
    {
      QString fieldName=showFields.value( index.column() );
      QString field=table->getField(fieldName, index.row());


      // Некоторые данные при отрисовке в таблице преобразуются в "экранные" представления
      // Преобразование возможно только для отображаемой в таблице информации

      if( role==Qt::DisplayRole && fieldName=="ctime")
      {
        // Преобразование временного штампа в дату и время
        QDateTime fieldDateTime=QDateTime::fromString(field, "yyyyMMddhhmmss");
        if(mytetraConfig.getEnableCustomDateTimeFormat()==false)
          return fieldDateTime.toString(Qt::SystemLocaleDate);
        else
          return fieldDateTime.toString( mytetraConfig.getCustomDateTimeFormat() );
      }

      else if( role==Qt::DisplayRole && fieldName=="hasAttach") // Наличие аттачей
      {
        if(field=="0")
          return ""; // Если аттачей нет, выводится пустая строка. Это повышает читабельность
        else
          return tr("...");
      }

      else if( role==Qt::DisplayRole && fieldName=="attachCount") // Количество аттачей
      {
        if(field=="0")
          return ""; // Если количество аттачей нуливое, выводится пустая строка. Это повышает читабельность
        else
          return field;
      }

      else if( role==Qt::DisplayRole && fieldName=="block") // Наличие блокировки записи
      {
        if(field!="1")
          return "";
        else
          return tr("...");
      }
      else
        return field;
    }
  }

  if(role==RECORD_ID_ROLE)
    return table->getField("id", index.row());

  if(role==RECORD_BLOCK_ROLE)
    return table->getField("block", index.row());

  // Подсветка заднего фона
  if(role==Qt::BackgroundRole)
  {
    if( mytetraConfig.getEnableRecordWithAttachHighlight() )
      if( table->getField("hasAttach", index.row())=="1" )
      {
        QColor color( mytetraConfig.getRecordWithAttachHighlightColor() );
        return QBrush(color);
      }
  }


  // Оформление иконками
  if(role == Qt::DecorationRole)
  {
    QStringList showFields=mytetraConfig.getRecordTableShowFields();
    QString fieldName=showFields.value( index.column() );
    QString field=table->getField(fieldName, index.row());

    // Иконка блокировки в названии записи
    if(fieldName=="name")
      if( !showFields.contains("block") ) // Среди отображаемых столбцов нет столбца "block" (чтобы не отрисовывалось два замочка в строке)
        if( table->getField("block", index.row())=="1" ) // Если есть блокировка
          return QIcon(":/resource/pic/note_block.svg");

    // Иконка наличия аттачей в специально предназначенном для этого столбце
    if(fieldName=="hasAttach" && field=="1")
      return QIcon(":/resource/pic/attach.svg");

    // Иконка блокировки в специально предназначенном для этого столбце
    if(fieldName=="block" && field=="1")
      return QIcon(":/resource/pic/note_block.svg");
  }


  // Если происходит запрос ссылки на таблицу данных
  /*
  if(role==TABLE_DATA_ROLE)
  {
    QVariant var;
    var.setValue<RecordTableDataPointer>( this->getTableData() );
    return var;
  }
  */
   
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

  // Если происходит редактирование
  if(role==Qt::EditRole)
  {
    // QStringList showFields=fixedParameters.recordFieldAvailableList(); // TODO: Заменить на показываемые поля
    QStringList showFields=mytetraConfig.getRecordTableShowFields();

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

  /*
  // Если происходит запись во всю таблицу данных
  if(role==TABLE_DATA_ROLE)
  {
   this->setTableData( qVariantFromValue(value) );
   return true;
  }

  // Если происходит запись одной строки
  if(role==ONE_RECORD_ROLE)
  {
    this->setTableData( qVariantFromValue(value) );
    return true;
  }
  */

  // Во всех остальных случаях
  return false;
}


// Получение заголовков столбцов и строк
QVariant RecordTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
 // QStringList showFields=fixedParameters.recordFieldAvailableList(); // TODO: Заменить на показываемые поля
 QStringList showFields=mytetraConfig.getRecordTableShowFields();

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

 static int previousColumnCount=0;

 int currentColumnCount=mytetraConfig.getRecordTableShowFields().size();

 if( currentColumnCount!=previousColumnCount)
 {
   qDebug() << "Column count change. New column count: " << currentColumnCount;
   previousColumnCount=currentColumnCount;
 }

 return currentColumnCount;
}


// Удаление строк в таблице
// note: Переопределение метода removeRows() влияет и на метод removeRow(),
// так как он просто вызывает removeRows() для удаления одной строки
bool RecordTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
  Q_UNUSED(parent);


  if(row<0 || row>=rowCount() || (row+count-1)<0 || (row+count-1)>=rowCount())
  {
    criticalError("Bad arguments in RecordTableModel::removeRows(). row: "+QString::number(row)+" count: "+QString::number(count));
    return false;
  }


  beginRemoveRows(QModelIndex(), row, row+count-1);

  // Удаляются строки непосредственно в таблице
  for(int i=row; i<row+count; ++i)
    table->deleteRecord(i);

  endRemoveRows();

  return true;
}


// Установка данных в таблицу данных
void RecordTableModel::setTableData(RecordTableData *rtData)
{
 beginResetModel();

 table=rtData;

 endResetModel();
}


// Получение ссылки на таблицу данных
RecordTableData *RecordTableModel::getTableData(void)
{
 // Возвращается ссылка на данные, с которыми в данный момент работает модель
 return table;
}


// Добавление данных
// Функция возвращает позицию нового добавленного элемента
int RecordTableModel::addTableData(int mode,
                                   QModelIndex posIndex,
                                   Record record)
{
 beginResetModel(); // Подумать, возможно нужно заменить на beginInsertRows

 // Вставка новых данных в таблицу конечных записей
 int selPos=table->insertNewRecord(mode,
                                   posIndex.row(),
                                   record);

 endResetModel(); // Подумать, возможно нужно заменить на endInsertRows

 return selPos;
}


void RecordTableModel::onRecordTableConfigChange(void)
{
  beginResetModel();
  endResetModel();
}

