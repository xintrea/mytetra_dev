#include <QHeaderView>
#include <QMenu>
#include <QDebug>

#include "main.h"
#include "ActionLogView.h"
#include "ActionLogScreen.h"
#include "controllers/actionLog/ActionLogController.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


ActionLogView::ActionLogView(QWidget *parent) : QTableView(parent)
{
  this->horizontalHeader()->setStretchLastSection( true );
  this->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделяется вся строка

  // Настройка области виджета для кинетической прокрутки
  setKineticScrollArea( qobject_cast<QAbstractItemView*>(this) );

  // Разрешение принимать жест QTapAndHoldGesture
  grabGesture(Qt::TapAndHoldGesture);
}


ActionLogView::~ActionLogView()
{

}


void ActionLogView::init(void)
{
  setupSignals();

  assemblyContextMenu();
  setContextMenuPolicy(Qt::CustomContextMenu);
}


void ActionLogView::setupSignals(void)
{
  // Сигнал чтобы показать контекстное меню по правому клику на списке записей
  connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),
          this,SLOT(onCustomContextMenuRequested(const QPoint &)));

  // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
  connect(this, SIGNAL(tapAndHoldGestureFinished(const QPoint &)),
          this, SLOT(onCustomContextMenuRequested(const QPoint &)));

  // Сигнал чтобы открыть на просмотр запись по двойному клику
  connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
          controller, SLOT(onOpenAttach(void)));
}


void ActionLogView::setController(ActionLogController *pController)
{
  controller=pController;
}


void ActionLogView::assemblyContextMenu(void)
{
  // Конструирование меню
  contextMenu=new QMenu(this);

  // find_object() не работает, потому что при инициализации еще не вызван метод assembly() у ActionLogScreen, и этому объекту не задан родитель layout
  // ActionLogScreen *screenPointer=find_object<ActionLogScreen>("ActionLogScreen");
  ActionLogScreen *screenPointer=qobject_cast<ActionLogScreen *>(controller->parent());

  contextMenu->addAction(screenPointer->actionAddAttach);
  contextMenu->addAction(screenPointer->actionEditFileName);
  contextMenu->addAction(screenPointer->actionDeleteAttach);
  contextMenu->addSeparator();
  contextMenu->addAction(screenPointer->actionOpenAttach);
  contextMenu->addAction(screenPointer->actionShowAttachInfo);
}


// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool ActionLogView::event(QEvent *event)
{
  if (event->type() == QEvent::Gesture)
  {
    qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
    return gestureEvent(static_cast<QGestureEvent*>(event));
  }

  return QTableView::event(event);
}


void ActionLogView::resizeEvent(QResizeEvent *event)
{
  // Первый столбец должен занимать 80% ширины при любом размере таблицы
  QRect viewRect = this->rect();
  float columnWidth = (float) viewRect.width() * 0.8;
  this->setColumnWidth( 0, columnWidth );

  // Отрисовка родительского класса
  QTableView::resizeEvent(event);
}


// Обработчик жестов
// Вызывается из обработчика событий event()
bool ActionLogView::gestureEvent(QGestureEvent *event)
{
  qDebug() << "In gestureEvent()" << event;

  if (QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
    tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

  return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов gestureEvent()
void ActionLogView::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
  qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

  if(gesture->state()==Qt::GestureFinished)
    if(globalParameters.getTargetOs()=="android")
      emit tapAndHoldGestureFinished( mapFromGlobal(gesture->position().toPoint()) );
}


// Открытие контекстного меню 
void ActionLogView::onCustomContextMenuRequested(const QPoint &pos)
{
  // Включение отображения меню на экране
  contextMenu->exec( viewport()->mapToGlobal(pos) );
}
