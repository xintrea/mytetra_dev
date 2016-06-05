#include <QTextBlock>
#include <QDebug>
#include <QColorDialog>
#include <QColor>
#include <QDomNode>

#include "TypefaceFormatter.h"

#include "main.h"
#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorToolBarAssistant.h"
#include "../EditorCursorPositionDetector.h"
#include "../../TraceLogger.h"


TypefaceFormatter::TypefaceFormatter()
{

}


// Форматирование Bold
void TypefaceFormatter::onBoldClicked(void)
{
  // TRACELOG

  smartFormat(Bold);
}


// Форматирование Italic
void TypefaceFormatter::onItalicClicked(void)
{
  // TRACELOG

  smartFormat(Italic);
}


// Форматирование подчеркивания
void TypefaceFormatter::onUnderlineClicked(void)
{
  // TRACELOG

  smartFormat(Underline);
}


void TypefaceFormatter::smartFormat(int formatType)
{
  // Если выделение есть
  if(textArea->textCursor().hasSelection())
  {
    if(formatType==Bold)
    {
      if(textArea->fontWeight() != QFont::Bold)
        textArea->setFontWeight(QFont::Bold); // Bold
      else
        textArea->setFontWeight(0); // Remove Bold
    }

    if(formatType==Italic)
    {
      if(!textArea->fontItalic())
        textArea->setFontItalic(true);
      else
        textArea->setFontItalic(false);
    }

    if(formatType==Underline)
    {
      if(!textArea->fontUnderline())
        textArea->setFontUnderline(true);
      else
        textArea->setFontUnderline(false);
    }

  }
  else
  {
    // Иначе надо выделить дополнительным курсором слово на котором стоит курсор
    QTextCursor cursor=textArea->textCursor();

    int cursorPosition=cursor.position();

    // Выделяется последний символ
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    QString prevCharacterAsString=cursor.selectedText();
    QChar prevCharacter;
    if(prevCharacterAsString.length()==1)
      prevCharacter=prevCharacterAsString[0];

    // Если последний символ не является пробелом
    if( !prevCharacter.isSpace() )
    {
      // Выделяется все слово
      cursor.setPosition(cursorPosition); // Сначала сбрасывается позиция на ту, которая была в начале до выделения последнего символа
      cursor.select(QTextCursor::WordUnderCursor); // Учесть, что WordUnderCursor - это выделение слова и пробельных символов после него
    }

    // К выделению применяется/отменяется форматирование
    QTextCharFormat format;

    if(formatType==Bold)
    {
      if(cursor.charFormat().fontWeight() != QFont::Bold)
        format.setFontWeight(QFont::Bold);
      else
        format.setFontWeight(0);
    }

    if(formatType==Italic)
    {
      if(!cursor.charFormat().fontItalic())
        format.setFontItalic(true);
      else
        format.setFontItalic(false);
    }

    if(formatType==Underline)
    {
      if(!cursor.charFormat().fontUnderline())
        format.setFontUnderline(true);
      else
        format.setFontUnderline(false);
    }

    cursor.mergeCharFormat(format);
  }

  emit updateOutlineButtonHiglight();
}


// Форматирование моноширинным шрифтом
void TypefaceFormatter::onMonospaceClicked(void)
{
  // TRACELOG

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
  // TRACELOG

  // Если выделения нет
  if(!textArea->textCursor().hasSelection())
    return;

  // Вначале происходит полная очистка выделенного текста
  onClearClicked();

  bool enableIndent;

  // Проверяется, выбран ли четко блок (блоки) текста
  // Блок - это текст между <p>...</p>
  if(!editor->cursorPositionDetector->isBlockSelect())
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
  // TRACELOG

  int startCursorPos=textArea->textCursor().position(); // Начало выделения (всегда меньше чем конец выделения независимо от того, справа-налево или слева-направо был выделен фрагмент)
  int stopCursorPos=textArea->textCursor().anchor(); // Конец выделения
  // qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos << " User cursor pos: " << userCursorPos;

  // Если выделение было сзаду-наперед, надо поменять начальную и конечную позицию местами
  if(startCursorPos>stopCursorPos)
  {
    int tempCursorPos=startCursorPos;
    startCursorPos=stopCursorPos;
    stopCursorPos=tempCursorPos;
  }
  // qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos;

  bool flag_cursor_on_empty_line=editor->cursorPositionDetector->isCursorOnEmptyLine();
  bool flag_cursor_on_space_line=editor->cursorPositionDetector->isCursorOnSpaceLine();

  // Очистка возможна только если что-то выделено
  // Или курсор стоит на пустой строке с одним символом перевода строки
  // Или курсор стоит на строке, в которой нет текста (одни пробелы)
  if(!(textArea->textCursor().hasSelection() ||
       flag_cursor_on_empty_line ||
       flag_cursor_on_space_line))
    return;

  textArea->textCursor().beginEditBlock();

  // Если что-то было выделено - выделение уже есть
  // Если курсор стоит на пустой строке с одним символом перевода строки - ничего выделять не нужно
  // Или курсор стоит на строке, в которой нет текста (одни пробелы) - нужно выделить эту строку
  if(flag_cursor_on_space_line)
    (textArea->textCursor()).select(QTextCursor::LineUnderCursor);


  // Создается стандартный шрифт
  QFont font;
  font.fromString( editorConfig->get_default_font() ); // Стандартное начертание взятое из конфига
  font.setPointSize( editorConfig->get_default_font_size() ); // Стандартный размер взятый из конфига

  // К выделенному тексту применяется стандартный шрифт
  textArea->setCurrentFont(font);

  // Новый установленный шрифт показывается в выпадающем списке шрифтов
  emit changeFontselectOnDisplay(font.family());

  // В выпадающем списке размеров выставляется установленный размер
  emit changeFontsizeOnDisplay(editorConfig->get_default_font_size());


  // *****************************
  // Установка начертания символов
  // *****************************

  // Очищается формат символов
  /*
  QColor clearColor;
  QBrush clearBrush( clearColor );
  QTextCharFormat clearCharFormat;
  clearCharFormat.setForeground( clearBrush );
  textArea->textCursor().mergeCharFormat(clearCharFormat);
  */

  // Получение HTML-кода очищенного фрагмента
  QString htmlCode=clearTypeFace( replaceSpacesOnlyTags( textArea->textCursor().selection().toHtml() ) );

  // Замена в HTML-коде пробелов на неразывные пробелы, иначе все повторяющиеся пробелы будут удален Qt-движком
  htmlCode=replaceSpaces(htmlCode);
  // qDebug() << "After replace spaces" << htmlCode;

  // Удаление выделенного фрагмента
  textArea->textCursor().removeSelectedText();

  // С помощью дополнительного курсора выясняется последняя позиция в тексте, в котором удален выделенный фрагмент
  QTextCursor cursor=textArea->textCursor();
  cursor.movePosition(QTextCursor::End);
  int afterRemoveSelectionLen=cursor.position();
  // qDebug() << "After remove selection length: " << afterRemoveSelectionLen;

  // Вставка очищенного фрагмента
  textArea->textCursor().insertHtml(htmlCode);
  qDebug() << "After insert HTML: "<< textArea->toHtml();

  // С помощью дополнительного курсора выясняется последняя позиция в тексте, в котором вставлен очищенный фрагмент
  cursor.movePosition(QTextCursor::End);
  int afterClearLen=cursor.position();
  // qDebug() << "After clear length: " << afterClearLen;

  // Вычисляется последняя позиция выделения очищенного текста
  int calculateEndCursorPos=startCursorPos + (afterClearLen - afterRemoveSelectionLen);
  // qDebug() << "Calculate end cursor pos: " << calculateEndCursorPos;


  // Замена ReplacementCharacter-символа на пробелы
  // Это необходимо, так как Qt самопроизвольно удаляет ведущие пробелы при вызове textArea->textCursor().insertHtml(htmlCode);
  QTextCursor replacementCursor=textArea->textCursor();
  for(int pos=startCursorPos; pos<calculateEndCursorPos; pos++)
  {
    // Выделяется один символ
    replacementCursor.setPosition(pos, QTextCursor::MoveAnchor);
    replacementCursor.setPosition(pos+1, QTextCursor::KeepAnchor);

    // Если выделение символа прошло успешно
    if(replacementCursor.selectedText().length()>0)
    {
      QChar currentChar=replacementCursor.selectedText().at(0);
      // qDebug() << "Pos: " << pos << " Char: " << currentChar << " Char code: " << currentChar.unicode();

      if(currentChar==QChar::ReplacementCharacter)
      {
        replacementCursor.insertText(" ");
        // qDebug() << "Replace RC to space in position: " << pos;
      }
    }
  }


  // Замена самопроизвольно вставляемых Qt концевых пробелов (последовательность пробел+QChar::ParagraphSeparator на QChar::ParagraphSeparator)
  // Эти самопроизвольно встваляемые пробелы появляются при вызове textArea->textCursor().insertHtml(htmlCode);
  bool flagPreviousSpace=false;
  for(int pos=startCursorPos; pos<=calculateEndCursorPos; pos++)
  {
    // Выделяется один символ
    replacementCursor.setPosition(pos, QTextCursor::MoveAnchor);
    replacementCursor.setPosition(pos+1, QTextCursor::KeepAnchor);

    // Если выделение символа прошло успешно
    if(replacementCursor.selectedText().length()>0)
    {
      QChar currentChar=replacementCursor.selectedText().at(0);
      qDebug() << "Pos: " << pos << " Char: " << currentChar << " Char code: " << currentChar.unicode();

      if(currentChar==QChar::ParagraphSeparator && flagPreviousSpace)
      {
        qDebug() << "Find space + paragraph separator";

        // Снимается выделение, чтобы правильно сработало удаление символа
        replacementCursor.setPosition(pos, QTextCursor::MoveAnchor);

        // Удаляется предыдущий символ
        replacementCursor.deletePreviousChar();

        // Номер текущего символа уменьшается на единицу, чтобы не пропустить следующую строку из пробела+QChar::ParagraphSeparator
        --pos;

        // Так как символ удален, длина всего вставляемого фрагмента уменьшается на единицу
        --calculateEndCursorPos;

        // Снимается флаг, что предыдущий символ был пробелом
        flagPreviousSpace=false;
      }

      if(currentChar==QChar::Space)
        flagPreviousSpace=true;
    }
  }


  // ********************************
  // Выделение вставленного фрагмента
  // Если его не сделать, то первая строка получит дополнительные вертикальные отступы
  // Это особенность Qt
  // ********************************
  cursor.setPosition(startCursorPos, QTextCursor::MoveAnchor);
  cursor.setPosition(calculateEndCursorPos, QTextCursor::KeepAnchor);
  textArea->setTextCursor(cursor);
  // qDebug() << "Select text after insert HTML: " << textArea->textCursor().selection().toHtml();


  // *******************************
  // Установка форматирования абзаца
  // *******************************

  // Если выделен блок
  // или курсор на пустой линии
  // или курсор на линии на которой нет символов
  if(editor->cursorPositionDetector->isBlockSelect() ||
     flag_cursor_on_empty_line ||
     flag_cursor_on_space_line)
  {
    qDebug() << "Set default text format";
    QTextBlockFormat format;

    // Убираются отступы
    format.setLeftMargin(0);   // Убирается левый отступ (который, возможно был установлен слайдером или кнопками изменения отступа)
    format.setRightMargin(0);
    format.setTopMargin(0);    // Убираются межстрочные интервалы, которые самопроизвольно появляются при вставке из других программ
    format.setBottomMargin(0);
    format.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

    // Применение форматирование
    textArea->textCursor().setBlockFormat(format);

    // qDebug() << "Select text after apply format: " << textArea->textCursor().selection().toHtml();
  }


  // Если была работа со строкой, в которой нет символов,
  // курсор переносится на начало строки, чтобы не путать пользователя
  if(flag_cursor_on_space_line)
    textArea->moveCursor(QTextCursor::StartOfLine);

  textArea->textCursor().endEditBlock();

  // Вызывается метод, как будто переместился курсор с выделением, чтобы
  // обновились состояния подсветок кнопок форматирования
  editor->onSelectionChanged();

  editor->updateIndentsliderToActualFormat();
}


// Замена пробелов в тегах <span атрибуты>...</span>, содержимое которых состоит из одних только пробелов
QString TypefaceFormatter::replaceSpacesOnlyTags(QString htmlCode)
{

  QRegExp replaceSpaceTagsEx("<span[^>]*>\\s*</span>");
  replaceSpaceTagsEx.setMinimal(true);

  // Поиск тегов <span> с одними пробелами внутри, с запоминанием их
  QStringList list;
  int lastPos = 0;
  while( ( lastPos = replaceSpaceTagsEx.indexIn( htmlCode, lastPos ) ) != -1 )
  {
    lastPos += replaceSpaceTagsEx.matchedLength();

    list << replaceSpaceTagsEx.cap(0);
    qDebug() << "Find space only sttring: " << list.last();
  }


  // Замена найденных тегов на теги с "&#65533;" вместо пробелов
  for( int n = 0; n < list.size(); n++)
  {
    QString line=list.at(n);

    // Здесь однозначно известно, что строка имеет вид "<span атрибуты>пробелы</span>"
    int spaceFrom=line.indexOf(">")+1;
    int spaceTo=line.indexOf("<", spaceFrom)-1;
    int spaceLen=spaceTo-spaceFrom;
    qDebug() << "Space length:" << spaceLen;

    QString spaceLine;
    for(int i=0; i<spaceLen; ++i)
      spaceLine+="&#65533;";

    QString replaceLineLeft=line.mid(0, spaceFrom-1);
    QString replaceLineRight=line.mid(spaceTo+1);
    QString replaceLine=replaceLineLeft+spaceLine+replaceLineRight;

    qDebug() << "Replace space line from: " << list.at(n);
    qDebug() << "Replace space line to  : " << replaceLine;
    htmlCode.replace( list.at(n), replaceLine);
  }

  return htmlCode;

/*
  htmlCode.replace(replaceSpaceTagsEx, "");

  QRegExp rx("<pre[^>]*>([^<]*)</pre>");
  rx.setMinimal(true);

  QStringList list;
  int pos = 0;
  while((pos = rx.indexIn( data, pos)) != -1)
  {
    list << rx.cap(1);
    pos += rx.matchedLength();
  }

  for( int n = 0; n < list.size(); n++)
  {
    QString szTmp = list.at(n);
    szTmp.replace( "\n", "<br/>");

    data.replace( list.at(n), szTmp);
  }
*/
}


// Очистка начертания символов
QString TypefaceFormatter::clearTypeFace(QString htmlCode)
{
  // Удаление какого-либо форматирования стилем
  qDebug() << "Before clearTypeFace apply: " << htmlCode;

  // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
  // Поэтому напрямую регвыру указывается что кванторы должны быть ленивые
  QRegExp removeStyleEx("style=\".*\"");
  removeStyleEx.setMinimal(true);
  htmlCode.replace(removeStyleEx, "style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\"");
  qDebug() << "After remove style: " << htmlCode;

  QRegExp startFragmentEx("<!--StartFragment-->");
  startFragmentEx.setMinimal(true);
  htmlCode.replace(startFragmentEx, "");

  QRegExp endFragmentEx("<!--EndFragment-->");
  endFragmentEx.setMinimal(true);
  htmlCode.replace(endFragmentEx, "");

  // Замена конструкции <p ...><br /></p>, которая вставляется автоматически Qt в конец HTML текста,
  // и тем самым создает лишнюю пустую строку
  // Жадная регулярка не всегда корректно захватывает строку (почему-то работает как ленивая), приходится разбивать на подстроки
  // Кроме того, в Qt нет возможности переключать режим multiline/не-multiline в регулярных выражениях
  QStringList list=htmlCode.split(QRegularExpression("\\n"));
  QString tempHtmlCode;
  for(int lineNum=0; lineNum<list.count(); ++lineNum)
  {
    qDebug() << "L" << lineNum << " " << list[lineNum];

    QRegExp replacePBrP("<p.*><br.?\\/><\\/p.*>");
    replacePBrP.setMinimal(true);
    list[lineNum].replace(replacePBrP, "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; \"></p>");
    qDebug() << "L" << lineNum << " " << list[lineNum];

    tempHtmlCode+=list[lineNum]+"\n";
  }
  htmlCode=tempHtmlCode;
  // qDebug() << "After replace p br p: " << htmlCode;

  QRegExp replaceOpenHeaderEx("<[hH]\\d.*>");
  replaceOpenHeaderEx.setMinimal(true);
  htmlCode.replace(replaceOpenHeaderEx, "<p>");

  QRegExp replaceCloseHeaderEx("</[hH]\\d.*>");
  replaceCloseHeaderEx.setMinimal(true);
  htmlCode.replace(replaceCloseHeaderEx, "</p>");

  QRegExp removeStartTagsEx(".*<body>");
  removeStartTagsEx.setMinimal(false);
  htmlCode.replace(removeStartTagsEx, "");

  QRegExp removeEndTagsEx("</body>.*");
  removeEndTagsEx.setMinimal(false);
  htmlCode.replace(removeEndTagsEx, "");

  return htmlCode;
}


// Замена в HTML-коде пробелов на неразывные пробелы, иначе все повторяющиеся пробелы будут удален Qt-движком
QString TypefaceFormatter::replaceSpaces(QString htmlCode)
{
  QDomDocument doc;
  bool isDocParse=doc.setContent("<root>"+htmlCode+"</root>");

  if(!isDocParse)
  {
    // Вывод сообщения: Слишком сложно еформатирование текста. Не могу разобрать выделенный текст. Возможно некоторое нарушение верстки
    showMessageBox(tr("Too hard text formatting. Can't normal parse selected text. May be any format regression"));
    return htmlCode;
  }

  // Рекурсивная правка текстовых узлов, начиная с корневого элемента
  recurseReplaceSpaces(doc.documentElement());

  // Особенность Qt. Преобразование DOM-XML портит символ амперсанда. Поэтому он восстанавливается
  // return doc.toString().replace("&amp;nbsp;", "&nbsp;");
  // return doc.toString().replace("&amp;#32;", "&#32;");
  // return doc.toString().replace("&amp;#65532;", "&#65532;");
  return doc.toString(0).replace("&amp;#65533;", "&#65533;");
}


void TypefaceFormatter::recurseReplaceSpaces(const QDomNode &node)
{
  QDomNode domNode = node.firstChild();
  QDomText domText;

  // Если текущий элемент существует
  while(!(domNode.isNull()))
  {
    // Если узел - это текст
    if(domNode.isText())
    {
      QDomText domText = domNode.toText();
      if(!domText.isNull())
      {
        QString text=domText.data();

        // Чтобы не смыкаликись повторяющиеся пробелы, они временно заменяются на RC
        text.replace(" ", "&#65533;"); // REPLACEMENT CHARACTER

        // В узле устанавливается новая строка
        domNode.setNodeValue(text);
      }
    }

    recurseReplaceSpaces(domNode);
    domNode = domNode.nextSibling();
  }
}


void TypefaceFormatter::onTextOnlyClicked()
{
  int startCursorPos=textArea->textCursor().position();
  int stopCursorPos=textArea->textCursor().anchor();

  qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos;
  bool flag_cursor_on_empty_line=editor->cursorPositionDetector->isCursorOnEmptyLine();
  bool flag_cursor_on_space_line=editor->cursorPositionDetector->isCursorOnSpaceLine();

  // Очистка возможна только если что-то выделено
  // Или курсор стоит на пустой строке с одним символом перевода строки
  // Или курсор стоит на строке, в которой нет текста
  if(!(textArea->textCursor().hasSelection() ||
       flag_cursor_on_empty_line ||
       flag_cursor_on_space_line))
    return;

  textArea->textCursor().beginEditBlock();

  // Если курсор на строке, у которой нет текста
  if(flag_cursor_on_space_line)
    (textArea->textCursor()).select(QTextCursor::LineUnderCursor);

  // Создается стандартный шрифт
  QFont font;
  font.fromString( editorConfig->get_default_font() ); // Стандартное начертание взятое из конфига
  font.setPointSize( editorConfig->get_default_font_size() ); // Стандартный размер взятый из конфига

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
  textArea->textCursor().mergeCharFormat(clearCharFormat);

  // Если выделен блок
  // или курсор на пустой линии
  // или курсор на линии на которой нет символов
  if(editor->cursorPositionDetector->isBlockSelect() ||
     flag_cursor_on_empty_line ||
     flag_cursor_on_space_line)
  {
    QTextBlockFormat format;

    // Убираются отступы
    format.setLeftMargin(0); // Убирается левый отступ (который, возможно был установлен слайдером или кнопками изменения отступа)
    format.setRightMargin(0);
    format.setTopMargin(0); // Убираются межстрочные интервалы, которые самопроизвольно появляются при вставке из других программ
    format.setBottomMargin(0);
    format.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

    // Применение форматирование
    textArea->textCursor().setBlockFormat(format);
  }

  // Если была работа со строкой, в которой нет символов,
  // курсор переносится на начало строки, чтобы не путать пользователя
  if(flag_cursor_on_space_line)
    textArea->moveCursor(QTextCursor::StartOfLine);

  textArea->textCursor().endEditBlock();

  // Вызывается метод, как будто переместился курсор с выделением, чтобы
  // обновились состояния подсветок кнопок форматирования
  editor->onSelectionChanged();
  editor->updateIndentsliderToActualFormat();
}


void TypefaceFormatter::onFixBreakSymbolClicked()
{
  textArea->textCursor().beginEditBlock();

  int startCursorPos=textArea->textCursor().position();
  int stopCursorPos=textArea->textCursor().anchor();

  // Получение исходного кода выделенного фрагмента
  QString htmlCode=textArea->textCursor().selection().toHtml();

  qDebug() << "Before replace break in onFixBreakSymbolClicked: " << htmlCode;

  // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
  // Поэтому напрямую регвыру указывается что кванторы должны быть ленивые
  QRegExp replace_expression("<br\\s?/>");
  replace_expression.setMinimal(true);

  htmlCode.replace(replace_expression, "</p><p>");

  qDebug() << "After remove style: " << htmlCode;

  // Старый код убирается
  textArea->textCursor().removeSelectedText();

  // Новый код вставляется
  textArea->textCursor().insertHtml(htmlCode);

  textArea->textCursor().setPosition(startCursorPos);
  textArea->textCursor().setPosition(stopCursorPos, QTextCursor::KeepAnchor);
  // textArea->setTextCursor( textArea->textCursor() );

  textArea->textCursor().endEditBlock();
}


// Слот, срабатывающий при изменении шрифта в списке шрифтов
void TypefaceFormatter::onFontselectChanged(const QFont &font)
{
  // TRACELOG

  // todo: Разобраться с этим флагом. Его нужно поместить в Editor или ToolBar, или вообще от него отказаться
  // if(editor->flagSetFontParametersEnabled==false)
  //   return;
  if( !(editor->editorToolBarAssistant->getFlagSetFontParametersEnabled()) )
    return;

  // textArea->setFontFamily(font.family()); // Устанавливается выбранный шрифт в области редактирования
  // editor->currentFontFamily=font.family();
  emit changeFontFamily(font.family());

  // Курсор после выбора возвращается в область редактирования
  textArea->setFocus();
}


// Слот, срабатывающий когда изменен размер шрифта через список размеров
void TypefaceFormatter::onFontsizeChanged(int n)
{
  // TRACELOG

  // todo: Разобраться с этим флагом. Его нужно поместить в Editor или ToolBar, или вообще от него отказаться
  // if(editor->flagSetFontParametersEnabled==false)
  //   return;
  if( !(editor->editorToolBarAssistant->getFlagSetFontParametersEnabled()) )
    return;

  // Номер выбранного размера преобразуется в размер
  n=editor->editorToolBarAssistant->getFontSizeByNum(n);

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
  // TRACELOG

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
