#include "MtTableWidget.h"

// Этот класс не используется, скорее всего будет удален

MtTableWidget::MtTableWidget(QWidget *parent) : QTableWidget(parent)
{
 Q_UNUSED(parent);

 itemDelegate = new QStyledItemDelegate();
 this->setItemDelegate(itemDelegate);
}


MtTableWidget::~MtTableWidget()
{
 delete itemDelegate;
}
