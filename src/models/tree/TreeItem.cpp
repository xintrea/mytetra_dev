#include <QStringList>
#include <QString>
#include <QMap>

#include "TreeItem.h"
#include "main.h"
#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "libraries/crypt/CryptService.h"
#include "models/appConfig/AppConfig.h"

extern GlobalParameters globalParameters;
extern FixedParameters fixedParameters;
extern AppConfig mytetraConfig;


TreeItem::TreeItem(const QMap<QString, QString> &data, TreeItem *parent)
{
  detachedState=false; // По-умолчанию элемент полноценный, не оторванный

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
unsigned int TreeItem::childCount() const
{
 return childItems.count();
}


// Возвращает номер, под которым данный объект хранится
// в массиве childItems своего родителя
unsigned int TreeItem::childNumber() const
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

 // Если элемент не является "оторванным"
 if(detachedState==false)
   qDeleteAll(childItems); // Удаляются все подветки
 
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
       itemName=CryptService::decryptString(globalParameters.getCryptKey(), itemName);
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
 if(fixedParameters.itemFieldAvailableList.contains(name))
  {
   // Если поле с таким именем существует
   if(fieldsTable.contains(name))
    {
     QString value=fieldsTable[name];

     // Если есть шифрование
     // и поле является зашифрованным
     if(fieldsTable.contains("crypt"))
      if(fieldsTable["crypt"]=="1")
       if(fixedParameters.itemFieldCryptedList.contains(name))
        {
         if(globalParameters.getCryptKey().length()>0 &&
            value!="")
          value=CryptService::decryptString(globalParameters.getCryptKey(), value);
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
   criticalError("In TreeItem::get_field() unavailable name '"+name+"'");
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
    if(fixedParameters.itemFieldAvailableList.contains(name))
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
  // Если имя поля допустимо
  if(fixedParameters.itemFieldAvailableList.contains(name))
  {
    // Если это иконка
    if(name=="icon")
    {
      if(value.length()>0)
        icon=QIcon(mytetraConfig.get_tetradir()+"/"+fixedParameters.iconsRelatedDirectory+"/"+value); // Изображение иконки кешируется
      else
        icon=QIcon(); // Изображение иконки обнуляется
    }

    // Если поле нужно шифровать
    if(fieldsTable["crypt"]=="1")
      if(fixedParameters.itemFieldCryptedList.contains(name))
      {
        // Если установлен пароль
        if(globalParameters.getCryptKey().length()>0)
        {
          // Если поле непустое, поле зашифровывается
          if(value!="")
            value=CryptService::encryptString(globalParameters.getCryptKey(), value);
        }
        else // Иначе пароль не установлен
          criticalError("TreeItem::setField() : Can not encrypt field \""+name+"\". Password not setted.");
      }

    // qDebug() << "Set to item data " << name << value;
    fieldsTable[name]=value;
  }
  else
    criticalError("TreeItem::setField() : Set unavailable field \""+ name +"\" to tree item");
}


// Установка данных напрямую - какие данные переданы, те и запомнятся
// без всяких преобразований, без шифрации
// Метод используется в одном месте - при инициализации дерева из XML файла
void TreeItem::setAllFieldDirect(const QMap<QString, QString> nameAndValue)
{
  foreach(QString name, nameAndValue.keys())
    if( !fixedParameters.itemFieldAvailableList.contains(name) )
      criticalError("TreeItem::setFieldDirect() : Set unavailable field \""+ name +"\" to tree item");

  // Устанавливаются значения полей
  fieldsTable=nameAndValue; // Qt сам должен правильно сделать привязку к переданным данным и оставить их в памяти

  // Если есть иконка и нет шифрования, изображение иконки кешируется
  if(nameAndValue.value("icon").length()>0 &&
     nameAndValue.value("crypt")!="1")
    icon=QIcon(mytetraConfig.get_tetradir()+"/"+fixedParameters.iconsRelatedDirectory+"/"+nameAndValue.value("icon"));

  if(nameAndValue.value("icon").length()==0)
    icon=QIcon();
}


QIcon TreeItem::getIcon()
{
  // Если ветка не зашифрована
  if( fieldsTable["crypt"]!="1" )
    return icon; // Просто возвращается иконка, не важно, пустая или с рисунком
  else
  {
    // Иначе ветка зашифрована

    // Если иконка уже была закеширована
    if(!icon.isNull())
      return icon;
    else
    {
      // Иначе иконка не закеширована

      // Если есть возможность закешировать (введен пароль и он правильный)
      if(globalParameters.getCryptKey().length()>0)
      {
        QString iconFileName=getField("icon");

        // Если иконка была задана
        if(iconFileName.length()>0)
          icon=QIcon(mytetraConfig.get_tetradir()+"/"+fixedParameters.iconsRelatedDirectory+"/"+iconFileName);
      }

      return icon;
    }
  }
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
    criticalError("In TreeItem data getting field with unavailable name 'id'");
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


// Добавление нового пустого подчиненного элемента
// в конец списка подчиненных элементов
bool TreeItem::addChildrenEmpty(void)
{
  QMap<QString, QString> data;

  TreeItem *item = new TreeItem(data, this); // Создается объект item

  childItems << item; // Добавление item в конец массива childItems

  return true;
}


// Добавление уже существующего Item-элемента
bool TreeItem::addChildrenItem(TreeItem *item)
{
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
    delete childItems.takeAt(0);
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
  unsigned int num=childNumber();

  // Если двигать вниз некуда, ничего делать ненужно
  if(num>=(parentItem->childCount()-1))
    return false;

  // Элемент перемещается вниз по списку
  ( parentItem->childItems ).swap(num,num+1);

  return true;
}


// Путь к элементу (список идентификаторов от корня до текущего элемента)
QStringList TreeItem::getPath(void)
{
  return getPathAsField("id");
}


// Путь к элементу (в виде списка названий веток)
QStringList TreeItem::getPathAsName(void)
{
  return getPathAsField("name");
}


// Путь к элементу в виде строки, разделенной указанным разделителем
QString TreeItem::getPathAsNameWithDelimeter(QString delimeter)
{
  QStringList path=getPathAsName();

  // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
  int emptyStringIndex=path.indexOf("");
  path.removeAt(emptyStringIndex);

  return path.join(delimeter);
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

  for(unsigned int i=0;i<(item->childCount());i++)
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
  qDebug() << "TreeItem::switchToEncrypt() : Crypt tree item " << fieldsTable["name"] << "id" << fieldsTable["id"];

  // Если ветка оказалось заашифрованной ее нельзя зашифровывать второй раз
  if(fieldsTable["crypt"]=="1")
    return;

  // Устанавливается поле, что ветка зашифрована
  fieldsTable["crypt"]="1";

  // Шифруются поля ветки, подлежащие шифрованию
  foreach(QString fieldName, fixedParameters.itemFieldCryptedList)
    fieldsTable[fieldName]=CryptService::encryptString(globalParameters.getCryptKey(), fieldsTable[fieldName]);

  // Шифрация конечных записей для этой ветки
  recordsTable.switchToEncrypt();


  // Шифрация подветок
  for(unsigned int i=0; i<childCount(); i++)
    child(i)->switchToEncrypt();
}


// Переключение ветки и всех подветок в расшифрованное состояние
void TreeItem::switchToDecrypt(void)
{
  qDebug() << "TreeItem::switchToDecrypt() : Decrypt tree item" << fieldsTable["name"] << "id" << fieldsTable["id"];

  // Если ветка оказалось незашифрованной, нечего расшифровывать
  if(fieldsTable["crypt"].length()==0 ||
     fieldsTable["crypt"]=="0")
    return;

  // Устанавливается поле, что ветка не зашифрована
  fieldsTable["crypt"]="0";

  // Расшифровка полей ветки, подлежащих шифрованию
  foreach(QString fieldName, fixedParameters.itemFieldCryptedList)
    fieldsTable[fieldName]=CryptService::decryptString(globalParameters.getCryptKey(), fieldsTable[fieldName]);

  // Дешифрация конечных записей для этой ветки
  recordsTable.switchToDecrypt();


  // Дешифрация подветок
  for(unsigned int i=0; i<childCount(); i++)
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


QDomElement TreeItem::recordtableExportDataToDom(QDomDocument *doc)
{
  return recordsTable.exportDataToDom( doc );
}


void TreeItem::recordtableExportDataToStreamWriter(QXmlStreamWriter *xmlWriter)
{
  recordsTable.exportDataToStreamWriter( xmlWriter );
}


void TreeItem::recordtableDeleteAllRecords(void)
{
  recordsTable.deleteAllRecords();
}


RecordTableData *TreeItem::recordtableGetTableData(void)
{
  return &recordsTable;
}


void TreeItem::setDetached(bool state)
{
  detachedState=state;
}


bool TreeItem::isDetached()
{
  return detachedState;
}
