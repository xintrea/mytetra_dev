#include <QDebug>

#include "EditorFontSizeComboBox.h"


EditorFontSizeComboBox::EditorFontSizeComboBox(QWidget *parent) : MtComboBox(parent)
{
    Q_UNUSED(parent);

    // Пустой пункт в начале списка размеров шрифтов, используется для обозначения что в выделенном тексте несколько размеров
    this->addItem("-",0);

    // Заполнение списка значениями размера шрифта
    for(int i=MINIMUM_ALLOWED_FONT_SIZE; i<=MAXIMUM_ALLOWED_FONT_SIZE; i++)
      this->addItem(QString("%1").arg(i),i);

    // Значение по-умолчанию
    this->setCurrentIndex(this->findData(10));

    this->setMinimumContentsLength(3);
    this->setEditable(true);

    // Устанавливается валидатор, однако он не работат как требуется, поэтому нужен дополнительный контроль
    QValidator *fontsizeValidator = new QIntValidator(MINIMUM_ALLOWED_FONT_SIZE, MAXIMUM_ALLOWED_FONT_SIZE, this);
    this->setValidator(fontsizeValidator);


    connect( this, SIGNAL(currentTextChanged(QString)), this, SLOT(onCurrentTextChanged(QString)));

}


EditorFontSizeComboBox::~EditorFontSizeComboBox()
{

}


void EditorFontSizeComboBox::onCurrentTextChanged(QString text)
{
    qDebug() << "EditorFontSizeComboBox::onCurrentTextChanged: " << text;
}

