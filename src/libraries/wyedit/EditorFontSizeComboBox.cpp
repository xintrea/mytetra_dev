#include <QDebug>

#include "EditorFontSizeComboBox.h"


EditorFontSizeComboBox::EditorFontSizeComboBox(QWidget *parent) : MtComboBox(parent)
{
    Q_UNUSED(parent);

    isProgrammChanged=false;

    // Пустой пункт в начале списка размеров шрифтов, используется для обозначения что в выделенном тексте несколько размеров
    this->addItem("",0);

    // Заполнение списка значениями размера шрифта
    for(int i=MINIMUM_ALLOWED_FONT_SIZE; i<=MAXIMUM_ALLOWED_FONT_SIZE; i++)
      this->addItem(QString("%1").arg(i),i);

    // Значение по-умолчанию
    this->setCurrentIndex(this->findData(10));

    this->setMaxVisibleItems(11);
    this->setMinimumContentsLength(2);
    this->setEditable(true);

    previousIndex=this->currentIndex();

    // Устанавливается валидатор, однако он не работат как требуется, поэтому нужен дополнительный контроль
    QValidator *fontsizeValidator = new QIntValidator(MINIMUM_ALLOWED_FONT_SIZE, MAXIMUM_ALLOWED_FONT_SIZE, this);
    this->setValidator(fontsizeValidator);


    connect( this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}


EditorFontSizeComboBox::~EditorFontSizeComboBox()
{

}


void EditorFontSizeComboBox::setIsProgrammChanged(bool flag)
{
    isProgrammChanged=flag;
}


// Данный слот срабатывает уже после того, как в объекте выставлен индекс
void EditorFontSizeComboBox::onCurrentIndexChanged(int index)
{
    // Устанавливать неизвестный размер шрифта можно только программно
    if(!isProgrammChanged && index==0) {
        qDebug() << "EditorFontSizeComboBox::onCurrentTextChanged. Disable manual set dash.";
        this->setCurrentIndex(previousIndex);
        return;
    }

    previousIndex=this->currentIndex();
}


/*
// Соответствующий сигнал выдается при каждом изменении хотя бы одной буквы/цифры в поле ввода
void EditorFontSizeComboBox::onCurrentTextChanged(QString text)
{
    // Устанавливать неизвестный размер шрифта можно только программно
    if(!isProgrammChanged && text=="-") {
        qDebug() << "EditorFontSizeComboBox::onCurrentTextChanged. Disable manual set dash.";
        this->setCurrentText(previousText);
        return;
    }

    // Если значение не неизвестный размер шрифта, то это должны быть только числа в разрешенном диапазоне
    if(text!="-") {
        bool isDisable=false;

        bool ok;
        int value=text.toInt(&ok);
        qDebug() << "EditorFontSizeComboBox::onCurrentTextChanged. Text: " << text << " Convert result: " << ok << " Value: " << value;

        if(ok==false) {
            qDebug() << "EditorFontSizeComboBox::onCurrentTextChanged. Ok false";
            isDisable=true;
        }

        if(value<MINIMUM_ALLOWED_FONT_SIZE || value>MAXIMUM_ALLOWED_FONT_SIZE) {
            qDebug() << "EditorFontSizeComboBox::onCurrentTextChanged. Value incorrect";
            isDisable=true;
        }

        if(isDisable) {
            qDebug() << "EditorFontSizeComboBox::onCurrentTextChanged. Disable set incorrect font size.";
            this->setCurrentText(previousText);
            return;
        }
    }

    previousText=this->currentText();
}
*/

