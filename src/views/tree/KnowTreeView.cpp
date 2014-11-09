#include <QDebug>
#include <QMimeData>
#include <QAbstractItemModel>

#include "main.h"
#include "KnowTreeView.h"
#include "libraries/ClipboardRecords.h"
#include "TreeScreen.h"
#include "models/tree/KnowTreeModel.h"
#include "models/recordTable/RecordTableData.h"
#include "models/tree/TreeItem.h"
#include "models/recordTable/RecordTableModel.h"
#include "views/recordTable/RecordTableView.h"


KnowTreeView::KnowTreeView(QWidget *parent) : QTreeView(parent)
{
 // Разрешение принимать Drop-события
 setAcceptDrops(true);
 setDropIndicatorShown(true);

 // Настройка области виджета для кинетической прокрутки
 setKineticScrollArea( qobject_cast<QAbstractItemView*>(this) );
}


KnowTreeView::~KnowTreeView()
{

}


void KnowTreeView::dragEnterEvent(QDragEnterEvent *event)
{
 if( isDragableData(event) )
  {
   event->setDropAction(Qt::MoveAction);
   event->accept();
  }
}


void KnowTreeView::dragMoveEvent(QDragMoveEvent *event)
{
 if( isDragableData(event) )
  {
   event->acceptProposedAction();

   // Выясняется элемент дерева, над которым находится курсор
   QModelIndex index=indexAt(event->pos());

   // Указатель на родительский элемент, чтобы далее получить модель данных
   TreeScreen *parentPointer=qobject_cast<TreeScreen *>( parent() );

   // В модели данных отмечается элемент дерева, над которым находится курсор
   parentPointer->knowTreeModel->setData(index, QVariant(true), Qt::UserRole);
  }
 else
  event->ignore();
}


template <class X> bool KnowTreeView::isDragableData(X *event)
{
 // Проверяется, содержит ли объект переноса данные нужного формата
 const QMimeData *mimeData=event->mimeData();
 if(mimeData==NULL)
  return false;
 if( ! (mimeData->hasFormat("mytetra/records")) )
  return false;

 QObject *sourceObject=qobject_cast<QObject *>( event->source() );

 if( sourceObject->objectName()=="RecordTableView" )
  return true;
 else
  return false;
}


void KnowTreeView::dropEvent(QDropEvent *event)
{
 qDebug() << "dropEvent() - Start";

 if( isDragableData(event) )
  {
   qDebug() << "Try move record by drag and drop";

   // Извлечение объекта
   const ClipboardRecords *clipboardRecords;
   clipboardRecords=qobject_cast<const ClipboardRecords *>(event->mimeData());

   // Печать содержимого перетаскиваемого объекта
   clipboardRecords->print();

   // Выясняется элемент дерева, над которым был сделан Drop
   QModelIndex index=indexAt(event->pos());

   // Если отпускание мышки произошло не на ветке дерева, а на свободном пустом месте в области виджета дерева
   if(!index.isValid())
    return;

   // Указатель на родительский элемент
   TreeScreen *parentPointer=qobject_cast<TreeScreen *>( parent() );

   // Выясняется ссылка на элемент дерева (на ветку), над которым был совершен Drop
   TreeItem *treeItem=parentPointer->knowTreeModel->getItem(index);

   // Выясняется ссылка на таблицу данных ветки, над которой совершен Drop
   RecordTableData *recordTableData=treeItem->recordtableGetTableData();

   // Исходная ветка в момент Drop (откуда переностся запись) - это выделенная курсором ветка
   QModelIndex indexFrom = find_object<TreeScreen>("TreeScreen")->getCurrentItemIndex();

   // Если перенос происходит в ту же самую ветку
   if(indexFrom==index)
    return;


   // Перенос записей, хранящихся в MimeData
   // В настоящий момент в MimeData попадает только одна запись,
   // но в дальнейшем планируется переносить несколько записей
   // и здесь код подготовлен для переноса нескольких записей
   for(int i=0; i<clipboardRecords->getRecordsNum(); i++)
    {
     // Данные записи (текстовые)
     QMap<QString, QString> exemplar=clipboardRecords->getRecordFields(i);
     QString text=exemplar["text"]; // Текст записи
     exemplar.remove("text"); // Текст удаляется из данных записи, он передается отдельно

     // Удаление записи из исходной ветки, удаление должно быть вначале, чтобы сохранился ID записи
     TreeItem *treeItemFrom=parentPointer->knowTreeModel->getItem(indexFrom);
     unsigned int recordPos=treeItemFrom->getRecordPos( exemplar["id"] );
     RecordTableView *recordTableView=find_object<RecordTableView>("RecordTableView");
     recordTableView->deleteRecordByPos(recordPos);

     // Добавление записи в базу
     recordTableData->insertNewRecord(ADD_NEW_RECORD_TO_END,
                                      0,
                                      exemplar,
                                      text,
                                      clipboardRecords->getRecordFiles(i) );

     // Сохранение дерева веток
     find_object<TreeScreen>("TreeScreen")->saveKnowTree();
    }

   // Обновление исходной ветки чтобы было видно что записей убавилось
   parentPointer->updateBranchOnScreen(indexFrom);

   // Обновлении конечной ветки чтобы было видно что записей прибавилось
   parentPointer->updateBranchOnScreen(index);

   // В модели данных обнуляется элемент, который подсвечивался при Drag And Drop
   parentPointer->knowTreeModel->setData(QModelIndex(), QVariant(false), Qt::UserRole);
  }
}

