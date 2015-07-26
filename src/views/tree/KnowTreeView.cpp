#include <QDebug>
#include <QMimeData>
#include <QAbstractItemModel>
#include <QMessageBox>

#include "main.h"
#include "KnowTreeView.h"
#include "TreeScreen.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/GlobalParameters.h"
#include "models/tree/KnowTreeModel.h"
#include "models/recordTable/RecordTableData.h"
#include "models/tree/TreeItem.h"
#include "models/recordTable/RecordTableModel.h"
#include "views/mainWindow/MainWindow.h"
#include "views/recordTable/RecordTableScreen.h"
#include "views/record/MetaEditor.h"
#include "controllers/recordTable/recordTableController.h"


extern GlobalParameters globalParameters;


KnowTreeView::KnowTreeView(QWidget *parent) : QTreeView(parent)
{
 // Разрешение принимать Drop-события
 setAcceptDrops(true);
 setDropIndicatorShown(true);

 // Разрешение принимать жест QTapAndHoldGesture
 grabGesture(Qt::TapAndHoldGesture);

 // Настройка области виджета для кинетической прокрутки
 setKineticScrollArea( qobject_cast<QAbstractItemView*>(this) );
}


KnowTreeView::~KnowTreeView()
{

}


// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool KnowTreeView::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
     {
        qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
        return gestureEvent(static_cast<QGestureEvent*>(event));
     }

    return QTreeView::event(event);
}


// Обработчик жестов
// Вызывается из обработчика событий
bool KnowTreeView::gestureEvent(QGestureEvent *event)
{
  qDebug() << "In gestureEvent()" << event;

  if (QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
    tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

  return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void KnowTreeView::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
  qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

  if(gesture->state()==Qt::GestureFinished)
    if(globalParameters.getTargetOs()=="android")
      emit tapAndHoldGestureFinished( mapFromGlobal(gesture->position().toPoint()) );
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

 if( sourceObject->objectName()=="recordTableView" )
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

   // Печать в консоль содержимого перетаскиваемого объекта (для отладки)
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
   QModelIndex indexFrom = find_object<TreeScreen>("treeScreen")->getCurrentItemIndex();

   // Если перенос происходит в ту же самую ветку
   if(indexFrom==index)
    return;


   // Перенос записей, хранящихся в MimeData
   // В настоящий момент в MimeData попадает только одна запись,
   // но в дальнейшем планируется переносить несколько записей
   // и здесь код подготовлен для переноса нескольких записей
   RecordTableController *recordTableController=find_object<RecordTableController>("recordTableController");  // Указатель на контроллер таблицы конечных записей
   for(int i=0; i<clipboardRecords->getCount(); i++)
    {
     // Полные данные записи
     Record record=clipboardRecords->getRecord(i);

     // Удаление записи из исходной ветки, удаление должно быть вначале, чтобы сохранился ID записи
     // В этот момент вид таблицы конечных записей показывает таблицу, из которой совершается Drag
     // TreeItem *treeItemFrom=parentPointer->knowTreeModel->getItem(indexFrom);
     recordTableController->removeRowById( record.fieldList["id"] );

     // Если таблица конечных записей после удаления перемещенной записи стала пустой
     if(recordTableController->getRowCount()==0)
       find_object<MetaEditor>("editorScreen")->clearAll(); // Нужно очистить поле редактирования чтобы не видно было текста последней удаленной записи
     find_object<RecordTableScreen>("recordTableScreen")->toolsUpdate();

     // Добавление записи в базу
     recordTableData->insertNewRecord(ADD_NEW_RECORD_TO_END,
                                      0,
                                      record);

     // Сохранение дерева веток
     find_object<TreeScreen>("treeScreen")->saveKnowTree();
    }

   // Обновление исходной ветки чтобы было видно что записей убавилось
   parentPointer->updateBranchOnScreen(indexFrom);

   // Обновлении конечной ветки чтобы было видно что записей прибавилось
   parentPointer->updateBranchOnScreen(index);

   // В модели данных обнуляется элемент, который подсвечивался при Drag And Drop
   parentPointer->knowTreeModel->setData(QModelIndex(), QVariant(false), Qt::UserRole);
  }
}

