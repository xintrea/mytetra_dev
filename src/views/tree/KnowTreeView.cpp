#include <QDebug>
#include <QMimeData>

#include "main.h"
#include "KnowTreeView.h"
#include "libraries/ClipboardRecords.h"
#include "TreeScreen.h"
#include "models/tree/KnowTreeModel.h"
#include "models/recordTable/RecordTableData.h"
#include "models/tree/TreeItem.h"

KnowTreeView::KnowTreeView(QWidget *parent) : QTreeView(parent)
{
 // Разрешение принимать Drop-события
 setAcceptDrops(true);
 setDropIndicatorShown(true);
}


KnowTreeView::~KnowTreeView()
{

}


void KnowTreeView::dragEnterEvent(QDragEnterEvent *event)
{
 // Проверяется, содержит ли объект переноса данные нужного формата
 const QMimeData *mimeData=event->mimeData();
 if(mimeData==NULL)
  return;
 if( ! (mimeData->hasFormat("mytetra/records")) )
  return;

 QObject *sourceObject=qobject_cast<QObject *>( event->source() );

 if( sourceObject->objectName()=="RecordListScreen" )
  {
   event->setDropAction(Qt::MoveAction);
   event->accept();
   qDebug() << "Accept in dragEnterEvent()";
  }
}


void KnowTreeView::dragMoveEvent(QDragMoveEvent *event)
{
 // Проверяется, содержит ли объект переноса данные нужного формата
 const QMimeData *mimeData=event->mimeData();
 if(mimeData==NULL)
  return;
 if( ! (mimeData->hasFormat("mytetra/records")) )
  return;

 QObject *sourceObject=qobject_cast<QObject *>( event->source() );

 if( sourceObject->objectName()=="RecordListScreen" )
  {
   event->acceptProposedAction();
   qDebug() << "Accept in dragMoveEvent()";
  }
 else
  event->ignore();
}


void KnowTreeView::dropEvent(QDropEvent *event)
{
 qDebug() << "dropEvent() - Start";

 // Проверяется, содержит ли объект переноса данные нужного формата
 const QMimeData *mimeData=event->mimeData();
 if(mimeData==NULL)
  return;
 if( ! (mimeData->hasFormat("mytetra/records")) )
  return;

 QObject *sourceObject=qobject_cast<QObject *>( event->source() );

 qDebug() << "dropEvent() - source object name is " << sourceObject->objectName();

 if( sourceObject->objectName()=="RecordListScreen" )
  {
   qDebug() << "Try move record by drag and drop";

   // Извлечение объекта
   const ClipboardRecords *clipboardRecords;
   clipboardRecords=qobject_cast<const ClipboardRecords *>(event->mimeData());

   // Печать содержимого перетаскиваемого объекта
   clipboardRecords->print();

   // Выясняется элемент дерева, над которым был сделан Drop
   QModelIndex index=indexAt(event->pos());

   // Указатель на родительский элемент
   TreeScreen *parentPointer=qobject_cast<TreeScreen *>( parent() );

   // Выясняется ссылка на элемент дерева (на ветку), над которым был совершен Drop
   TreeItem *treeItem=parentPointer->knowTreeModel->getItem(index);

   // Выясняется ссылка на таблицу данных которая содержится в данной ветке
   RecordTableData *recordTableData=treeItem->recordtableGetTableData();

   // Текстовые данные записи
   QMap<QString, QString> exemplar=clipboardRecords->getRecordFields(0);
   QString text=exemplar["text"]; // Текст записи
   exemplar.remove("text"); // Текст удаляется из данных записи, он передается отдельно

   recordTableData->insertNewRecord(ADD_NEW_RECORD_TO_END,
                                    0,
                                    exemplar,
                                    text,
                                    clipboardRecords->getRecordFiles(0) );

   // todo: Сделать удаление записи из исходной ветки

   // todo: Сделать обновлении исходной ветки чтобы было видно что записей убавилось

   // todo: Сделать обновлении конечной ветки чтобы было видно что записей прибавилось
  }
}

