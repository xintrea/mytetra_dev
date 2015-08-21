#include <QHeaderView>

#include "AttachTableView.h"
#include "controllers/attachTable/AttachTableController.h"


AttachTableView::AttachTableView(QWidget *parent) : QTableView(parent)
{
  this->horizontalHeader()->setStretchLastSection( true );
  this->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделяется вся строка
}


AttachTableView::~AttachTableView()
{

}


void AttachTableView::setController(AttachTableController *pController)
{
  controller=pController;
}


void AttachTableView::resizeEvent(QResizeEvent *event)
{
  QRect viewRect = this->rect();
  float columnWidth = (float) viewRect.width() * 0.8;
  this->setColumnWidth( 0, columnWidth );

  // Отрисовка родительского класса
  QTableView::resizeEvent(event);
}
