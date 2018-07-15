#include <QHeaderView>
#include <QMenu>
#include <QDebug>
#include <QPainter>

#include "main.h"
#include "AttachTableView.h"
#include "AttachTableScreen.h"
#include "controllers/attachTable/AttachTableController.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


AttachTableView::AttachTableView(QWidget *parent) : QTableView(parent)
{
  this->horizontalHeader()->setStretchLastSection( true );
  this->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделяется вся строка

  // Настройка области виджета для кинетической прокрутки
  setKineticScrollArea( qobject_cast<QAbstractItemView*>(this) );

  // Разрешение принимать жест QTapAndHoldGesture
  grabGesture(Qt::TapAndHoldGesture);
}


AttachTableView::~AttachTableView()
{

}


void AttachTableView::init(void)
{
  setupSignals();

  assemblyContextMenu();
  setContextMenuPolicy(Qt::CustomContextMenu);
}


void AttachTableView::setupSignals(void)
{
  // Сигнал чтобы показать контекстное меню по правому клику на списке записей
  connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),
          this,SLOT(onCustomContextMenuRequested(const QPoint &)));

  // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
  connect(this, SIGNAL(tapAndHoldGestureFinished(const QPoint &)),
          this, SLOT(onCustomContextMenuRequested(const QPoint &)));

  // Сигнал чтобы открыть на просмотр/редактирование файл по двойному клику
  connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
          controller, SLOT(onOpenAttach(void)));
}


void AttachTableView::setController(AttachTableController *pController)
{
  controller=pController;
}


void AttachTableView::assemblyContextMenu(void)
{
  // Конструирование меню
  contextMenu=new QMenu(this);

  // find_object() не работает, потому что при инициализации еще не вызван метод assembly() у attachTableScreen, и этому объекту не задан родитель layout
  // AttachTableScreen *screenPointer=find_object<AttachTableScreen>("attachTableScreen");
  AttachTableScreen *screenPointer=qobject_cast<AttachTableScreen *>(controller->parent());

  contextMenu->addAction(screenPointer->actionAddAttach);
  contextMenu->addAction(screenPointer->actionAddAttachFromUrl);
  contextMenu->addAction(screenPointer->actionEditFileName);
  contextMenu->addAction(screenPointer->actionDeleteAttach);
  contextMenu->addAction(screenPointer->actionSaveAsAttach);
  contextMenu->addSeparator();
  contextMenu->addAction(screenPointer->actionOpenAttach);
  contextMenu->addAction(screenPointer->actionShowAttachInfo);
}


// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool AttachTableView::event(QEvent *event)
{
  if (event->type() == QEvent::Gesture)
  {
    qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
    return gestureEvent(static_cast<QGestureEvent*>(event));
  }

  return QTableView::event(event);
}


void AttachTableView::resizeEvent(QResizeEvent *event)
{
  // Первый столбец должен занимать 80% ширины при любом размере таблицы
  QRect viewRect = this->rect();
  float columnWidth = (float) viewRect.width() * 0.8;
  this->setColumnWidth( 0, columnWidth );

  // Отрисовка родительского класса
  QTableView::resizeEvent(event);
}


void AttachTableView::paintEvent(QPaintEvent *event)
{
  QTableView::paintEvent(event);

  // Если список приаттаченных файлов пуст
  if(model()!=NULL)
    if(model()->rowCount()==0)
    {
      QPainter painter(viewport());
      painter.setPen( QApplication::palette().color(QPalette::ToolTipText) );
      painter.setFont(QFont("Arial", 14));
      painter.drawText(rect(), Qt::AlignCenter, tr("No attach files"));
    }
}


// Обработчик жестов
// Вызывается из обработчика событий event()
bool AttachTableView::gestureEvent(QGestureEvent *event)
{
  qDebug() << "In gestureEvent()" << event;

  if (QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
    tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

  return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов gestureEvent()
void AttachTableView::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
  qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

  if(gesture->state()==Qt::GestureFinished)
    if(globalParameters.getTargetOs()=="android")
      emit tapAndHoldGestureFinished( mapFromGlobal(gesture->position().toPoint()) );
}


// Открытие контекстного меню в таблице приаттаченных файлов
void AttachTableView::onCustomContextMenuRequested(const QPoint &pos)
{
  // Включение отображения меню на экране
  contextMenu->exec( viewport()->mapToGlobal(pos) );
}


// Получение номера первого выделенного элемента
int AttachTableView::getFirstSelectionPos(void)
{
 // Получение списка выделенных Item-элементов
 QModelIndexList selectItems=selectionModel()->selectedIndexes();

 if(selectItems.isEmpty())
  return -1; // Если ничего не выделено
 else
  return (selectItems.at(0)).row(); // Индекс первого выделенного элемента
}
