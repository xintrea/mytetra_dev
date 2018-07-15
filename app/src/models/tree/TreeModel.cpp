#include "TreeItem.h"
#include "TreeModel.h"
#include "main.h"
#include "models/recordTable/RecordTableData.h"
#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "models/appConfig/AppConfig.h"

extern GlobalParameters globalParameters;
extern FixedParameters fixedParameters;
extern AppConfig mytetraConfig;


// TreeModel - Это модель данных, которая работает с видом TreeScreen
// От него наследуется класс KnowTreeModel

TreeModel::TreeModel(QObject *parent) : QAbstractItemModel(parent)
{
 return;
}

TreeModel::~TreeModel(void)
{
 return;
}

int TreeModel::columnCount(const QModelIndex &itemIndex) const
{
 Q_UNUSED(itemIndex);
 
 // Ранее число столбцов вычислялось исходя из 
 // количества полей в корневом элементе
 // return rootItem->fieldCount();
 
 return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    // Если индекс невалиден, возвращается несуществующий элемент
    if(!index.isValid())return QVariant();

    // Если запрашивается окраска текста элемента
    if(role==Qt::ForegroundRole)
    {
     TreeItem *item = getItem(index);

     if(item->recordtableGetRowCount()>0)
      return QColor(Qt::black);// Если узел содержит таблицу конечных записей
     else
      return QColor(Qt::darkGray); // Ветка без таблицы конечных записей
    }


    if(role==Qt::BackgroundRole)
    {
     if(index==cursorOverIndex)
      return QColor(Qt::gray);
    }


    // Если запрашивается содержимое текста элемента
    if(role==Qt::DisplayRole || role== Qt::EditRole)
    {
     TreeItem *item = getItem(index);

     return QVariant( item->getField("dynamicname") ); // Запрашивается строка имени с количеством элементов
    }

    // Если запрашиваются элементы оформления
    if(role == Qt::DecorationRole)
    {
      TreeItem *item = getItem(index);

      QIcon resultIcon=item->getIcon();

      // Если иконка существует, она выводится
      if( !resultIcon.isNull() )
        return resultIcon;

      // Если ветка зашифрована
      if(item->getField("crypt")=="1")
      {
        // Если пароль не введен, доступа к ветке нет
        if(globalParameters.getCryptKey().length()==0)
         return QIcon(":/resource/pic/branch_closed.svg");
        else
         return QIcon(":/resource/pic/branch_opened.svg");
      }
    }

    return QVariant();
}


Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


// Получение указателя на Item-злемент связанный с заданным QModelIndex
TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
  if (index.isValid())
  {
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if (item)
    {
      // qDebug() << "Get tree item " << item->data("name").toString();
      return item;
    }
    else
    {
      qDebug() << "Detect bad castind to TreeItem in getItem() method ";
      return NULL;
    }

  }

  // qDebug() << "Detect bad QModelIndex in getItem() method ";
  return rootItem;
}


QModelIndex TreeModel::getIndexByItem(TreeItem *item)
{
 // Инициализация рекурсивной функции
 getIndexRecurse(QModelIndex(), item, 0);

 return getIndexRecurse(QModelIndex(), item, 1);
}


QModelIndex TreeModel::getIndexRecurse(QModelIndex index, TreeItem *item, int mode)
{
 static QModelIndex find_index;
 static bool is_find=false;

 if(is_find) return find_index;

 if(mode==0)
  {
   is_find=false;
   return QModelIndex();
  }

 if(mode==1)
  {
   // Если указатель узла совпадает с заданным item
   if(item==static_cast<TreeItem*>(index.internalPointer()))
    {
     is_find=true;
     find_index=index;
     return find_index;
    }
   else
    {
     // Иначе указатель узла не совпадает с заданным
     // и нужно рекурсивно искать далее
     for(int i=0; i<index.row(); i++)
      getIndexRecurse(index.child(i, 0), item, 1);
    }
  }

 return QModelIndex();
}


// Получение указателя на Item-злемент с указанным путем
TreeItem *TreeModel::getItem(QStringList path) const
{
 TreeItem *curritem=rootItem;

 // Перебор идентификаторов пути
 for(int i=1;i<path.size();i++)
 {
  int found=0;

  // Поиск нужного идентификатора в подчиненных узлах текущего узла
  for(unsigned int j=0; j < curritem->childCount(); j++)
   if( (curritem->child(j))->getField("id") == path.at(i) )
    {
     // Узел найден, он становится текущим
     curritem=curritem->child(j);
     found=1;
     break;
    }

  // Если очередной идентификатор пути не был найден
  if(found==0)
   criticalError("Detect bad path in getItem() method "+path.join(","));
 }

 return curritem;
}


bool TreeModel::isItemValid(QStringList path) const
{
  if(path.count()==0)
    return false;

  if(path.count()==1 && path[0]=="0")
    return false;

  TreeItem *curritem=rootItem;

  // Перебор идентификаторов пути
  for(int i=1;i<path.size();i++)
  {
    int found=0;

    // Поиск нужного идентификатора в подчиненных узлах текущего узла
    for(unsigned int j=0;j<curritem->childCount();j++)
      if( (curritem->child(j))->getField("id") == path.at(i) )
      {
        // Узел найден, он становится текущим
        curritem=curritem->child(j);
        found=1;
        break;
      }

    // Если очередной идентификатор пути не был найден
    if(found==0)
      return false;
  }

  return true;
}


void TreeModel::emitSignalDataChanged(const QModelIndex &index)
{
  emit dataChanged(index, index);
}


// Получение заголовка столбца
// Заголовки хранятся в корневом Item элементе просто в виде значений 
// для каждого столбца
// section - для какого столбца возвращается заголовок
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
  Q_UNUSED(section);
  Q_UNUSED(orientation);
  Q_UNUSED(role);
  
  // Для всех столбцов возвращается одинаковое значение
  // фактически используется только один столбец
  return "";

  /*
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
 */
}


// Получение индекса элемента, который задан через parent и 
// номер строки и столбца относительно parent (нумерация с нуля)
// Загадочный метод, надо еще подумать что он делает на самом деле
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  TreeItem *parentItem = getItem(parent);

  TreeItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}


// Вставка пустых строк с позиции position в количестве rows
bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
  TreeItem *parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);

  // Добавляются строки начиная с указанной позиции, в количестве rows
  // с числом столбцов равным единице
  success = parentItem->insertChildren(position, rows, 1);

  endInsertRows();

  return success;
}


QModelIndex TreeModel::parent(const QModelIndex &index) const
{
  if(!index.isValid())
    return QModelIndex();

  TreeItem *childItem = getItem(index);
  TreeItem *parentItem = childItem->parent();

  if(parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->childNumber(), 0, parentItem);
}


bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
  TreeItem *parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}


int TreeModel::rowCount(const QModelIndex &itemIndex) const
{
 TreeItem *item = getItem(itemIndex);
  return item->childCount();
}


// Установка значений в Item элементе, связанного с указанным QModelIndex 
bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Роль UserRole в настоящий момент используется для задания флага, сообщающего
  // что курсор неаходится над элементом при Drag and Drop
  if(role==Qt::UserRole)
  {
    QModelIndex previousIndex=cursorOverIndex;

    if(value.toBool())
      cursorOverIndex=index;
    else
      cursorOverIndex=QModelIndex();

    if(previousIndex.isValid())
      emitSignalDataChanged(previousIndex);

    if(cursorOverIndex.isValid())
      emitSignalDataChanged(cursorOverIndex);

    return true;
  }

  if(role==Qt::EditRole)
  {

    // Вычисляется указатель на Item элемент по QModelIndex
    // в визуальной модели дерева
    TreeItem *item = getItem(index);

    // Устанавливаются данные в Item элемент
    item->setField("name", value.toString());

    return true;
  }

  return false;
}


bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
  if (role != Qt::EditRole || orientation != Qt::Horizontal)
    return false;

  Q_UNUSED(section);
  rootItem->setField("name", value.toString());
  return true;
}
