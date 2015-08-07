#include "AttachTableView.h"
#include "controllers/attachTable/attachTableController.h"


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
