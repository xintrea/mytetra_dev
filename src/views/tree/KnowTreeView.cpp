#include <QDebug>
#include <QMimeData>

#include "KnowTreeView.h"
#include "libraries/ClipboardRecords.h"

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

 if( sourceObject->objectName()=="recordview" )
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

 if( sourceObject->objectName()=="recordview" )
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

 if( sourceObject->objectName()=="recordview" )
  {
   qDebug() << "Try move record by drag and drop";

   // Извлечение объекта
   const ClipboardRecords *clipboardRecords;
   clipboardRecords=qobject_cast<const ClipboardRecords *>(event->mimeData());
   clipboardRecords->print();
  }
}

