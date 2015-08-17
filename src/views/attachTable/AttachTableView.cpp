#include "AttachTableView.h"
#include "controllers/attachTable/AttachTableController.h"


AttachTableView::AttachTableView(QWidget *parent) : QTableView(parent)
{

}


AttachTableView::~AttachTableView()
{

}


void AttachTableView::setController(AttachTableController *pController)
{
  controller=pController;
}
