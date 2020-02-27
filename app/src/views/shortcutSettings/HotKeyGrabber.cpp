#include <QKeyEvent>
#include <QDebug>

#include "HotKeyGrabber.h"


HotKeyGrabber::HotKeyGrabber(QWidget* parent) : QWidget(parent)
{
    this->setupUI();
    this->setupSignals();
    this->assembly();
}


HotKeyGrabber::~HotKeyGrabber()
{

}


void HotKeyGrabber::setupUI()
{
    this->keySequenceEdit=new QKeySequenceEdit(this);

    this->buttonCancel=new QPushButton(this);
    this->buttonCancel->setText("Cancel");
}


void HotKeyGrabber::setupSignals()
{
    // Обработка кнопки Cansel
    connect(this->buttonCancel, &QPushButton::clicked,
            this, &HotKeyGrabber::hide);

    // Аналог сигнала QKeySequenceEdit::editingFinished испускается от виджета
    connect(this->keySequenceEdit, &QKeySequenceEdit::editingFinished,
            this, &HotKeyGrabber::editingFinished);
}


void HotKeyGrabber::assembly()
{
    this->layout=new QHBoxLayout();
    this->layout->addWidget(this->keySequenceEdit);
    this->layout->addWidget(this->buttonCancel);

    setLayout(this->layout);
}


void HotKeyGrabber::clear()
{
    keySequenceEdit->clear();
}


QKeySequence HotKeyGrabber::keySequence()
{
    return keySequenceEdit->keySequence();
}


