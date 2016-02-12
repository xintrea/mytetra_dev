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
  setupUi();
  setupSignals();
  assembly();
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

  buttonBox.setOrientation(Qt::Horizontal);
  buttonBox.addButton(tr("OK"),QDialogButtonBox::AcceptRole);
  buttonBox.addButton(tr("Cancel"),QDialogButtonBox::RejectRole);
}


void EditorTablePropertiesForm::setupSignals()
{
  connect(&buttonBackgroundColor, SIGNAL(clicked()), this, SLOT(onClickedButtonBackgroundColor()) );

  connect(&buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(&buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
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
  QColor selectedColor=QColorDialog::getColor(backgroundColor, this);

  // Если цвет выбран, и он правильный
  if(selectedColor.isValid())
    setColorForButtonBackgroundColor(selectedColor);
}



