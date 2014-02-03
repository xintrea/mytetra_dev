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
#include "views/recordTable/RecordListScreen.h"

KnowTreeView::KnowTreeView(QWidget *parent) : QTreeView(parent)
{
 // Разрешение принимать Drop-события
 setAcceptDrops(true);
 setDropIndicatorShown(true);

 // this->setFlag(Qt::WA_Hover);
 // QString style="QTreeView::item:hover { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1); border: 1px solid #bfcde4; }";
 // this->setStyleSheet(style);
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
   qDebug() << "Accept in dragEnterEvent()";
  }
}


void KnowTreeView::dragMoveEvent(QDragMoveEvent *event)
{
 if( isDragableData(event) )
  {
   event->acceptProposedAction();
   qDebug() << "Accept in dragMoveEvent()";

   // Выясняется элемент дерева, над которым находится курсор
   QModelIndex index=indexAt(event->pos());

   // Указатель на родительский элемент
   TreeScreen *parentPointer=qobject_cast<TreeScreen *>( parent() );

   // Изменяется модель данных, в ней выставляется флаг что курсор находится над элементом
   parentPointer->knowTreeModel->setData(index, QVariant(true), Qt::UserRole);

   parentPointer->knowTreeModel->emitSignalDataChanged(index);

   // Выясняется элемент дерева, над которым находится курсор
   // QModelIndex index=indexAt(event->pos());
   // QAbstractItemDelegate *item=itemDelegate(index);

   // Стилизация по item:hover не работает при перетаскивании (работает только при обычном перемещении мышки над элементом)
   // QString style="QTreeView::item:hover { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1); border: 1px solid #bfcde4; }";
   // this->setStyleSheet(style);

   // todo: ... доделать подсветку элемента над которым находится мышка ...


   // item->setEditorData();
   // setBackground(0, Qt::red);
   // this->setMouseTracking(true);

   // Подсвечивается данный элемент
   // index.model()->setData(&index, QColor(Qt::darkGray), Qt::BackgroundRole);
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

 if( sourceObject->objectName()=="RecordListScreen" )
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

   // Удаление записи из исходной ветки, удаление должно быть вначале, чтобы сохранился ID записи
   // Исходная ветка в момент Drop - это выделенная курсором ветка
   QModelIndex indexFrom = find_object<TreeScreen>("treeview")->getCurrentItemIndex();
   TreeItem *treeItemFrom=parentPointer->knowTreeModel->getItem(indexFrom);
   unsigned int recordPos=treeItemFrom->recordtableGetTableData()->getWorkPos(); // Или можно сделать через getRecordPos(QString recordId)
   RecordListScreen *recordListScreen=find_object<RecordListScreen>("RecordListScreen");
   recordListScreen->deleteRecordByPos(recordPos);

   // Добавление записи в базу
   recordTableData->insertNewRecord(ADD_NEW_RECORD_TO_END,
                                    0,
                                    exemplar,
                                    text,
                                    clipboardRecords->getRecordFiles(0) );

   // Сохранение дерева веток
   find_object<TreeScreen>("treeview")->saveKnowTree();

   // Обновление исходной ветки чтобы было видно что записей убавилось
   parentPointer->updateBranchOnScreen(indexFrom);

   // Обновлении конечной ветки чтобы было видно что записей прибавилось
   parentPointer->updateBranchOnScreen(index);

  }
}

