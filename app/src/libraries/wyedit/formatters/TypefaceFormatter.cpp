#include <QTextBlock>
#include <QDebug>
#include <QColorDialog>
#include <QColor>
#include <QDomNode>
#include <QTextTable>

#include "TypefaceFormatter.h"

#include "main.h"
#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorToolBarAssistant.h"
#include "../EditorCursorPositionDetector.h"
#include "../../TraceLogger.h"
#include "libraries/helpers/MessageHelper.h"


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


// Форматирование Strike
void TypefaceFormatter::onStrikeOutClicked(void)
{
    // TRACELOG

    smartFormat(StrikeOut);
}


// Форматирование SuperScript
void TypefaceFormatter::onSuperScriptClicked(void)
{
    // TRACELOG

    smartFormat(SuperScript);
}


// Форматирование SubScript
void TypefaceFormatter::onSubScriptClicked(void)
{
    // TRACELOG

    smartFormat(SubScript);
}


void TypefaceFormatter::smartFormat(int formatType)
{
    // Если выделение есть
    if(textArea->textCursor().hasSelection())
    {
        // Переназначение позиций выделения, иначе снятие форматирования не срабатывает
        // в случае выделения справа-налево (снизу-вверх)
        QTextCursor cursor = textArea->textCursor();
        const int anchor = cursor.anchor();
        const int position = cursor.position();
        if (anchor > position)
        {
            cursor.setPosition(position, QTextCursor::MoveAnchor);
            cursor.setPosition(anchor, QTextCursor::KeepAnchor);
            textArea->setTextCursor(cursor);
        }

        if(formatType==Bold)
        {
            if(textArea->fontWeight() != QFont::Bold)
                textArea->setFontWeight(QFont::Bold); // Bold
            else
                textArea->setFontWeight(QFont::Normal); // Remove Bold
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

        if(formatType==StrikeOut)
        {
            QTextCharFormat format;
            if(!textArea->textCursor().charFormat().fontStrikeOut())
                format.setFontStrikeOut(true);
            else
                format.setFontStrikeOut(false);
            textArea->textCursor().mergeCharFormat(format);
        }

        if(formatType==SuperScript)
        {
            QTextCharFormat format;
            if(textArea->textCursor().charFormat().verticalAlignment() != QTextCharFormat::AlignSuperScript)
                format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
            else
                format.setVerticalAlignment(QTextCharFormat::AlignNormal);
            textArea->textCursor().mergeCharFormat(format);
        }

        if(formatType==SubScript)
        {
            QTextCharFormat format;
            if(textArea->textCursor().charFormat().verticalAlignment() != QTextCharFormat::AlignSubScript)
                format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
            else
                format.setVerticalAlignment(QTextCharFormat::AlignNormal);
            textArea->textCursor().mergeCharFormat(format);
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
                format.setFontWeight(QFont::Normal); // Remove Bold
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

        if(formatType==StrikeOut)
        {
            if(!cursor.charFormat().fontStrikeOut())
                format.setFontStrikeOut(true);
            else
                format.setFontStrikeOut(false);
        }

        if(formatType==SuperScript)
        {
            if(cursor.charFormat().verticalAlignment() != QTextCharFormat::AlignSuperScript)
                format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
            else
                format.setVerticalAlignment(QTextCharFormat::AlignNormal);
            textArea->mergeCurrentCharFormat(format);
        }

        if(formatType==SubScript)
        {
            if(cursor.charFormat().verticalAlignment() != QTextCharFormat::AlignSubScript)
                format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
            else
                format.setVerticalAlignment(QTextCharFormat::AlignNormal);
            textArea->mergeCurrentCharFormat(format);
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

    textArea->textCursor().beginEditBlock();
    // Далее при любом выходе из метода нужно вызывать textArea->textCursor().endEditBlock()

    // Обработка мягкого переноса в выделенном тексте
    // Учитываются мягкие переносы до выделенного текста (1-й символ до выделения) и в выделенных абзацах
    workingSoftCarryInSelection();
    
    bool enableIndent;

    // Проверяется, выбран ли четко блок (блоки) текста,
    // это нужно знать для того, чтобы форматировать/не форматировать отступы
    // Блок - это текст между <p>...</p>
    if(!editor->cursorPositionDetector->isBlockSelect())
    {
        // Если выбран не четко блок (блоки)

        // Нужно определить, находится ли выделение в пределах одного блока
        // Если выделение вылазит за пределы одного блока, форматировать отступы нельзя

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
        else{
            qDebug() << "The selection is outside one block";
            //Хорошо бы писать в статусную строку сообщение, почему форматирование не выполнено
            textArea->textCursor().endEditBlock();
            return;
        }
    }
    else
        enableIndent=true; // Выбран четко блок (блоки) текста, нужно делать отступ


    // Вначале происходит преобразование фрагмента в чистый текст (onClearClicked() не подходит, так как съедается табуляция)
    onTextOnlyClicked();

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
// Картинки и таблицы не удаляются
void TypefaceFormatter::onClearClicked(void)
{
    // TRACELOG

    int startCursorPos=textArea->textCursor().anchor(); // Начало выделения
    int stopCursorPos=textArea->textCursor().position(); // Конец выделения
    qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos;

    // Если выделение было сзаду-наперед, надо поменять начальную и конечную позицию местами
    bool isSelectionReverse=false;
    if(startCursorPos>stopCursorPos)
    {
        int tempCursorPos=startCursorPos;
        startCursorPos=stopCursorPos;
        stopCursorPos=tempCursorPos;

        isSelectionReverse=true;
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


    // Запоминается положение прокрутки окна редактирования.
    // Это нужно, чтобы вернуться к такому положению после всех действий.
    // Иначе прокрутка перепрыгнет наверх документа
    int scrollBarPosition=editor->getScrollBarPosition();


    // Начало изменения текста
    textArea->textCursor().beginEditBlock();


    // Если выбран НЕ четко блок, и есть форматирование списка
    if(editor->cursorPositionDetector->isBlockSelect()==false &&
            textArea->textCursor().currentList()!=nullptr)
    {
        // Выделение части строки в списке нельзя очищать обычным способом, так как появится перенос строки
        clearSimple(); // Запускается упрощенная очистка
    }
    else
    {
        // Если курсор стоит на строке, в которой нет текста (одни пробелы) - нужно выделить эту строку
        if(flag_cursor_on_space_line)
            (textArea->textCursor()).select(QTextCursor::LineUnderCursor);

        // К выделению применяется стандартный шрифт
        applyStandartFontForSelection();

        // Очищается цвет заднего фона - теперь не нужно, так как очистка происходит путем полного удаления атрибутов стиля
        // clearBackgroundColorForSelection();

        // *****************************
        // Установка начертания символов
        // *****************************

        // Получение HTML-кода очищенного фрагмента
        QString htmlCode=clearTypeFace( replaceSpacesOnlyTags( textArea->textCursor().selection().toHtml() ) );

        // Замена в HTML-коде пробелов на неразывные пробелы, иначе все повторяющиеся пробелы будут удален Qt-движком
        htmlCode=replaceSpaces(htmlCode);
        // qDebug() << "After replace spaces" << htmlSimplyfier( htmlCode );

        // Замена в HTML-коде табуляции на спецкоды, иначе символы табуляции будут удален Qt-движком
        // Проблема осталась: спецкоды тоже удаляются в момент insertHtml()
        // htmlCode=replaceTabs(htmlCode);
        // qDebug() << "After replace tabs" << htmlSimplyfier( htmlCode );

        // Удаление выделенного фрагмента
        textArea->textCursor().removeSelectedText();
        // qDebug() << "After remove selected text: "<< htmlSimplyfier( textArea->toHtml() );

        // С помощью дополнительного курсора выясняется последняя позиция в тексте, в котором удален выделенный фрагмент
        QTextCursor cursor=textArea->textCursor();
        cursor.movePosition(QTextCursor::End);
        int afterRemoveSelectionLen=cursor.position();
        // qDebug() << "After remove selection length: " << afterRemoveSelectionLen;

        // Вставка очищенного фрагмента
        textArea->textCursor().insertHtml(htmlCode);
        // qDebug() << "After insert HTML: "<< htmlSimplyfier( textArea->toHtml() );

        // С помощью дополнительного курсора выясняется последняя позиция в тексте, в котором вставлен очищенный фрагмент
        cursor.movePosition(QTextCursor::End);
        int afterClearLen=cursor.position();
        // qDebug() << "After clear length: " << afterClearLen;

        // Вычисляется последняя позиция выделения очищенного текста
        int calculateEndCursorPos=startCursorPos + (afterClearLen - afterRemoveSelectionLen);
        // qDebug() << "Calculate end cursor pos: " << calculateEndCursorPos;

        // Удаление самопроизвольно вставляемых Qt концевых пробелов в каждой текстовой ноде (компенсация поведения insertHtml())
        calculateEndCursorPos=removeSpaces(startCursorPos, calculateEndCursorPos);

        // Замена заранее внесенных символов ReplacementCharacter на пробелы (компенсация поведения insertHtml())
        replaceReplacementCharacterToSpaceInSelectedText(startCursorPos, calculateEndCursorPos);

        // Замена самопроизвольно вставляемых Qt концевых пробелов (последовательность пробел+QChar::ParagraphSeparator на QChar::ParagraphSeparator)
        // calculateEndCursorPos=replaceSpaceAndParagraphSeparatorToParagraphSeparator(startCursorPos, calculateEndCursorPos);

        // ********************************
        // Выделение вставленного фрагмента
        // Если его не сделать, то первая строка получит дополнительные вертикальные отступы. Это особенность Qt
        // ********************************
        if(!isSelectionReverse)
        {
            cursor.setPosition(startCursorPos, QTextCursor::MoveAnchor);
            cursor.setPosition(calculateEndCursorPos, QTextCursor::KeepAnchor);
        }
        else
        {
            cursor.setPosition(calculateEndCursorPos, QTextCursor::MoveAnchor);
            cursor.setPosition(startCursorPos, QTextCursor::KeepAnchor);
        }
        textArea->setTextCursor(cursor);
        // qDebug() << "Select text after insert HTML: " << htmlSimplyfier( textArea->textCursor().selection().toHtml() );


        // Установка форматирования абзаца
        // Если выделен блок
        // или курсор на пустой линии
        // или курсор на линии на которой нет символов
        if(editor->cursorPositionDetector->isBlockSelect() ||
                flag_cursor_on_empty_line ||
                flag_cursor_on_space_line)
            applyPureBlockFormatForSelection();

        // Если была работа со строкой, в которой нет символов,
        // курсор переносится на начало строки, чтобы не путать пользователя
        if(flag_cursor_on_space_line)
            textArea->moveCursor(QTextCursor::StartOfLine);
    }

    // Завершение изменения текста
    textArea->textCursor().endEditBlock();

    editor->setScrollBarPosition(scrollBarPosition);

    // Вызывается метод, как будто переместился курсор с выделением, чтобы
    // обновились состояния подсветок кнопок форматирования
    editor->onSelectionChanged();

    editor->updateIndentsliderToActualFormat();

    emit updateOutlineButtonHiglight();
}


void TypefaceFormatter::applyStandartFontForSelection()
{
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
}


void TypefaceFormatter::applyPureBlockFormatForSelection()
{
    // qDebug() << "Set default text format";

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


void TypefaceFormatter::removeAnchorDataForSelection()
{
    QTextCharFormat format;
    format.setAnchor(false);
    format.setAnchorHref("");

    // Применение форматирование
    textArea->textCursor().mergeCharFormat(format);
}


void TypefaceFormatter::clearColorForSelection()
{
    QTextCharFormat format;
    format.setForeground( qApp->palette("EditorTextArea").text() ); // QColor color=qApp->palette.text().color();

    // Применение форматирование
    textArea->textCursor().mergeCharFormat(format);
}


// В настоящий момент этот метод не нужен, так как очистка происходит в стилях (через полное удаление атрибуторв стиля)
void TypefaceFormatter::clearBackgroundColorForSelection()
{
    // Сбрасывается цвет заднего фона
    QColor clearColor;
    QBrush clearBrush( clearColor );
    QTextCharFormat clearCharFormat;
    clearCharFormat.setForeground( clearBrush );
    textArea->textCursor().mergeCharFormat(clearCharFormat);
}


// Замена пробелов внутри тегов <span>, содержимое которых состоит из одних только пробелов
QString TypefaceFormatter::replaceSpacesOnlyTags(QString htmlCode)
{
    qDebug() << "In TypefaceFormatter::replaceSpacesOnlyTags(): " << htmlSimplyfier( htmlCode );

    QRegExp replaceSpaceTagsEx("<span[^>]*>\\s*</span>");
    replaceSpaceTagsEx.setMinimal(true);

    // Поиск тегов <span> с одними пробелами внутри, с запоминанием их
    QStringList list;
    int lastPos = 0;
    while( ( lastPos = replaceSpaceTagsEx.indexIn( htmlCode, lastPos ) ) != -1 )
    {
        lastPos += replaceSpaceTagsEx.matchedLength();

        list << replaceSpaceTagsEx.cap(0);
        // qDebug() << "Find space only sttring: " << list.last();
    }


    // Замена найденных тегов на теги с RC-символом вместо пробелов
    for( int n = 0; n < list.size(); n++)
    {
        QString line=list.at(n);

        // Здесь однозначно известно, что строка имеет вид "<span атрибуты>пробелы</span>"
        int spaceFrom=line.indexOf(">")+1;
        int spaceTo=line.indexOf("<", spaceFrom)-1;
        int spaceLen=spaceTo-spaceFrom+1;
        // qDebug() << "Space length:" << spaceLen;

        QString spaceLine;
        for(int i=0; i<spaceLen; ++i)
            spaceLine.append(QChar::ReplacementCharacter); // spaceLine+="&#65533;" - это была вставка как HTML-код, она работала, но лучше сразу вставлять Unicode символ

        QString replaceLineLeft=line.mid(0, spaceFrom);
        QString replaceLineRight=line.mid(spaceTo+1);
        QString replaceLine=replaceLineLeft+spaceLine+replaceLineRight;

        // qDebug() << "Replace space line from: " << list.at(n);
        // qDebug() << "Replace space line to  : " << replaceLine;
        htmlCode.replace( list.at(n), replaceLine);
    }

    return htmlCode;
}


// Очистка начертания символов
QString TypefaceFormatter::clearTypeFace(QString htmlCode)
{
    // Удаление какого-либо форматирования стилем
    // qDebug() << "Before clearTypeFace apply: " << htmlCode;

    // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
    // Поэтому напрямую регвыру указывается что кванторы должны быть ленивые
    QRegExp removeStyleEx("style=\".*\"");
    removeStyleEx.setMinimal(true);
    htmlCode.replace(removeStyleEx, "style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\"");
    // qDebug() << "After remove style: " << htmlCode;

    // Тег <pre> превращается в <p>, так как последующая вставка <pre>-тегов через insertHtml() автоматически добавляет системный моноширинный стиль
    htmlCode.replace("<pre ", "<p ");
    htmlCode.replace("</pre>", "</p>");

    htmlCode.replace("<!--StartFragment-->", "");
    // qDebug() << "After replace StartFragment: " << htmlSimplyfier( htmlCode );

    // Просто так удалять <!--EndFragment--> нельзя.
    // Нужно вначале удалить p-тег вида: <p style="..."><br /><!--EndFragment--></p>
    // Так как такая конструкция свидетельствует, что в конце выделения курсор находится на строке,
    // на которой ничего не выделено (т. е. находится в самой первой позиции строки).
    // Таким образом данный p-тег будет лишним, и будет преобразовываться в отдельную пустую строку,
    // если его сразу не убрать
    // Код лишний, но оставляю на будущее
    /*
    QRegExp removeEmptyFinishPTag("<p style=\"[^\"]*\"><br.?\\/><!--EndFragment--><\\/p>");
    removeEmptyFinishPTag.setMinimal(true);
    htmlCode.replace(removeEmptyFinishPTag, "<!--EndFragment-->");
    qDebug() << "After replace special tag EndFragment: " << htmlSimplyfier( htmlCode );
    */

    // Теперь однозначно удаляется признак конца фрагмента, на тот случай, если предыдущая замена не сработала
    htmlCode.replace("<!--EndFragment-->", "");
    // qDebug() << "After replace EndFragment: " << htmlCode;


    // Замена конструкции <p ...><br /></p>, которая вставляется автоматически Qt в конец HTML текста,
    // и тем самым создает лишнюю пустую строку
    // Жадная регулярка не всегда корректно захватывает строку (почему-то работает как ленивая), приходится разбивать на подстроки
    // Кроме того, в Qt нет возможности переключать режим multiline/не-multiline в регулярных выражениях
    QStringList list=htmlCode.split(QRegularExpression("\\n"));
    QString tempHtmlCode;
    for(int lineNum=0; lineNum<list.count(); ++lineNum)
    {
        // qDebug() << "L" << lineNum << " " << list[lineNum];

        QRegExp replacePBrP("<p.*><br.?\\/><\\/p.*>");
        replacePBrP.setMinimal(true);
        list[lineNum].replace(replacePBrP, "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; \"></p>");
        // qDebug() << "L" << lineNum << " " << list[lineNum];

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

    QStringList chunks=htmlCode.split("<body>"); // Вместо удаления ".*<body>" через медленную регулярку
    if(chunks.length()==2)
        htmlCode=chunks.at(1);

    chunks=htmlCode.split("</body>"); // Вместо удаления "</body>.*" через медленную регулярку
    if(chunks.length()==2)
        htmlCode=chunks.at(0);

    return htmlCode;
}


// Метод, применяемый при выводе отладочной информации, чтобы проще было смотреть на код
QString TypefaceFormatter::htmlSimplyfier(QString htmlCode)
{
    QRegExp rx("style=\"([^\"]*)\"");
    rx.setMinimal(true);
    htmlCode.replace(rx, "");
    // qDebug() << "After replace htmlSimplyfier: " << htmlCode;

    return htmlCode;
}


// Замена в HTML-коде пробелов на спец-последовательности, иначе все повторяющиеся пробелы будут удален Qt-движком
QString TypefaceFormatter::replaceSpaces(QString htmlCode)
{
    QDomDocument doc;
    bool isDocParse=doc.setContent("<root>"+htmlCode+"</root>");

    if(!isDocParse)
    {
        // Вывод сообщения: Слишком сложное форматирование текста. Не могу разобрать выделенный текст. Возможно некоторое нарушение верстки
        showMessageBox(tr("Unreliable parsing of complexly formatted text. Expect formatting inconsistencies."));
        return htmlCode;
    }

    // Рекурсивная правка текстовых узлов, начиная с корневого элемента
    recurseReplaceSpaces(doc.documentElement());

    // Особенность Qt. Преобразование DOM-XML портит символ амперсанда. Поэтому он восстанавливается
    htmlCode=doc.toString(0).replace("&amp;#65533;", "&#65533;");

    htmlCode.replace("<root>", "");
    htmlCode.replace("</root>", "");

    return htmlCode;
}


void TypefaceFormatter::recurseReplaceSpaces(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();

    // Если текущий узел существует
    while(!(domNode.isNull()))
    {
        if(domNode.isText()) // Если текущий узел - это текст
        {
            QDomText domText = domNode.toText();
            if(!domText.isNull())
            {
                QString text=domText.data();
                // qDebug() << "Found text node: " << text;

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


// Замена заранее внесенных символов ReplacementCharacter на пробелы
// Если предварительно не заменять пробелы на ReplacementCharacter,
// то пробелы исчезнут, ибо Qt самопроизвольно удаляет ведущие пробелы при вызове textArea->textCursor().insertHtml(htmlCode);
void TypefaceFormatter::replaceReplacementCharacterToSpaceInSelectedText(int startCursorPos, int endCursorPos)
{
    QTextCursor replacementCursor=textArea->textCursor();

    for(int pos=startCursorPos; pos<endCursorPos; pos++)
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
}


// Удаление пробелов происходит из предположения о том, во вставляем тексте вообще не должно быть пробелов,
// т. к. все пробелы были предварительно заменены на RC-символ
int TypefaceFormatter::removeSpaces(int startCursorPos, int calculateEndCursorPos)
{
    // qDebug() << "Replace spaces from pos" << startCursorPos << "to pos" << calculateEndCursorPos;

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

            if(currentChar==QChar::Space)
            {
                // qDebug() << "Find space in pos" << pos;

                // Удаляется пробел
                replacementCursor.deleteChar();

                // Номер текущего символа уменьшается на единицу, чтобы не пропустить следующий символ
                --pos;

                // Так как символ удален, длина всего вставляемого фрагмента уменьшается на единицу
                --calculateEndCursorPos;

            }
        }
    }

    return calculateEndCursorPos; // Возвращается последняя позиция в выделении, так как количество символов в выделении изменилось
}


QString TypefaceFormatter::replaceTabs(QString htmlCode)
{
    htmlCode.replace(QChar::Tabulation, "&#9;");
    return htmlCode;
}


// Замена самопроизвольно вставляемых Qt концевых пробелов (последовательность пробел+QChar::ParagraphSeparator на QChar::ParagraphSeparator)
// Эти самопроизвольно встваляемые пробелы появляются при вызове textArea->textCursor().insertHtml(htmlCode);
// Метод временно не используется
int TypefaceFormatter::replaceSpaceAndParagraphSeparatorToParagraphSeparator(int startCursorPos, int calculateEndCursorPos)
{
    QTextCursor replacementCursor=textArea->textCursor();

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
            // qDebug() << "Pos: " << pos << " Char: " << currentChar << " Char code: " << currentChar.unicode();

            if(currentChar==QChar::ParagraphSeparator && flagPreviousSpace)
            {
                // qDebug() << "Find space + paragraph separator";

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

    return calculateEndCursorPos; // Возвращается последняя позиция в выделении, так как количество символов в выделении изменилось
}


// Упрощенная очистка начертания текста
// Взята из старой версии MyTetra 1.32.163
// Используется для очистки начертания в строках, отформатированных как нумерованное или ненумерованное перечисление
// Так как при использовании стандартного более агрессивного метода очистки, происходит создание новой строки в месте выделения,
// а условие в агресивный метод невозможно вставить, так как на вход очищающего кода (даже в случае одного
// слова из нумерованной строки) подается конструкция <ul><li>слово</li></ul>, которую невозможно отличить от конструкции
// с реальным одельным пунктом перечисления
void TypefaceFormatter::clearSimple(void)
{
    bool flagCursorOnEmptyLine=editor->cursorPositionDetector->isCursorOnEmptyLine();
    bool flagCursorOnSpaceLine=editor->cursorPositionDetector->isCursorOnSpaceLine();

    // Очистка возможна только если что-то выделено
    // Или курсор стоит на пустой строке с одним символом перевода строки
    // Или курсор стоит на строке, в которой нет текста
    if(!(textArea->textCursor().hasSelection() ||
         flagCursorOnEmptyLine ||
         flagCursorOnSpaceLine))
        return;

    if(flagCursorOnSpaceLine)
        (textArea->textCursor()).select(QTextCursor::LineUnderCursor);

    applyStandartFontForSelection();

    // Очищается формат символов
    QColor clearColor;
    QBrush clearBrush( clearColor );
    QTextCharFormat clearCharFormat;
    clearCharFormat.setForeground( clearBrush );
    textArea->setCurrentCharFormat( clearCharFormat );

    // Если выделен блок
    // или курсор на пустой линии
    // или курсор на линии на которой нет символов
    if(editor->cursorPositionDetector->isBlockSelect() ||
            flagCursorOnEmptyLine ||
            flagCursorOnSpaceLine)
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
    if(flagCursorOnSpaceLine)
        textArea->moveCursor(QTextCursor::StartOfLine);
}


void TypefaceFormatter::onTextOnlyClicked()
{
    int startCursorPos=textArea->textCursor().anchor(); // Начало выделения
    int stopCursorPos=textArea->textCursor().position(); // Конец выделения
    // qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos;

    // Если выделение было сзаду-наперед, надо поменять начальную и конечную позицию местами
    bool isSelectionReverse=false;
    if(startCursorPos>stopCursorPos)
    {
        int tempCursorPos=startCursorPos;
        startCursorPos=stopCursorPos;
        stopCursorPos=tempCursorPos;

        isSelectionReverse=true;
    }
    // qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos;

    bool flag_cursor_on_empty_line=editor->cursorPositionDetector->isCursorOnEmptyLine();
    bool flag_cursor_on_space_line=editor->cursorPositionDetector->isCursorOnSpaceLine();

    // Очистка возможна только если что-то выделено
    // Или курсор стоит на пустой строке с одним символом перевода строки
    // Или курсор стоит на строке, в которой нет текста
    if(!(textArea->textCursor().hasSelection() ||
         flag_cursor_on_empty_line ||
         flag_cursor_on_space_line))
        return;

    // Запоминается положение прокрутки окна редактирования.
    // Это нужно, чтобы вернуться к такому положению после всех действий.
    // Иначе прокрутка перепрыгнет наверх документа
    int scrollBarPosition=editor->getScrollBarPosition();

    textArea->textCursor().beginEditBlock();

    // Если курсор на строке, у которой нет текста
    if(flag_cursor_on_space_line)
        (textArea->textCursor()).select(QTextCursor::LineUnderCursor);

    // Запоминается выделенный текст - только текст, без форматирования
    QString text=textArea->textCursor().selectedText();
    // qDebug() << "Text without formatting: " << text;

    // Удаляются спецсимволы, замещающие картинки. Если этого не сделать, после вставки на экране текст будет виден,
    // а во внутреннем представлении исчезнет целый абзац, где была картинка, и текст абзаца не будет сохранен
    text=text.replace(QChar::ObjectReplacementCharacter, QChar::Space);
    // qDebug() << "Text without difficul characters: " << text;

    // Удаление выделенного фрагмента
    textArea->textCursor().removeSelectedText();

    // С помощью дополнительного курсора выясняется последняя позиция в тексте, в котором удален выделенный фрагмент
    QTextCursor cursor=textArea->textCursor();
    cursor.movePosition(QTextCursor::End);
    int afterRemoveSelectionLen=cursor.position();
    // qDebug() << "After remove selection length: " << afterRemoveSelectionLen;

    // Вставка запомненного текста
    textArea->textCursor().insertText(text);
    // qDebug() << "After insert HTML: "<< textArea->toHtml();

    // С помощью дополнительного курсора выясняется последняя позиция в тексте, в котором вставлен очищенный фрагмент
    cursor.movePosition(QTextCursor::End);
    int afterClearLen=cursor.position();
    // qDebug() << "After clear length: " << afterClearLen;

    // Вычисляется последняя позиция выделения очищенного текста
    int calculateEndCursorPos=startCursorPos + (afterClearLen - afterRemoveSelectionLen);
    // qDebug() << "Calculate end cursor pos: " << calculateEndCursorPos;


    // ********************************
    // Выделение вставленного фрагмента
    // Если его не сделать, то первая строка получит дополнительные вертикальные отступы. Это особенность Qt
    // ********************************
    if(!isSelectionReverse)
    {
        cursor.setPosition(startCursorPos, QTextCursor::MoveAnchor);
        cursor.setPosition(calculateEndCursorPos, QTextCursor::KeepAnchor);
    }
    else
    {
        cursor.setPosition(calculateEndCursorPos, QTextCursor::MoveAnchor);
        cursor.setPosition(startCursorPos, QTextCursor::KeepAnchor);
    }
    textArea->setTextCursor(cursor);

    applyStandartFontForSelection();
    removeAnchorDataForSelection();
    clearColorForSelection();

    // Если выделен блок
    // или курсор на пустой линии
    // или курсор на линии на которой нет символов
    if(editor->cursorPositionDetector->isBlockSelect() ||
            flag_cursor_on_empty_line ||
            flag_cursor_on_space_line)
        applyPureBlockFormatForSelection();

    // Если была работа со строкой, в которой нет символов,
    // курсор переносится на начало строки, чтобы не путать пользователя
    if(flag_cursor_on_space_line)
        textArea->moveCursor(QTextCursor::StartOfLine);

    textArea->textCursor().endEditBlock();

    editor->setScrollBarPosition(scrollBarPosition);

    // Вызывается метод, как будто переместился курсор с выделением, чтобы
    // обновились состояния подсветок кнопок форматирования
    editor->onSelectionChanged();
    editor->updateIndentsliderToActualFormat();

    emit updateOutlineButtonHiglight();
}


/*
void TypefaceFormatter::onTextOnlyClickedV1()
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
*/

void TypefaceFormatter::onFixBreakSymbolClicked()
{
    textArea->textCursor().beginEditBlock();

    int startCursorPos=textArea->textCursor().position();
    int stopCursorPos=textArea->textCursor().anchor();

    // Получение исходного кода выделенного фрагмента
    QString htmlCode=textArea->textCursor().selection().toHtml();

    // qDebug() << "Before replace break in onFixBreakSymbolClicked: " << htmlCode;

    // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
    // Поэтому напрямую регвыру указывается что кванторы должны быть ленивые
    QRegExp replace_expression("<br\\s?/>");
    replace_expression.setMinimal(true);

    htmlCode.replace(replace_expression, "</p><p>");

    // qDebug() << "After remove style: " << htmlCode;

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

    // Нельзя устанавливать через интерфейс пользователя неизвестный шрифт
    if(font.family()=="") {
        return;
    }

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

    // Текущий цвет шрифта возле курсором
    QColor currentColor = textArea->textColor();

    // Формат символов под курсором
    QTextCharFormat textAreaCharFormat = textArea->currentCharFormat();

    // Есть ли ForegroundBrush под курсором в тексте
    bool hasForegroundBrush = textAreaCharFormat.hasProperty(QTextFormat::ForegroundBrush);

    // Если нет ForegroundBrush в тексте под курсором, то
    // за цвет кнопки берется цвет foreground редактора textArea (QTextEdit)
    // (это позволяет учитывать также цвет шрифта, заданный в файле stylesheet.css)
    if(!hasForegroundBrush)
        currentColor = textArea->palette().windowText().color();

    // Диалог запроса цвета текста
    QColor selectedColor = QColorDialog::getColor(currentColor, editor, tr("Select text color"), QColorDialog::ShowAlphaChannel);

    // Если цвет выбран, и он правильный
    if(selectedColor.isValid())
    {
        // Меняется цвет кнопки
        emit changeFontcolor( selectedColor );
    }
}


// Вставка горизонтальной линии в "пустой" абзац, где расположен курсор (пустой абзац заменяется на горизонтальную линию)
// Если есть выделение в тексте, или курсор стоит на тексте, вставка не производится
void TypefaceFormatter::onInsertHorizontalLineClicked()
{
    // Если есть выделение в тексте, то вставка не производится
    if (textArea->textCursor().hasSelection())
        return;

    QTextCursor textCursor = textArea->textCursor();
    textCursor = textArea->textCursor();

    // Если курсор стоит на тексте, то вставка не производится
    textCursor.select(QTextCursor::LineUnderCursor);
    if(!textCursor.selectedText().isNull())
        return;

    int cursorPosition = textCursor.position();

    // Выбираем 1 символ слева от позиции курсора
    textCursor = textArea->textCursor();
    textCursor.movePosition(QTextCursor::StartOfLine);
    textCursor.setPosition(cursorPosition-1, QTextCursor::MoveAnchor);
    textCursor.setPosition(cursorPosition, QTextCursor::KeepAnchor);

    // Определяем, не является ли этот символ слева от курсора мягким переносом
    QString html = textCursor.selection().toHtml();
    QRegExp regExp("<span\\s+style=\"(?:(?:(?:\\s*font-family:'(?:[^<]+)';)(?:\\s*font-size:(?:\\d+)pt;))|(?:(?:\\s*font-size:(?:\\d+)pt;)(?:\\s*font-family:'(?:[^<]+)';)))\">\\s*<br\\s*/\\s*>\\s*</span>");
    regExp.setMinimal(true);
    if(html.indexOf(regExp) != -1)
    {
        // Если это мягкий перенос - заменяем его на абзац
        textArea->setTextCursor(textCursor);
        textArea->textCursor().insertText("\n");
    }

    // Вставка горизонтальной линии в "пустой" абзац (заменяем его на <hr>)
    textCursor.setPosition(cursorPosition, QTextCursor::MoveAnchor);
    textCursor.setPosition(cursorPosition+1, QTextCursor::KeepAnchor);
    textArea->setTextCursor(textCursor);
    textArea->textCursor().removeSelectedText();
    textArea->moveCursor(QTextCursor::Left);
    textArea->insertHtml("<hr>");
    textArea->moveCursor(QTextCursor::Down);
    textArea->moveCursor(QTextCursor::StartOfLine);
}


// Обработка мягкого переноса
// Учитываются мягкие переносы до выделенного текста (1-й символ до выделения),
// после выделенного текста и в выделенных абзацах
void TypefaceFormatter::workingSoftCarryInSelection()
{
    // Если нет выделения, то возврат
    if(!textArea->textCursor().hasSelection())
        return;

    int scrollBarPosition=editor->getScrollBarPosition();

    // Запоминаем первоначальное выделение текста
    int selectionStart = textArea->textCursor().selectionStart();
    int selectionEnd   = textArea->textCursor().selectionEnd();

    // Выбираем 1 символ слева от начала выделения текста
    QTextCursor textCursor = textArea->textCursor();
    textCursor.movePosition(QTextCursor::StartOfLine);
    textCursor.setPosition(selectionStart-1, QTextCursor::MoveAnchor);
    textCursor.setPosition(selectionStart, QTextCursor::KeepAnchor);

    // Определяем, является ли этот 1-й символ слева от выделения текста мягким переносом
    if(textCursor.anchor() != 0 && textCursor.position() != 0) // Пропускаем начало документо
    {
        QString html = textCursor.selection().toHtml();
        QRegExp regExp("<span\\s+style=\"(?:(?:(?:\\s*font-family:'(?:[^<]+)';)(?:\\s*font-size:(?:\\d+)pt;))|(?:(?:\\s*font-size:(?:\\d+)pt;)(?:\\s*font-family:'(?:[^<]+)';)))\">\\s*(?:<br\\s*/\\s*>\\s*){1,}\\s*</span>");
        regExp.setMinimal(true);
        if(html.indexOf(regExp) != -1)
        {
            // Если это мягкий перенос - заменяем его на абзац
            textArea->setTextCursor(textCursor);
            textArea->textCursor().insertText("\n");
        }
    }

    // Расширяем выделение на 1 символ вправо, чтобы захватить мягкий перенос в конце выделения, если он есть
    textCursor.setPosition(selectionStart, QTextCursor::MoveAnchor);
    textCursor.setPosition(selectionEnd+1, QTextCursor::KeepAnchor);
    textArea->setTextCursor(textCursor);

    // Ищем мягкий перенос в качестве пустого абзаца в расширенном вправо выделении
    QString htmlCode = textArea->textCursor().selection().toHtml();
    QRegExp regExp("<span\\s+style=\"(?:(?:(?:\\s*font-family:'(?:[^<]+)';)(?:\\s*font-size:(?:\\d+)pt;))|(?:(?:\\s*font-size:(?:\\d+)pt;)(?:\\s*font-family:'(?:[^<]+)';)))\">\\s*(?:<br\\s*/\\s*>\\s*){1,}\\s*</span>");
    regExp.setMinimal(true);
    if(htmlCode.indexOf(regExp) != -1)
    {
        // Заменяем проблемный код в html
        htmlCode.replace(regExp, "</p><p><br/>");
        textArea->textCursor().insertHtml(htmlCode);
    }

    // Теперь обрабатываем смитуацию, когда на конце строки - мягкий перенос
    htmlCode = textArea->textCursor().selection().toHtml();
    regExp.setPattern("(<span\\s+style=\"\\s*((?:[^<]+);\">)(?:.+)\\s*)(?:(?:<br\\s*/\\s*>\\s*){1,}\\s*)(</span>)");
    regExp.setMinimal(true);
    if(htmlCode.indexOf(regExp) != -1)
    {
        // Удаляем в html код <br/>
        htmlCode.replace(regExp, "\\1");
        textArea->textCursor().insertHtml(htmlCode);
    }

    // Восстанавливаем выделение курсора
    textCursor.setPosition(selectionStart, QTextCursor::MoveAnchor);
    textCursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
    textArea->setTextCursor(textCursor);

    editor->setScrollBarPosition(scrollBarPosition);
}


void TypefaceFormatter::onLowerCase()
{
    QTextCharFormat format;
    format.setFontCapitalization(QFont::AllLowercase);
    mergeFormatOnWordOrSelection(format);
}


void TypefaceFormatter::onUpperCase()
{
    QTextCharFormat format;
    format.setFontCapitalization(QFont::AllUppercase);
    mergeFormatOnWordOrSelection(format);
}


void TypefaceFormatter::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textArea->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textArea->mergeCurrentCharFormat(format);
    // textArea->setFocus(Qt::TabFocusReason);
}


// Слот, срабатыващий при нажатии на кнопку выбора цвета фона текста
void TypefaceFormatter::onBackgroundcolorClicked()
{
    // TRACELOG

    // Текущий цвет фона под курсором
    QColor currentColor = textArea->textBackgroundColor();

    // Формат символов под курсором
    QTextCharFormat textAreaCharFormat = textArea->currentCharFormat();

    // Есть ли BackgroundBrush под курсором в тексте
    bool hasTextBackgroundBrush = textAreaCharFormat.hasProperty(QTextFormat::BackgroundBrush);

    // Есть ли BackgroundBrush в тексте под курсором
    if(hasTextBackgroundBrush)
    {
        // Если есть BackgroundBrush в тексте под курсором, то
        // в диалог выбора цвета передаем цвет заливки текста под курсором
        currentColor = textArea->textBackgroundColor();
    }
    else
    {
        // Проверка, есть ли таблица под курсором и/или подключены стили из stylesheet.css
        QTextCursor txtCursor = textArea->textCursor();
        QTextTable *textTable = txtCursor.currentTable();
        if(textTable != nullptr)
        {
            // Если курсор находится в таблице
            QTextTableFormat textTableFormat = textTable->format();
            QTextTableCell tableCell = textTable->cellAt(txtCursor);
            QTextCharFormat tableCellFormat = tableCell.format();
            QColor tableColor = textTableFormat.background().color();
            QColor charColor = tableCellFormat.background().color();

            // Есть ли BackgroundBrush в таблице под курсором
            bool hasTableBackgroundBrush = textTableFormat.hasProperty(QTextFormat::BackgroundBrush);

            // Есть ли BackgroundBrush в ячейке под курсором
            bool hasCelBackgroundBrush = tableCellFormat.hasProperty(QTextFormat::BackgroundBrush);

            if(hasTableBackgroundBrush && hasCelBackgroundBrush && charColor.isValid())
            {
                // Если есть BackgroundBrush в таблице под курсором и
                // есть BackgroundBrush в ячейке под курсором, то
                // в диалог выбора цвета передаем цвет заливки ячейки
                currentColor = charColor;
            }
            else if(hasTableBackgroundBrush && !hasCelBackgroundBrush && tableColor.isValid())
            {
                // Если есть BackgroundBrush в таблице под курсором но
                // нет BackgroundBrush в ячейке под курсором, то
                // в диалог выбора цвета передаем цвет заливки таблицы
                currentColor = tableColor;
            }
            else
            {
                // Если нет BackgroundBrush в таблице под курсором и
                // нет BackgroundBrush в ячейке под курсором, то
                // в диалог выбора цвета передаем цвет background редактора textArea (QTextEdit)
                // (это позволяет учитывать также цвет фона, заданный в файле stylesheet.css)
                currentColor = textArea->palette().window().color();
            }
        }
        else
        {
            // Если нет BackgroundBrush в тексте под курсором, то
            // в диалог выбора цвета передаем цвет background редактора textArea (QTextEdit)
            // (это позволяет учитывать также цвет фона, заданный в файле stylesheet.css)
            currentColor = textArea->palette().window().color();
        }
    }

    // Диалог запроса цвета фона
    QColor selectedColor = QColorDialog::getColor(currentColor, editor, tr("Select background color"), QColorDialog::ShowAlphaChannel);

    // Если цвет выбран, и он правильный
    if(selectedColor.isValid())
    {
        // Меняется цвет кнопки
        emit changeBackgroundcolor( selectedColor );
    }
}
