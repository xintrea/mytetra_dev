#include <QStringList>
#include <QString>
#include <QMap>

#include "TreeItem.h"
#include "main.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;



TreeItem::TreeItem(const QMap<QString, QString> &data, TreeItem *parent)
{
 parentItem = parent;
 fieldsTable = data;
}


TreeItem::~TreeItem()
{
 // В родительском объекте ссылка на данный объект удаляется
 // Подумать, нужно ли это действие
 // Вроде ненужно, но может понадобится вдальнейшем
 // if(parentItem)parentItem->removeChildrenLink(childNumber(),1);

 // Вызывается процедура очищения ветки без физического удаления данных на диске
 empty();
}


// Возвращение ссылки на потомка, который хранится в списке childItems
// под указанным номером
TreeItem *TreeItem::child(int number)
{
 return childItems.value(number);
}


// Возвращение количества потомков (т.е. количество записей в списке childItems)
int TreeItem::childCount() const
{
 return childItems.count();
}


// Возвращает номер, под которым данный объект хранится
// в массиве childItems своего родителя
int TreeItem::childNumber() const
{
 if (parentItem)
  return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

 return 0;
}


// Метод мягкого удаления всех данных из узла
// При таком удалении полностью очищается сам узел, 
// а физические данные на диске не затрагиваются
void TreeItem::empty(void)
{
 fieldsTable.clear();

 recordsTable.empty();

 // Удаляются все подветки
 qDeleteAll(childItems);
 
 parentItem=NULL;
}


int TreeItem::fieldCount() const
{
 return fieldsTable.count();
}


QString TreeItem::getField(QString name)
{

 // Если запрашивается динамическое имя из имени и количества потомков
 if(name=="dynamicname")
  {
   // Имя ветки
   QString itemName=fieldsTable["name"];

   // Если есть шифрование в этой ветке
   // и поле является зашифрованным
   if(fieldsTable.contains("crypt"))
    if(fieldsTable["crypt"]=="1")
     {
      if(globalParameters.getCryptKey().length()>0)
       itemName=decryptString(globalParameters.getCryptKey(), itemName);
      else
       itemName=QString(QObject::tr("Closed"));
     }

   // Выясняется, есть ли у текущего элемента конечные записи
   int recordCount=this->recordtableGetRowCount();

   // Если конечных элементов нет, возвращатся просто имя
   if(recordCount==0)
    return itemName;
   else
    {
     // Иначе конечные элементы есть, возвращается имя записи
     // и количество конечных элементов
     QString r,i;
     r=itemName+" ["+i.setNum(recordCount)+"]";
     return r;
    }
   }


 // Если имя поля допустимо
 if(isFieldNameAvailable(name))
  {
   // Если поле с таким именем существует
   if(fieldsTable.contains(name))
    {
     QString value=fieldsTable[name];

     // Если есть шифрование
     // и поле является зашифрованным
     if(fieldsTable.contains("crypt"))
      if(fieldsTable["crypt"]=="1")
       if(fieldNameForCryptList().contains(name))
        {
         if(globalParameters.getCryptKey().length()>0 &&
            value!="")
          value=decryptString(globalParameters.getCryptKey(), value);
         else
          value="";
        }

     return value;
    }
   else
    return QString(""); // Если поле не существует, возвращается пустая строка
  }
 else
  {
   critical_error("In TreeItem::get_field() unavailable name '"+name+"'");
   exit(1);
   return "";
  }
}


// Получение всех установленных полей "имя_поля"->"значение"
QMap<QString, QString> TreeItem::getAllFields()
{
 qDebug() << "TreeItem::getAllFields() : Fields data " << fieldsTable;

 QMap<QString, QString> result;

 QList<QString> names=fieldsTable.keys();

 foreach(QString name, names)
  {
   // В результат добаляются только параметры с разрешенным именем
   if(isFieldNameAvailable(name))
     result[name]=getField(name);
  }

 return result;
}


// Получение всех установленных полей "имя_поля"->"значение"
// Напрямую, без преобразований, без расшифровки
QMap<QString, QString> TreeItem::getAllFieldsDirect()
{
 return fieldsTable;
}


// Установка данных
// Первый параметр - имя поля
// Второй параметр - устанавливаемое значение
void TreeItem::setField(QString name, QString value)
{
 if(isFieldNameAvailable(name))
  {
   // Если поле нужно шифровать
   // и поле является зашифрованным
   if(fieldsTable.contains("crypt"))
    if(fieldsTable["crypt"]=="1")
     if(fieldNameForCryptList().contains(name))
      {
       // Если установлен пароль
       if(globalParameters.getCryptKey().length()>0)
        {
         // Если поле непустое, поле зашифровывается
         if(value!="")
          value=encryptString(globalParameters.getCryptKey(), value);
        }
       else // Иначе пароль не установлен
        critical_error("TreeItem::setField() : Can not encrypt field \""+name+"\". Password not setted.");
      }
   
   // qDebug() << "Set to item data " << name << value;
   fieldsTable[name]=value;
  } 
 else
  critical_error("TreeItem::setField() : Set unavailable field \""+ name +"\" to item of branch tree");
}


// Установка данных напрямую - какие данные переданы, те и запомнятся
// без всяких преобразований, без шифрации
void TreeItem::setFieldDirect(QString name, QString value)
{
 if(isFieldNameAvailable(name))
  {
   fieldsTable[name]=value;
  }
 else
  critical_error("TreeItem::setFieldDirect() : Set unavailable field \""+ name +"\" to item of branch tree");
}


bool TreeItem::isFieldNameAvailable(QString name) const
{
 if(fieldNameAvailableList().contains(name))
  return true;
 else
  return false;
}


QStringList TreeItem::fieldNameAvailableList(void) const
{
 QStringList names;

 names << "id";
 names << "name";
 names << "ctime";
 names << "crypt";

 return names;
}


QStringList TreeItem::fieldNameForCryptList(void) const
{
 QStringList names;

 names << "name";

 return names;
}


TreeItem *TreeItem::parent()
{
 return parentItem;
}


QString TreeItem::getId()
{
 if(fieldsTable.contains("id"))
  return (fieldsTable["id"]);
 else
  {
   critical_error("In TreeItem data getting field with unavailable name 'id'");
   exit(1);
   return "";
  }
}


QString TreeItem::getParentId()
{
 if(parentItem!=NULL)
  {
   return parentItem->getField("id");
  }
 else
  return "";
}


// Добавление новых подчиненных элементов
// position - позиция в списке подчиненных элементов для вставки элементов
// count - сколько новых элементов будет вставлено
// columns - сколько колонок содержит добавляемый элемент
bool TreeItem::insertChildren(int position, int count, int columns)
{
  Q_UNUSED(columns);

  if(position < 0 || position > childItems.size())
   return false;

  for(int row = 0; row < count; ++row)
   {
    QMap<QString, QString> data;
    TreeItem *item = new TreeItem(data, this); // Создается объект item
    childItems.insert(position, item); // Вставка item в нужную позицию массива childItems
   }

  return true;
}


// Добавление нового подчиненного элемента 
// в конец списка подчиненных элементов
bool TreeItem::addChildren(void)
{
 QMap<QString, QString> data;

 TreeItem *item = new TreeItem(data, this); // Создается объект item

 childItems << item; // Добавление item в конец массива childItems

 return true;
}


bool TreeItem::removeChildren(int position, int count)
{
 if(position < 0 || position + count > childItems.size())
  return false;

 for(int row = 0; row < count; ++row)
  delete childItems.takeAt(position);

 return true;
}


void TreeItem::removeAllChildren()
{
 for(int i=0; i< childItems.size(); i++)
  delete childItems.takeAt(i);
}


bool TreeItem::removeChildrenLink(int position, int count)
{
 if(position < 0 || position + count > childItems.size())
  return false;

 // Ссылка на удаленный элемент убирается из списка подчиненных элементов
 for (int row = 0; row < count; ++row)
  childItems.removeAt(position);

 return true;
}


bool TreeItem::moveUp(void)
{
 // Выясняется номер данного элемента в списке родителя
 int num=childNumber();

 // Если двигать вверх некуда, ничего делать ненужно
 if(num==0)return false;

 // Элемент перемещается вверх по списку
 ( parentItem->childItems ).swap(num,num-1);
 
 return true;
}


bool TreeItem::moveDn(void)
{
 // Выясняется номер данного элемента в списке родителя
 int num=childNumber();

 // Если двигать вниз некуда, ничего делать ненужно
 if(num>=(parentItem->childCount()-1))return false;

 // Элемент перемещается вниз по списку
 ( parentItem->childItems ).swap(num,num+1);
 
 return true;
}


// Путь к элементу (список идентификаторов от корня до текущего элемента)
QStringList TreeItem::getPath(void)
{
 return getPathAsField("id");
}


// Путь к элементу (список названий веток)
QStringList TreeItem::getPathAsName(void)
{
 return getPathAsField("name");
}


QStringList TreeItem::getPathAsField(QString fieldName)
{
 QStringList path;
 TreeItem *currentItem=this;
 
 path << currentItem->getField(fieldName);
 
 while(currentItem->parent()!=NULL)
  {
   currentItem=currentItem->parent();
   path << currentItem->getField(fieldName);
  }
 
 // Поворот массива идентификаторов задом наперед
 int k=path.size()-1;
 int j=path.size()/2;
 for(int i=0;i<j;++i)path.swap(i,k-i);
 
 return path;
}


// Возвращает массив путей всех подветок, которые содержит ветка
QList<QStringList> TreeItem::getAllChildrenPath(void)
{
 // Очищение списка путей
 getAllChildrenPathAsFieldRecurse(this, "", 0);
 
 // Получение списка путей
 QList<QStringList> pathList=getAllChildrenPathAsFieldRecurse(this, "id", 1);
 
 return pathList;
}


QList<QStringList> TreeItem::getAllChildrenPathAsField(QString fieldName)
{
 // Очищение списка путей
 getAllChildrenPathAsFieldRecurse(this, "", 0);

 // Получение списка путей
 QList<QStringList> pathList=getAllChildrenPathAsFieldRecurse(this, fieldName, 1);

 return pathList;
}


/*
// Возвращает массив путей всех подветок, которые содержит ветка
// Внутренняя рекурсивная функция
QList<QStringList> TreeItem::get_all_children_path_recurse(TreeItem *item,int mode)
{
 static QList<QStringList> pathList;
 
 // Если дана команда очистить список путей
 if(mode==0)
  {
   pathList.clear();
   return QList<QStringList>();
  }
 
 for(int i=0;i<(item->childCount());i++)
 {
  QStringList path=(item->child(i))->get_path();
  pathList << path;
  get_all_children_path_recurse(item->child(i),2);
 }
 
 if(mode==1)return pathList;
 else return QList<QStringList>();
}
*/


// Возвращает массив указанных полей всех подветок, которые содержит ветка
// Внутренняя рекурсивная функция
QList<QStringList> TreeItem::getAllChildrenPathAsFieldRecurse(TreeItem *item, QString fieldName, int mode)
{
 static QList<QStringList> pathList;

 // Если дана команда очистить список путей
 if(mode==0)
  {
   pathList.clear();
   return QList<QStringList>();
  }

 for(int i=0;i<(item->childCount());i++)
 {
  QStringList path=(item->child(i))->getPathAsField(fieldName);
  pathList << path;
  getAllChildrenPathAsFieldRecurse(item->child(i), fieldName, 2);
 }

 if(mode==1)return pathList;
 else return QList<QStringList>();
}


// Переключение ветки и всех подветок в зашифрованное состояние
void TreeItem::switchToEncrypt(void)
{
 qDebug() << "TreeItem::switchToEncrypt() : Crypt branch" << fieldsTable["name"] << "id" << fieldsTable["id"];

 // Если ветка оказалось заашифрованной ее нельзя зашифровывать второй раз
 if(fieldsTable["crypt"]=="1")
  return;

 // Устанавливается поле, что ветка зашифрована
 fieldsTable["crypt"]="1";

 // Шифруется имя ветки
 fieldsTable["name"]=encryptString(globalParameters.getCryptKey(), fieldsTable["name"]);


 // Шифрация конечных записей для этой ветки
 recordsTable.switchToEncrypt();


 // Шифрация подветок
 for(int i=0; i<childCount(); i++)
  child(i)->switchToEncrypt();
}


// Переключение ветки и всех подветок в расшифрованное состояние
void TreeItem::switchToDecrypt(void)
{
 qDebug() << "TreeItem::switchToDecrypt() : Decrypt branch" << fieldsTable["name"] << "id" << fieldsTable["id"];

 // Если ветка оказалось незашифрованной, нечего расшифровывать
 if(fieldsTable["crypt"].length()==0 ||
    fieldsTable["crypt"]=="0")
  return;

 // Устанавливается поле, что ветка не зашифрована
 fieldsTable["crypt"]="0";

 // Расшифровка имени ветки
 fieldsTable["name"]=decryptString(globalParameters.getCryptKey(), fieldsTable["name"]);


 // Дешифрация конечных записей для этой ветки
 recordsTable.switchToDecrypt();


 // Дешифрация подветок
 for(int i=0; i<childCount(); i++)
  child(i)->switchToDecrypt();
}


void TreeItem::recordtableInit(QDomElement domModel)
{
 recordsTable.init(this, domModel);
}


int TreeItem::recordtableGetRowCount(void)
{
 return recordsTable.size();
}


QDomDocument TreeItem::recordtableExportDataToDom(void)
{
 return recordsTable.exportDataToDom();
}


void TreeItem::recordtableDeleteAllRecords(void)
{
 recordsTable.deleteAllRecords();
}


RecordTableData *TreeItem::recordtableGetTableData(void)
{
 return &recordsTable;
}


// Получение Source номера строки, в которой находится запись с указанным идентификатором
// Если записи с таким идентификатором нет, возвращается -1
int TreeItem::getRecordPos(QString recordId)
{
 for(int i=0; i<recordsTable.size(); i++)
  if(recordsTable.getField("id", i)==recordId)
   return i;

 return -1;
}


