#include "PlacementFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"

PlacementFormatter::PlacementFormatter()
{

}


// Добавление отступа
void PlacementFormatter::onIndentplusClicked(void)
{
  // Выяснение текущего отступа
  int currentIndent=(int) textArea->textCursor().blockFormat().leftMargin();

  // Выяснение приращения отступа, заданного в конфиге редактора
  int deltaIndent=editorConfig->get_indent_step();

  // Создание форматирования
  QTextBlockFormat indentFormatting;
  indentFormatting.setLeftMargin(currentIndent+deltaIndent); // Возможно, лучше воспользоваться setTextIndent

  // Форматирование
  textArea->textCursor().mergeBlockFormat(indentFormatting);

  // editor->updateIndentsliderToActualFormat();
  emit updateIndentsliderToActualFormat();
}


// Убирание отступа
void PlacementFormatter::onIndentminusClicked(void)
{
  // Выяснение текущего отступа
  int currentIndent=(int) textArea->textCursor().blockFormat().leftMargin();

  // Выяснение приращения отступа, заданного в конфиге редактора
  int deltaIndent=editorConfig->get_indent_step();

  // Отступ, который должен быть установлен
  int indentForSet;
  if((currentIndent-deltaIndent)>0)
    indentForSet=currentIndent-deltaIndent;
  else
    indentForSet=0;

  // Создание форматирования
  QTextBlockFormat indentFormatting;
  indentFormatting.setLeftMargin(indentForSet);

  // Форматирование
  textArea->textCursor().mergeBlockFormat(indentFormatting);

  // editor->updateIndentsliderToActualFormat();
  emit updateIndentsliderToActualFormat();
}

// Задание типа выравнивания
void PlacementFormatter::setTextAlign(int align)
{
    alignText(static_cast<Qt::AlignmentFlag>(align));
}

// Выравнивание текста, вспомогательный метод
void PlacementFormatter::alignText(Qt::AlignmentFlag mode)
{
  // Создание форматирования
  QTextBlockFormat formatting;
  formatting.setAlignment(mode);

  // Форматирование
  textArea->textCursor().mergeBlockFormat(formatting);

  emit updateAlignButtonHiglight(true);
}
