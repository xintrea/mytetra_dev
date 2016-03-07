#include <QHeaderView>
#include <QMenu>
#include <QDebug>

#include "main.h"
#include "ActionLogView.h"
#include "ActionLogScreen.h"
#include "controllers/actionLog/ActionLogController.h"
#include "libraries/GlobalParameters.h"
#include "views/mainWindow/MainWindow.h"


extern GlobalParameters globalParameters;


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


void ActionLogView::assemblyContextMenu()
{
  ActionLogScreen *parentPointer=qobject_cast<ActionLogScreen *>(parent());

  contextMenu.addAction( parentPointer->actionCopy );
}

