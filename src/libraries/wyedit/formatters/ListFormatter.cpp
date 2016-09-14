#include <QTextList>
#include <QDebug>

#include "ListFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorCursorPositionDetector.h"


ListFormatter::ListFormatter()
{

}

// Форматирование в нумерованный список
void ListFormatter::onNumericlistClicked(void)
{
  formatToList(QTextListFormat::ListDecimal);
}


// Форматирование в список с точечками
void ListFormatter::onDotlistClicked(void)
{
  formatToList(QTextListFormat::ListDisc);
}


void ListFormatter::formatToList(QTextListFormat::Style setFormat)
{
  // Если выделения нет
  if(!textArea->textCursor().hasSelection())
    return;

  // Форматирование в список возможно только если выделен блок
  if(!editor->cursorPositionDetector->isBlockSelect())
    return;

  // Если строки выбраны
  if(textArea->textCursor().hasSelection())
  {
    if(textArea->textCursor().currentList()==0 ||
       textArea->textCursor().currentList()->format().style()!=setFormat)
    {
      // Cтроки еще не отформатированы в данный вид списка,
      // надо отформатировать в список

      qDebug() << "Formatting to list";

      // Применение форматирования
      textArea->textCursor().createList(setFormat);
    }
    else
    {
      // Строки уже были отформатированы в список, надо форматирование
      // сбросить в стандартное

      qDebug() << "Remove list formatting";

      // Выяснение текущего отступа
      int currentIndent;
      currentIndent=(int) textArea->textCursor().blockFormat().leftMargin();

      // Создание форматирования
      QTextBlockFormat indentFormatting;
      indentFormatting.setLeftMargin(currentIndent);

      // Форматирование
      textArea->textCursor().setBlockFormat(indentFormatting);

      // Создание форматирования по умолчанию чтобы убрать форматирование в список
      // QTextBlockFormat formatting;
      // formatting.setAlignment(Qt::AlignLeft);

      // Форматирование
      // textarea->textCursor().setBlockFormat(formatting);
    }
  }
  else
  {
    // Вставляется первый пустой пункт
    textArea->textCursor().insertList(setFormat);
  }

  // Выравнивание прокрутки чтоб курсор был виден если он уехал вниз
  textArea->ensureCursorVisible();
}

