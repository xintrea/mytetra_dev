#include "Formatter.h"


Formatter::Formatter(QObject *parent) : QObject(parent)
{

}


// Установка редактора, для вызова служебных функций
void Formatter::setEditor(Editor *iEditor)
{
  editor=iEditor;

  editorConfig=editor->editorConfig;
}


// Установка области редактирования, с которой должен работать форматировщик
void Formatter::setTextArea(EditorTextArea *iTextArea)
{
  textArea=iTextArea;
}

