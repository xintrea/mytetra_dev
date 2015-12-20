#include <QTextBlock>
#include <QDebug>
#include <QColorDialog>
#include <QColor>

#include "TypefaceFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"


TypefaceFormatter::TypefaceFormatter()
{

}


// Форматирование Bold
void TypefaceFormatter::onBoldClicked(void)
{
  // Если выделение есть
  if(textArea->textCursor().hasSelection())
  {
    // Обычное форматирование
    if(textArea->fontWeight() != QFont::Bold)
      textArea->setFontWeight(QFont::Bold); // Bold
    else
      textArea->setFontWeight(0); // Remove Bold
  }
  else
  {
    // Иначе надо выделить дополнительным курсором слово на
    // котором стоит курсор
    QTextCursor cursor=textArea->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);

    // К выделению применяется форматирование Bold
    QTextCharFormat format;
    if(cursor.charFormat().fontWeight() != QFont::Bold)
      format.setFontWeight(QFont::Bold);
    else
      format.setFontWeight(0);
    cursor.mergeCharFormat(format);
  }

  emit updateOutlineButtonHiglight();
}


// Форматирование Italic
void TypefaceFormatter::onItalicClicked(void)
{
  // Если выделение есть
  if(textArea->textCursor().hasSelection())
  {
    // Обычное форматирование
    if(!textArea->fontItalic())
      textArea->setFontItalic(true);
    else
      textArea->setFontItalic(false);
  }
  else
  {
    // Иначе надо выделить дополнительным курсором слово на
    // котором стоит курсор
    QTextCursor cursor=textArea->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);

    // К выделению применяется форматирование Italic
    QTextCharFormat format;
    if(!cursor.charFormat().fontItalic())
      format.setFontItalic(true);
    else
      format.setFontItalic(false);
    cursor.mergeCharFormat(format);
  }

  emit updateOutlineButtonHiglight();
}


// Форматирование подчеркивания
void TypefaceFormatter::onUnderlineClicked(void)
{
  // Если выделение есть
  if(textArea->textCursor().hasSelection())
  {
    // Обычное форматирование
    if(!textArea->fontUnderline())
      textArea->setFontUnderline(true);
    else
      textArea->setFontUnderline(false);
  }
  else
  {
    // Иначе надо выделить дополнительным курсором слово на
    // котором стоит курсор
    QTextCursor cursor=textArea->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);

    // К выделению применяется форматирование Underline
    QTextCharFormat format;
    if(!cursor.charFormat().fontUnderline())
      format.setFontUnderline(true);
    else
      format.setFontUnderline(false);
    cursor.mergeCharFormat(format);
  }

  emit updateOutlineButtonHiglight();
}


// Форматирование моноширинным шрифтом
void TypefaceFormatter::onMonospaceClicked(void)
{
  textArea->textCursor().beginEditBlock();

  // Устанавливается шрифт
  QFont font;
  font.fromString(editorConfig->get_monospace_font());
  emit changeFontFamily(font.family());

  // Новый установленный шрифт показывается в выпадающем списке шрифтов
  emit changeFontselectOnDisplay(font.family());

  // Если в настройках разрешена автоматическая установка нужного размера
  // моноширинного шрифта
  if(editorConfig->get_monospace_font_size_apply()==true)
  {
    // Устанавливается нужный размер
    emit changeFontPointSize(editorConfig->get_monospace_font_size());

    // В выпадающем списке размеров выставляется установленный размер
    emit changeFontsizeOnDisplay( editorConfig->get_monospace_font_size() );
  }

  textArea->textCursor().endEditBlock();
}


// Форматирование в качестве кода
void TypefaceFormatter::onCodeClicked(void)
{
  // Если выделения нет
  if(!textArea->textCursor().hasSelection()) return;

  bool enableIndent;

  // Проверяется, выбран ли четко блок (блоки) текста
  // Блок - это текст между <p>...</p>
  if(!editor->isBlockSelect())
  {
    // Если выбран не четко блок (блоки)

    // Нужно определить, находится ли выделение в пределах одного блока
    // Если выделение вылазит за пределы одного блока, форматировать нельзя

    // Выясняются позиции начала и конца блока
    QTextBlock currentBlock = textArea->textCursor().block();
    int blockStart=currentBlock.position();
    int blockStop=currentBlock.position()+currentBlock.length();

    // Выясняются позиции начала и конца выделения
    int selectStart=textArea->textCursor().selectionStart();
    int selectStop =textArea->textCursor().selectionEnd();

    qDebug() << "Code format action, block " << blockStart << blockStop << " selection " << selectStart << selectStop;

    if(blockStart<=selectStart && blockStop>=selectStop)
      enableIndent=false; // Выбран кусок текста в пределах блока
    else
      return;
  }
  else
    enableIndent=true; // Выбран четко блок (блоки) текста, нужно делать отступ

  textArea->textCursor().beginEditBlock();

  // Устанавливается шрифт
  QFont font;
  font.fromString(editorConfig->get_code_font());
  emit changeFontFamily(font.family());

  // Новый установленный шрифт показывается в выпадающем списке шрифтов
  emit changeFontselectOnDisplay(font.family());

  // Если в настройках разрешена автоматическая установка нужного размера
  if(editorConfig->get_code_font_size_apply()==true)
  {
    // Устанавливается нужный размер
    emit changeFontPointSize(editorConfig->get_code_font_size());

    // В выпадающем списке размеров выставляется установленный размер
    emit changeFontsizeOnDisplay(editorConfig->get_code_font_size());
  }

  // Устанавливается цвет текста
  QColor codeColor(editorConfig->get_code_font_color ());
  textArea->setTextColor(codeColor);

  // Если в настройках разрешена автоматическая установка нужного отступа
  // И выбран четко блок (блоки)
  if(editorConfig->get_code_indent_size_apply()==true &&
     enableIndent==true)
  {
    // Создание форматирования
    QTextBlockFormat indentFormatting;
    indentFormatting.setLeftMargin(editorConfig->get_code_indent_size());
    indentFormatting.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

    // Форматирование для добавления отступа
    textArea->textCursor().mergeBlockFormat(indentFormatting);

    editor->updateIndentsliderToActualFormat();
  }

  textArea->textCursor().endEditBlock();
}


// Очистка форматирования, т.е. установка стандартного шрифта,
// размера и убирание утолщения, наклона, подчеркивания
void TypefaceFormatter::onClearClicked(void)
{
  int startCursorPos=textArea->textCursor().position();
  int stopCursorPos=textArea->textCursor().anchor();
  qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos;


  bool flag_cursor_on_empty_line=editor->isCursorOnEmptyLine();
  bool flag_cursor_on_space_line=editor->isCursorOnSpaceLine();

  // Очистка возможна только если что-то выделено
  // Или курсор стоит на пустой строке с одним символом перевода строки
  // Или курсор стоит на строке, в которой нет текста
  if(!(textArea->textCursor().hasSelection() ||
       flag_cursor_on_empty_line ||
       flag_cursor_on_space_line))
    return;

  textArea->textCursor().beginEditBlock();

  if(flag_cursor_on_space_line)
    (textArea->textCursor()).select(QTextCursor::LineUnderCursor);


  // Создается стандартный шрифт
  QFont font;
  font.fromString( editorConfig->get_default_font() ); // Стандартное начертание
  font.setPointSize( editorConfig->get_default_font_size() ); // Стандартный размер

  // Применяется стандартный шрифт
  textArea->setCurrentFont(font);

  // Новый установленный шрифт показывается в выпадающем списке шрифтов
  emit changeFontselectOnDisplay(font.family());

  // В выпадающем списке размеров выставляется установленный размер
  emit changeFontsizeOnDisplay(editorConfig->get_default_font_size());


  // Очищается формат символов
  QColor clearColor;
  QBrush clearBrush( clearColor );
  QTextCharFormat clearCharFormat;
  clearCharFormat.setForeground( clearBrush );
  textArea->setCurrentCharFormat( clearCharFormat );


  // Если выделен блок
  // или курсор на пустой линии
  // или курсор на линии на которой нет символов
  if(editor->isBlockSelect() ||
     flag_cursor_on_empty_line ||
     flag_cursor_on_space_line)
  {
    QTextBlockFormat format;

    // Убираются отступы
    format.setLeftMargin(0);   // Убирается левый отступ (который, возможно был установлен слайдером или кнопками изменения отступа)
    format.setRightMargin(0);
    format.setTopMargin(0);    // Убираются межстрочные интервалы, которые самопроизвольно появляются при вставке из других программ
    format.setBottomMargin(0);
    format.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

    // Применение форматирование
    textArea->textCursor().setBlockFormat(format);
  }


  // Если была работа со строкой, в которой нет символов,
  // курсор переносится на начало строки, чтобы не путать пользователя
  if(flag_cursor_on_space_line)
    textArea->moveCursor(QTextCursor::StartOfLine);


  // Очистка закомментирована, так как она заменена очисткой формата символов setCurrentCharFormat()
  // А так же эта очистка некорректно работает из-за особенностей вставки в Qt (первая строка получает отличный от остальных строк формат).
  // Думать дальше
  /*
 // Удаление какого-либо форматирования стилем
 QString htmlCode=textArea->textCursor().selection().toHtml();
 qDebug() << "Before remove style: " << htmlCode;

 // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
 // Поэтому напрямую регвыру указывается что кванторы должны быть ленивые
 QRegExp replace_expression("style=\".*\"");
 replace_expression.setMinimal(true);

 htmlCode.replace(replace_expression, "");
 qDebug() << "After remove style: " << htmlCode;

 QString currStyleSheet=textArea->document()->defaultStyleSheet();
 textArea->document()->setDefaultStyleSheet(" ");

 textArea->textCursor().removeSelectedText();
 textArea->textCursor().insertHtml(htmlCode);

 textArea->document()->setDefaultStyleSheet(currStyleSheet);

 textArea->textCursor().setPosition(startCursorPos);
 textArea->textCursor().setPosition(stopCursorPos, QTextCursor::KeepAnchor);
 // textArea->setTextCursor( textArea->textCursor() );

 qDebug() << "Cursor position: " << textArea->textCursor().position() << "Cursor anchor: " << textArea->textCursor().anchor();
 */

  textArea->textCursor().endEditBlock();

  // Вызывается метод, как будто переместился курсор с выделением, чтобы
  // обновились состояния подсветок кнопок форматирования
  editor->onSelectionChanged();

  editor->updateIndentsliderToActualFormat();
}


// Слот, срабатывающий при изменении шрифта в списке шрифтов
void TypefaceFormatter::onFontselectChanged(const QFont &font)
{
  // todo: Разобраться с этим флагом. Его нужно поместить в Editor или ToolBar, или вообще от него отказаться
  // if(editor->flagSetFontParametersEnabled==false)
  //   return;

  // textArea->setFontFamily(font.family()); // Устанавливается выбранный шрифт в области редактирования
  // editor->currentFontFamily=font.family();
  emit changeFontFamily(font.family());

  // Курсор после выбора возвращается в область редактирования
  textArea->setFocus();
}


// Слот, срабатывающий когда изменен размер шрифта через список размеров
void TypefaceFormatter::onFontsizeChanged(int n)
{
  // todo: Разобраться с этим флагом. Его нужно поместить в Editor или ToolBar, или вообще от него отказаться
  // if(editor->flagSetFontParametersEnabled==false)
  //   return;

  // int n=(editor->editorToolBar->fontSize->itemData(i)).toInt(); // Теперь значение берется из сигнала

  if(n<MINIMUM_ALLOWED_FONT_SIZE || n>MAXIMUM_ALLOWED_FONT_SIZE)
    return;

  // textArea->setFontPointSize(n);
  // editor->currentFontSize=n;
  emit changeFontPointSize(n);

  // Курсор после выбора возвращается в область редактирования
  textArea->setFocus();
}


// Слот, срабатыващий при нажатии на кнопку выбора цвета текста
void TypefaceFormatter::onFontcolorClicked()
{
  // Текущий цвет возле курсора
  QColor currentColor=textArea->textColor();

  // Диалог запроса цвета
  QColor selectedColor=QColorDialog::getColor(currentColor, editor);

  // Если цвет выбран, и он правильный
  if(selectedColor.isValid())
  {
    // Меняется цвет кнопки
    // editor->editorToolBar->fontColor->setPalette(QPalette(selectedColor));
    // editor->currentFontColor=selectedColor.name(); // Запоминается текущий цвет (подумать, доделать)
    emit changeFontcolor( selectedColor );
  }
}
