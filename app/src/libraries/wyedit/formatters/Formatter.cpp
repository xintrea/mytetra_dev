#include "Formatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"


Formatter::Formatter(QObject *parent) : QObject(parent)
{

}


Formatter::~Formatter()
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

