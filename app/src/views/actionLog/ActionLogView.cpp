#include <QHeaderView>
#include <QMenu>
#include <QDebug>

#include "main.h"
#include "ActionLogView.h"
#include "ActionLogScreen.h"
#include "controllers/actionLog/ActionLogController.h"
#include "views/mainWindow/MainWindow.h"
#include "models/appConfig/AppConfig.h"


extern AppConfig mytetraConfig;


ActionLogView::ActionLogView(QWidget *parent) : QTableView(parent)
{

}


ActionLogView::~ActionLogView()
{

}


void ActionLogView::init()
{
  this->horizontalHeader()->setStretchLastSection( true ); // Растягивание последней секции до размеров виджета
  this->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделяется вся строка
  this->horizontalHeader()->setHighlightSections(false); // Заголовки не должны выглядеть нажатыми
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключается горизонтальная прокрутка

  // Установка ширины и высоты окна
  int dialogWidth=int( 0.8 * (float)(find_object<MainWindow>("mainwindow")->width()) );
  int dialogHeight=int( 0.8 * (float)(find_object<MainWindow>("mainwindow")->height()) );
  setMinimumWidth( dialogWidth );
  setMinimumHeight( dialogHeight );
  resize( size() );

  // Установка ширины столбцов
  // this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive); // Interactive, ResizeToContents
  this->resizeColumnToContents(0);

  // Настройка области виджета для кинетической прокрутки
  setKineticScrollArea( qobject_cast<QAbstractItemView*>(this) );

  assemblyContextMenu();
  setContextMenuPolicy(Qt::CustomContextMenu);

  setupSignals();
}


void ActionLogView::setupSignals(void)
{
  // Сигнал чтобы показать контекстное меню по правому клику на списке записей
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(onCustomContextMenuRequested(const QPoint &)));

  // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
  connect(this, SIGNAL(tapAndHoldGestureFinished(const QPoint &)),
          this, SLOT(onCustomContextMenuRequested(const QPoint &)));
}


void ActionLogView::setController(ActionLogController *pController)
{
  controller=pController;
}


void ActionLogView::resizeEvent(QResizeEvent *event)
{
  // Второй столбец должен занимать 80% ширины при любом размере таблицы
  // QRect viewRect = this->rect();
  // float columnWidth = (float) viewRect.width() * 0.8;
  // this->setColumnWidth( 1, columnWidth );

  // Отрисовка родительского класса
  QTableView::resizeEvent(event);
}


void ActionLogView::paintEvent(QPaintEvent *event)
{
  QTableView::paintEvent(event);

  // Если нет записей лога
  if(model()!=NULL)
    if(model()->rowCount()==0 && mytetraConfig.getEnableLogging()==false)
    {
      QPainter painter(viewport());
      painter.setPen( QApplication::palette().color(QPalette::ToolTipText) ); // Qt::gray
      // painter.setFont(QFont("Arial", 14));
      painter.drawText(rect(), Qt::AlignCenter, tr("Action log is empty\nPlease enable action logging in Tools -> Preferences -> Misc"));
    }
}


void ActionLogView::assemblyContextMenu()
{
  ActionLogScreen *parentPointer=qobject_cast<ActionLogScreen *>(parent());

  contextMenu.addAction( parentPointer->actionCopy );
}


// Открытие контекстного меню в таблице конечных записей
void ActionLogView::onCustomContextMenuRequested(const QPoint &pos)
{
  qDebug() << "In ActionLogView::on_customContextMenuRequested";

  // Включение отображения меню на экране
  // menu.exec(event->globalPos());
  contextMenu.exec( viewport()->mapToGlobal(pos) );
}

