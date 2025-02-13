#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QColorDialog>

#include "EditorTablePropertiesForm.h"


EditorTablePropertiesForm::EditorTablePropertiesForm()
{
  directSetAlign=false;

  setupUi();
  setupSignals();
  assembly();
}


EditorTablePropertiesForm::~EditorTablePropertiesForm()
{

}


void EditorTablePropertiesForm::setupUi()
{
  this->setWindowTitle(tr("Table properties"));

  // Первичная настройка экранных элементов без задания значений
  labelTableWidth.setText( tr("Width: ") );
  labelTablePercent.setText( tr("%") );
  spinTableWidth.setRange(1,100);
  spinTableWidth.setValue(100);

  labelBorderWidth.setText( tr("Border lines width: ") );
  labelBorderPix.setText( tr("px") );
  spinBorderWidth.setRange(0,10);

  labelBackgroundColor.setText( tr("Background color: ") );


  labelAlign.setText( tr("Table align: ") );

  buttonAlignLeft.setStatusTip(tr("Table align left"));
  buttonAlignLeft.setIcon(QIcon(":/resource/pic/edit_alignleft.svg"));
  buttonAlignLeft.setCheckable(true); // Кнопка является зажимаемой

  buttonAlignCenter.setStatusTip(tr("Table align center"));
  buttonAlignCenter.setIcon(QIcon(":/resource/pic/edit_aligncenter.svg"));
  buttonAlignCenter.setCheckable(true);

  buttonAlignRight.setStatusTip(tr("Table align right"));
  buttonAlignRight.setIcon(QIcon(":/resource/pic/edit_alignright.svg"));
  buttonAlignRight.setCheckable(true);

  buttonBox.setOrientation(Qt::Horizontal);
  buttonBox.addButton(tr("OK"),QDialogButtonBox::AcceptRole);
  buttonBox.addButton(tr("Cancel"),QDialogButtonBox::RejectRole);
}


void EditorTablePropertiesForm::setupSignals()
{
  connect(&buttonBackgroundColor, &QToolButton::clicked, this, &EditorTablePropertiesForm::onClickedButtonBackgroundColor);

  connect(&buttonAlignLeft,   &QToolButton::toggled, this, &EditorTablePropertiesForm::onToggleButtonAlignLeft);
  connect(&buttonAlignCenter, &QToolButton::toggled, this, &EditorTablePropertiesForm::onToggleButtonAlignCenter);
  connect(&buttonAlignRight,  &QToolButton::toggled, this, &EditorTablePropertiesForm::onToggleButtonAlignRight);

  connect(&buttonBox, &QDialogButtonBox::accepted, this, &EditorTablePropertiesForm::accept);
  connect(&buttonBox, &QDialogButtonBox::rejected, this, &EditorTablePropertiesForm::reject);
}


void EditorTablePropertiesForm::assembly()
{
  QGridLayout *gridLayout=new QGridLayout();

  gridLayout->addWidget(&labelTableWidth,       0, 0);
  gridLayout->addWidget(&spinTableWidth,        0, 1);
  gridLayout->addWidget(&labelTablePercent,     0, 2);

  gridLayout->addWidget(&labelBorderWidth,      1, 0);
  gridLayout->addWidget(&spinBorderWidth,       1, 1);
  gridLayout->addWidget(&labelBorderPix,        1, 2);

  gridLayout->addWidget(&labelBackgroundColor,  2, 0);
  gridLayout->addWidget(&buttonBackgroundColor, 2, 1);

  gridLayout->addWidget(&labelAlign,            3, 0);

  /*
  gridLayout->addWidget(&buttonAlignLeft,       3, 1);
  gridLayout->addWidget(&buttonAlignCenter,     3, 2);
  gridLayout->addWidget(&buttonAlignRight,      3, 3);
  */

  QHBoxLayout *alignBox=new QHBoxLayout();
  alignBox->addWidget(&buttonAlignLeft);
  alignBox->addWidget(&buttonAlignCenter);
  alignBox->addWidget(&buttonAlignRight);
  gridLayout->addLayout(alignBox, 3, 1);

  // Максимально растягивается по ширине первый столбец
  gridLayout->setColumnStretch(0, 1);

  QVBoxLayout *mainLayout=new QVBoxLayout(this);
  mainLayout->addLayout(gridLayout);
  mainLayout->addWidget(&buttonBox);
}


void EditorTablePropertiesForm::setTableWidth(int iWidth)
{
  spinTableWidth.setValue(iWidth);
}


int EditorTablePropertiesForm::getTableWidth(void)
{
  return spinTableWidth.value();
}


void EditorTablePropertiesForm::setBorderWidth(int iWidth)
{
  spinBorderWidth.setValue(iWidth);
}


int EditorTablePropertiesForm::getBorderWidth(void)
{
  return spinBorderWidth.value();
}


void EditorTablePropertiesForm::setBackgroundColor(QColor iColor)
{
  setColorForButtonBackgroundColor(iColor);
}


QColor EditorTablePropertiesForm::getBackgroundColor(void)
{
  return backgroundColor;
}


void EditorTablePropertiesForm::setTableAlign(TableAlign iAlign)
{
  tableAlign=iAlign;

  directSetAlign=true;

  buttonAlignLeft.setChecked(false);
  buttonAlignCenter.setChecked(false);
  buttonAlignRight.setChecked(false);

  if(tableAlign==Left)
    buttonAlignLeft.setChecked(true);

  if(tableAlign==Center)
    buttonAlignCenter.setChecked(true);

  if(tableAlign==Right)
    buttonAlignRight.setChecked(true);

  directSetAlign=false;
}


int EditorTablePropertiesForm::getTableAlign(void)
{
  return tableAlign;
}


void EditorTablePropertiesForm::setColorForButtonBackgroundColor(QColor iColor)
{
  // Квадратик на кнопке выбора цвета кода
  QPixmap pix(16, 16);
  pix.fill(iColor.rgb());
  buttonBackgroundColor.setIcon(pix);

  backgroundColor=iColor;
}


void EditorTablePropertiesForm::onClickedButtonBackgroundColor()
{
  // Диалог запроса цвета (доработать)
  QColor selectedColor=QColorDialog::getColor(backgroundColor, this, QString(), QColorDialog::ShowAlphaChannel);

  // Если цвет выбран, и он правильный
  if(selectedColor.isValid())
    setColorForButtonBackgroundColor(selectedColor);
}


void EditorTablePropertiesForm::onToggleButtonAlignLeft()
{
  if(directSetAlign) // Отключается обработка переключения если идет прямая установка из кода (а не пользолвателем)
    return;

  setTableAlign(Left);
}


void EditorTablePropertiesForm::onToggleButtonAlignCenter()
{
  if(directSetAlign) // Отключается обработка переключения если идет прямая установка из кода (а не пользолвателем)
    return;

  setTableAlign(Center);
}


void EditorTablePropertiesForm::onToggleButtonAlignRight()
{
  if(directSetAlign) // Отключается обработка переключения если идет прямая установка из кода (а не пользолвателем)
    return;

  setTableAlign(Right);
}
