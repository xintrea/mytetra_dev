#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

#include "EditorImageProperties.h"


EditorImageProperties::EditorImageProperties(QWidget *parent) : QDialog(parent)
{
 imageRealWidth=0;
 imageRealHeight=0;

 // Флаг, используемый для запрещения цикличного взаимного изменения
 // высоты и ширины
 isRelateSizeSetted=false;

 setup_ui();
 setup_signals();
 assembly();
}


EditorImageProperties::~EditorImageProperties()
{

}


void EditorImageProperties::setup_ui()
{
 infoLabel=new QLabel(" ");

 percentSizeLabel=new QLabel(tr("Size (%): "));

 percentSizeSpin=new QSpinBox;
 percentSizeSpin->setRange(1,500);
 percentSizeSpin->setValue(100);


 widthLabel=new QLabel(tr("Width (pixels): "));

 widthSpin=new QSpinBox;
 widthSpin->setRange(1,4096);
 widthSpin->setValue(100);


 heightLabel=new QLabel(tr("Height (pixels): "));

 heightSpin=new QSpinBox;
 heightSpin->setRange(1,4096);
 heightSpin->setValue(100);


 resetSizeButton=new QPushButton(tr("Reset to real image size"));
 resetSizeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));


 buttonBox=new QDialogButtonBox(Qt::Horizontal);
 buttonBox->addButton(tr("OK"),QDialogButtonBox::AcceptRole);
 buttonBox->addButton(tr("Cancel"),QDialogButtonBox::RejectRole);


 this->setWindowTitle(tr("Image properies"));
}


void EditorImageProperties::setup_signals()
{
 connect(percentSizeSpin, SIGNAL(valueChanged(int) ), this, SLOT(on_changed_percent(int)) );
 connect(widthSpin, SIGNAL(valueChanged(int) ), this, SLOT(on_changed_width(int)) );
 connect(heightSpin, SIGNAL(valueChanged(int) ), this, SLOT(on_changed_height(int)) );

 connect(resetSizeButton, SIGNAL(pressed() ), this, SLOT(on_click_reset_size()) );

 connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
 connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
}


void EditorImageProperties::assembly()
{
 QVBoxLayout *mainLayout=new QVBoxLayout(this);


 // Область запроса размеров картинки
 QGridLayout *gridLayout=new QGridLayout();

 gridLayout->addWidget(percentSizeLabel, 0, 0);
 gridLayout->addWidget(percentSizeSpin,  0, 1);

 gridLayout->addWidget(widthLabel, 1, 0);
 gridLayout->addWidget(widthSpin,  1, 1);

 gridLayout->addWidget(heightLabel, 2, 0);
 gridLayout->addWidget(heightSpin,  2, 1);

 // Кнопка сбора размера к настоящему размеру картинки
 gridLayout->addWidget(resetSizeButton, 3, 1);

 // Добавляется область запроса размеров картинки на основной слой
 mainLayout->addLayout(gridLayout);


 // Добавляется строка с информацией о картинке
 QGroupBox *groupBox = new QGroupBox(tr("Image info"));
 QVBoxLayout *vbox = new QVBoxLayout;
 vbox->addWidget(infoLabel);
 groupBox->setLayout(vbox);
 mainLayout->addWidget(groupBox);


 // Добавляется линейка кнопок OK и Cancel на основной слой
 mainLayout->addWidget(buttonBox);
}


void EditorImageProperties::set_info(QString infoText)
{
 infoLabel->setText(infoText);
}


void EditorImageProperties::set_real_width(int width)
{
 imageRealWidth=width;
}


void EditorImageProperties::set_real_height(int height)
{
 imageRealHeight=height;
}


void EditorImageProperties::set_width(int width)
{
 isRelateSizeSetted=true;
 widthSpin->setValue(width);
 isRelateSizeSetted=false;
}


void EditorImageProperties::set_height(int height)
{
 isRelateSizeSetted=true;
 heightSpin->setValue(height);
 isRelateSizeSetted=false;
}


int EditorImageProperties::get_width(void)
{
 return widthSpin->value();
}


int EditorImageProperties::get_height(void)
{
 return heightSpin->value();
}


// Расчет размеров картинки в процентах на основании реальных размеров
// и заданных, и обновление виджета, отображающего проценты
void EditorImageProperties::update_percent(void)
{
 if(imageRealWidth==0 || imageRealHeight==0) return;

 int calculatePercent;

 // Расчет процента далается на основании более длинной стороны рисунка
 if(imageRealWidth>imageRealHeight)
  calculatePercent=(widthSpin->value()*100)/imageRealWidth;
 else
  calculatePercent=(heightSpin->value()*100)/imageRealHeight;

 isRelateSizeSetted=true;
 percentSizeSpin->setValue(calculatePercent);
 isRelateSizeSetted=false;
}


void EditorImageProperties::on_changed_percent(int n)
{
 // Запрещается "циклическое" изменение ширины и высоты
 if(isRelateSizeSetted) return;
 
 int calculateWidth;
 int calculateHeight;

 calculateWidth=(n*imageRealWidth)/100;
 calculateHeight=(n*imageRealHeight)/100;

 isRelateSizeSetted=true;
 widthSpin->setValue(calculateWidth);
 heightSpin->setValue(calculateHeight);
 isRelateSizeSetted=false;
}


void EditorImageProperties::on_changed_width(int n)
{
 if(imageRealWidth==0 || imageRealHeight==0) return;

 // Запрещается "циклическое" изменение ширины и высоты
 if(isRelateSizeSetted) return;

 // Новая высота
 int h=(n*imageRealHeight)/imageRealWidth;

 // Новый процент
 int percent=(h*100)/imageRealHeight;

 isRelateSizeSetted=true;
 heightSpin->setValue(h);
 percentSizeSpin->setValue(percent);
 isRelateSizeSetted=false;
}


void EditorImageProperties::on_changed_height(int n)
{
 if(imageRealHeight==0 || imageRealHeight==0) return;

 // Запрещается "циклическое" изменение ширины и высоты
 if(isRelateSizeSetted) return;

 // Новая ширина
 int w=(n*imageRealWidth)/imageRealHeight;

 // Новый процент
 int percent=(w*100)/imageRealWidth;

 isRelateSizeSetted=true;
 widthSpin->setValue(w);
 percentSizeSpin->setValue(percent);
 isRelateSizeSetted=false;
}

void EditorImageProperties::on_click_reset_size()
{
 set_width(imageRealWidth);
 set_height(imageRealHeight);
 update_percent();
}
