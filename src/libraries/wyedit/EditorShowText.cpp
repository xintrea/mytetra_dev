#include <QVBoxLayout>

#include "EditorShowText.h"


EditorShowText::EditorShowText(QWidget *parent) : QDialog(parent)
{
  setupUi();
  setupSignals();
  assembly();
}

EditorShowText::~EditorShowText()
{

}

void EditorShowText::setupUi()
{
 QSizePolicy sizePolicy;
 sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
 sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

 textArea=new QTextEdit(this);
 textArea->setAcceptRichText(true);
 textArea->setSizePolicy(sizePolicy);
}


void EditorShowText::setupSignals()
{

}


void EditorShowText::assembly()
{
 QVBoxLayout *mainLayout=new QVBoxLayout(this);
 mainLayout->setContentsMargins(0,0,0,0);

 // Добавляется область текста
 mainLayout->addWidget(textArea);
}


void EditorShowText::setHtml(QString text)
{
 textArea->setHtml(text);
}
