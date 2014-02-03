#include "main.h"
#include "MtComboBox.h"

MtComboBox::MtComboBox(QWidget *parent) : QComboBox(parent)
{
 Q_UNUSED(parent);

 itemDelegate = new QStyledItemDelegate();
 this->setItemDelegate(itemDelegate);
}

MtComboBox::~MtComboBox()
{
 delete itemDelegate;
}

