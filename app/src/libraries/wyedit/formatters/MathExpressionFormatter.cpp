#include <QTextBlock>
#include <QTextFragment>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QTextDocumentFragment>
#include <QMessageBox>
#include <QImage>

#include "MathExpressionFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorCursorPositionDetector.h"

#include "main.h"


MathExpressionFormatter::MathExpressionFormatter()
{

}


// Содержимое формулы, которая выделена (если выделена единственная картинка формулы)
QString MathExpressionFormatter::mathExpressionOnSelect(void)
{
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
            QTextImageFormat imageFormat=fragment.charFormat().toImageFormat();

            return QString();
          }
        }
    }
  }

  return QString();
}


// Сождержимое формулы (картинки формулы) на которой находится курсор
QString MathExpressionFormatter::mathExpressionOnCursor(void)
{
  // Проверка срабатывает только если нет выделения
  if(textArea->textCursor().hasSelection()==false)
  {
    QTextImageFormat imageFormat = textArea->textCursor().charFormat().toImageFormat();

    if(imageFormat.isValid())
      return QString();
  }

  return QString();
}


void MathExpressionFormatter::editMathExpression(void)
{


}


// Обработка клавиши добавления картинки
void MathExpressionFormatter::onMathExpressionClicked(void)
{

  // Если выделена картинка
  if(editor->cursorPositionDetector->isImageSelect() ||
     editor->cursorPositionDetector->isCursorOnImage())
  {
    qDebug() << "Math expression selected";

    editMathExpression();
  }
  else
  {
    // Иначе математическое выражение не выделено, и срабатывает режим добавления

  }

}


// Вызов окна настройки свойств изображения
void MathExpressionFormatter::onContextMenuEditMathExpression()
{
  // Если выделено математичсекое выражение
  if(editor->cursorPositionDetector->isImageSelect() ||
     editor->cursorPositionDetector->isCursorOnImage())
  {
    qDebug() << "Math expression selected";

    editMathExpression();
  }
}

