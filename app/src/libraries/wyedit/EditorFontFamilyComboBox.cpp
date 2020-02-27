#include <QDebug>
#include <QtGlobal>

#include "EditorFontFamilyComboBox.h"


EditorFontFamilyComboBox::EditorFontFamilyComboBox(QWidget *parent) :
    QFontComboBox(parent),
    toolFocus(this)
{
    Q_UNUSED(parent)

    isProgrammChanged=false;

    // Пустой пункт в конце списка шрифтов, используется для обозначения что в
    // выделенном тексте несколько шрифтов
    this->insertItem(0, "");
    this->setItemIcon(0, QIcon(":/resource/pic/edit_font_many.svg")); // Установка иконки в QFontComboBox не работает, но пока оставлено

    previousIndex=this->currentIndex();

    QRegularExpression rx(".+"); // 1 или больше символов (т. е. не пустая строка)
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    this->setValidator(validator);

    this->setupSignals();
}


EditorFontFamilyComboBox::~EditorFontFamilyComboBox()
{

}


void EditorFontFamilyComboBox::setupSignals()
{
    connect(this, qOverload<int>(&EditorFontFamilyComboBox::currentIndexChanged),
            this, &EditorFontFamilyComboBox::onCurrentIndexChanged);
}


void EditorFontFamilyComboBox::setIsProgrammChanged(bool flag)
{
    isProgrammChanged=flag;
}


// Данный слот срабатывает уже после того, как в объекте выставлен индекс
void EditorFontFamilyComboBox::onCurrentIndexChanged(int index)
{
    // Устанавливать неизвестный размер шрифта можно только программно
    if(!isProgrammChanged && index==0) {
        qDebug() << "EditorFontFamilyComboBox::onCurrentIndexChanged. Disable manual set empty font.";
        this->setCurrentIndex(previousIndex);
        return;
    }

    previousIndex=index;
}



