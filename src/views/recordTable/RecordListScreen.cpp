#include <QObject>
#include <QMimeData>

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/ClipboardRecords.h"
#include "RecordListScreen.h"
#include "views/recordTable/RecordTableScreen.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableData.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraconfig;


// Виджет, отображащий список записей в ветке

// Так как в Qt нет такого режима выделения, чтобы при движении
// мышкой записи в QListView не помечались, то удобный Drag&Drop
// сделать нельзя. Поэтому в данный момент виджет ничего не делает
// сверх того что есть в QListView

RecordListScreen::RecordListScreen(QWidget *parent) : QListView(parent)
{
 this->setSelectionMode(QAbstractItemView::SingleSelection); // ExtendedSelection

 isStartDrag=false;
}


RecordListScreen::~RecordListScreen()
{

}


// Реакция на нажатие кнопок мышки
void RecordListScreen::mousePressEvent(QMouseEvent *event)
{
 // Если нажата левая кнопка мыши
 if(event->buttons() == Qt::LeftButton)
  {
   mouseStartPos=event->pos();
  }

 QListView::mousePressEvent(event);
}


// Реакция на движение мышкой
void RecordListScreen::mouseMoveEvent(QMouseEvent *event)
{
 // Если при движении нажата левая кнопка мышки
 if(event->buttons() & Qt::LeftButton)
  {
   // Выясняется расстояние от места начала нажатия
   int distance=(event->pos() - mouseStartPos).manhattanLength();

   if(distance >= QApplication::startDragDistance()*2)
    {
     if(isStartDrag==false)
      {
       isStartDrag=true;
       startDrag();
      }
    }
  }

 QListView::mouseMoveEvent(event);
}


// Реакция на отпускание клавиши мышки
void RecordListScreen::mouseReleaseEvent(QMouseEvent *event)
{
 if(isStartDrag==true) isStartDrag=false;

 QListView::mouseReleaseEvent(event);
}


void RecordListScreen::startDrag()
{
 // return; // Drag&Drop не работает как хотелось бы, отключен

 qDebug() << "Start record drag\n";

 // Копирование выделенных строк в буфер обмена
 qobject_cast<RecordTableScreen *>(parent())->copy();

 // Запуск операции перетаскивания объекта, доделать
 // if(drag->start(Qt::MoveAction)==Qt:MoveAtion)

}


// Переопределенный слот (virtual protected slot)
void RecordListScreen::selectionChanged(const QItemSelection &selected,
                                        const QItemSelection &deselected )
{
 // qDebug() << "RecordListScreen::selectionChanged()";

 emit listSelectionChanged(selected, deselected);
}
