#include <QTextImageFormat>
#include <QTextBlock>
#include <QTextFragment>
#include <QTextCursor>


#include "EditorCursorPositionDetector.h"
#include "EditorTextArea.h"


EditorCursorPositionDetector::EditorCursorPositionDetector()
{

}


void EditorCursorPositionDetector::setTextArea(EditorTextArea *iTextArea)
{
  textArea=iTextArea;
}


// Проверка, находится ли курсор на позиции, где находится картинка
bool EditorCursorPositionDetector::isCursorOnImage(void)
{
  // Проверка срабатывает только если нет выделения
  if(textArea->textCursor().hasSelection()==false)
  {
    QTextImageFormat imageFormat = textArea->textCursor().charFormat().toImageFormat();

    if(imageFormat.isValid())
      return true;
  }

  return false;
}


// Метод, определяющий, выбрана ли только одна картинка
bool EditorCursorPositionDetector::isImageSelect(void)
{
  // Происходит анализ, выделена ли картинка
  bool is_image_select_flag=false;

  // Блок, в пределах которого находится курсор
  QTextBlock currentBlock = textArea->textCursor().block();
  QTextBlock::iterator it;
  QTextFragment fragment;

  // Если есть выделение
  if(textArea->textCursor().hasSelection())
  {
    // Перебиратся фрагметы блока
    for(it = currentBlock.begin(); !(it.atEnd()); ++it)
    {
      fragment = it.fragment();

      // Если фрагмент содержит изображение
      if(fragment.isValid())
        if(fragment.charFormat().isImageFormat ())
        {
          int fragmentStart=fragment.position();
          int fragmentEnd=fragmentStart+fragment.length();
          int selectionStart=textArea->textCursor().selectionStart();
          int selectionEnd=textArea->textCursor().selectionEnd();

          // Если начало и конец фрагмента совпадает с координатами выделения
          // Проверяется и случай, когда блок выделен в обратную сторону
          if( (fragmentStart==selectionStart && fragmentEnd==selectionEnd) ||
              (fragmentStart==selectionEnd && fragmentEnd==selectionStart) )
          {
            is_image_select_flag=true;
            break;
          }
        }
    }
  }

  return is_image_select_flag;
}


bool EditorCursorPositionDetector::isCursorOnSpaceLine(void)
{
  // Если есть выделение, функция работать не должна
  if(textArea->textCursor().hasSelection())
    return false;

  // Создаётся дополнительный курсор
  QTextCursor cursor=textArea->textCursor();

  // Выделяется анализируемая строка
  cursor.select(QTextCursor::LineUnderCursor);

  // Текст анализируемой строки
  QString selectedText=cursor.selectedText();

  if(selectedText.simplified().size()>0)
    return false;
  else
    return true;
}


// Стоит ли курсор на пустой строке (т.е. в строке есть только символ
// перевода на новую строку)
bool EditorCursorPositionDetector::isCursorOnEmptyLine(void)
{
  if(!textArea->textCursor().atBlockStart())
    return false;

  // Создаётся дополнительный курсор
  QTextCursor cursor=textArea->textCursor();

  // Выделяется анализируемая строка
  cursor.select(QTextCursor::LineUnderCursor);

  // Текст анализируемой строки
  QString selectedText=cursor.selectedText();

  if(selectedText.size()>0)
    return false;
  else
    return true;
}


// Выбран ли блок текста (т.е. находятся ли начало и конец выделения
// точно на границах блока)
bool EditorCursorPositionDetector::isBlockSelect(void)
{
  // Выясняются позиции начала и конца выделения
  int start=textArea->textCursor().selectionStart();
  int stop =textArea->textCursor().selectionEnd();

  // Высталяются флаги, находится ли начало и конец выделения на границах блока.
  // Это определяется с помощью дополнительного курсора
  int flagStartBlockEdge=0;
  int flagStopBlockEdge=0;
  QTextCursor cursor=textArea->textCursor();
  cursor.setPosition(start);
  if(cursor.atBlockStart()==true || cursor.atBlockEnd()==true)
    flagStartBlockEdge=1;
  cursor.setPosition(stop);
  if(cursor.atBlockStart()==true || cursor.atBlockEnd()==true)
    flagStopBlockEdge=1;

  if(flagStartBlockEdge==1 && flagStopBlockEdge==1)
    return true;
  else
    return false;
}


// Проверка, находится ли курсор на ссылке
bool EditorCursorPositionDetector::isCursorOnReference(void)
{
  // todo: доработать этот метод

  // Проверка срабатывает только если нет выделения
  if(textArea->textCursor().hasSelection()==false)
  {
    QTextImageFormat imageFormat = textArea->textCursor().charFormat().toImageFormat();

    if(imageFormat.isValid())
      return true;
  }

  return false;
}
