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
}


ActionLogView::~ActionLogView()
{

}


void ActionLogView::setController(ActionLogController *pController)
{
  controller=pController;
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

