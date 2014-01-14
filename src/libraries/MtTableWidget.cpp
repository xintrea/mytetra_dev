#include "MtTableWidget.h"


MtTableWidget::MtTableWidget(QWidget *parent) : QTableWidget(parent)
{
 Q_UNUSED(parent);

 itemDelegate = new QStyledItemDelegate();
 this->setItemDelegate(itemDelegate);
}


MtTableWidget::~MtTableWidget()
{
 if(itemDelegate!=NULL)
  delete itemDelegate;
}
