#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QColorDialog>
#include <QDebug>

#include "EditorTablePropertiesForm.h"
#include "../helpers/PaletteHelper.h"

EditorTablePropertiesForm::EditorTablePropertiesForm()
{
  this->directSetAlign=false;

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
  buttonBackgroundColor.addAction( new QAction(QIcon(":/resource/pic/edit_fontbackgroundcolor.svg"), tr("Select color"), this) );
  buttonBackgroundColor.addAction( new QAction(QIcon(":/resource/pic/edit_no_color.svg"), tr("No color"), this) );


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
  connect(&buttonBackgroundColor, &EditorDropDownButton::menuItemClicked,
          this, &EditorTablePropertiesForm::onClickedButtonBackgroundColor);

  connect(&buttonAlignLeft,   &QToolButton::toggled,
          this, &EditorTablePropertiesForm::onToggleButtonAlignLeft);
  connect(&buttonAlignCenter, &QToolButton::toggled,
          this, &EditorTablePropertiesForm::onToggleButtonAlignCenter);
  connect(&buttonAlignRight,  &QToolButton::toggled,
          this, &EditorTablePropertiesForm::onToggleButtonAlignRight);

  connect(&buttonBox, &QDialogButtonBox::accepted,
          this, &EditorTablePropertiesForm::accept);
  connect(&buttonBox, &QDialogButtonBox::rejected,
          this, &EditorTablePropertiesForm::reject);
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
  this->spinTableWidth.setValue(iWidth);
}


int EditorTablePropertiesForm::getTableWidth(void)
{
  return this->spinTableWidth.value();
}


void EditorTablePropertiesForm::setBorderWidth(int iWidth)
{
  this->spinBorderWidth.setValue(iWidth);
}


int EditorTablePropertiesForm::getBorderWidth(void)
{
  return this->spinBorderWidth.value();
}


void EditorTablePropertiesForm::setBackgroundColor(QColor iColor)
{
  setColorForButtonBackgroundColor(iColor);

  this->backgroundColor=iColor;
}


QColor EditorTablePropertiesForm::getBackgroundColor(void)
{
  return this->backgroundColor;
}


void EditorTablePropertiesForm::setTableAlign(TableAlign iAlign)
{
  this->tableAlign=iAlign;

  this->directSetAlign=true;

  this->buttonAlignLeft.setChecked(false);
  this->buttonAlignCenter.setChecked(false);
  this->buttonAlignRight.setChecked(false);

  if(this->tableAlign==Left)
    buttonAlignLeft.setChecked(true);

  if(this->tableAlign==Center)
    buttonAlignCenter.setChecked(true);

  if(this->tableAlign==Right)
    buttonAlignRight.setChecked(true);

  this->directSetAlign=false;
}


int EditorTablePropertiesForm::getTableAlign(void)
{
  return this->tableAlign;
}


void EditorTablePropertiesForm::setColorForButtonBackgroundColor(QColor iColor)
{
  // Квадратик на кнопке выбора цвета кода
  QPixmap pix(16, 16);

  // Заполнение есть, если цвет непрозрачный
  if ( iColor.alpha()!=0 )
  {
      pix.fill( iColor.rgb() ); // Когда цвет непрозрачный
  }
  else // Иначе цвет прозрачный
  {
      pix = QIcon(":/resource/pic/edit_no_color.svg").pixmap(16, 16);
  }

  this->buttonBackgroundColor.setIcon(pix);
}


// Слот, срабатыващий при нажатии на кнопку выбора цвета фона текста
// Параметр n - это номер выбранного пункта в выпадающей кнопке выбора цвета,
// счет кнопок с нуля.
// При n=0 нужно запустить диалог выбора цвета,
// При n=1 нужно установить стандартный цвет (нет цвета)
void EditorTablePropertiesForm::onClickedButtonBackgroundColor(int n)
{
    QColor selectedColor;

    // Пункт для выбора цвета
    if (n==0)
    {
        QColor initColor = this->backgroundColor;

        // Цвет с полной прозрачностью не может быть установлен как инициализирующий диалог выбора,
        // так как для установки полностью прозрачного цвета есть другой пункт - "без цвета"
        if ( initColor.alpha() == 0)
        {
            // Начальный цвет в диалоге выбора цвета будет обычным, без прозрачности
            // Но наличие в диалоге настройки опции QColorDialog::ShowAlphaChannel
            // позволяет пользователю менять прозразрачность как ему необходимо
            initColor.setAlpha(255);
        }

        // Диалог запроса цвета фона
        selectedColor = QColorDialog::getColor(initColor,
                                               this,
                                               tr("Select table background color"),
                                               QColorDialog::ShowAlphaChannel );
    }

    // Пункт для выбора "без цвета"
    if (n==1)
    {
        selectedColor = Qt::transparent;
    }

    qDebug() << "Select color for table background: " << PaletteHelper::getRgbaColorText(selectedColor);

    // Если цвет выбран, и он правильный
    if(selectedColor.isValid())
    {
        // Меняется цвет кнопки, и запоминается в свойстве класса
        setColorForButtonBackgroundColor(selectedColor); // this->doChangeBackgroundColor( selectedColor );
        this->backgroundColor=selectedColor;
    }
}


void EditorTablePropertiesForm::onToggleButtonAlignLeft()
{
  if(this->directSetAlign) // Отключается обработка переключения если идет прямая установка из кода (а не пользолвателем)
    return;

  setTableAlign(Left);
}


void EditorTablePropertiesForm::onToggleButtonAlignCenter()
{
  if(this->directSetAlign) // Отключается обработка переключения если идет прямая установка из кода (а не пользолвателем)
    return;

  setTableAlign(Center);
}


void EditorTablePropertiesForm::onToggleButtonAlignRight()
{
  if(this->directSetAlign) // Отключается обработка переключения если идет прямая установка из кода (а не пользолвателем)
    return;

  setTableAlign(Right);
}
