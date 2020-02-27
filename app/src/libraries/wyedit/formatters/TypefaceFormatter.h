#ifndef TYPEFACEFORMATTER_H
#define TYPEFACEFORMATTER_H

#include <QColor>

#include "Formatter.h"

// Класс форматирования для начертания текста

class QDomNode;
class QTextCharFormat;

class TypefaceFormatter : public Formatter
{
    Q_OBJECT

public:
    TypefaceFormatter();

    enum EasyFormatType {Bold, Italic, Underline, StrikeOut, SuperScript, SubScript};

signals:

    void updateOutlineButtonHiglight();

    void changeFontselectOnDisplay(QString fontFamily);
    void changeFontsizeOnDisplay(int n);
    void changeFontFamily(QString fontFamily);
    void changeFontPointSize(int n);
    void changeFontcolor(const QColor &color);
    void changeBackgroundcolor(const QColor &color);

public slots:

    // Действия в области редактирования
    void onBoldClicked(void);
    void onItalicClicked(void);
    void onUnderlineClicked(void);
    void onStrikeOutClicked(void); // Зачеркивание текста
    void onSuperScriptClicked(void);
    void onSubScriptClicked(void);
    void onMonospaceClicked(void);
    void onCodeClicked(void);
    void onClearClicked(void);
    void onTextOnlyClicked(void);
    void onFixBreakSymbolClicked(void);

    void onFontselectChanged(const QFont &font);
    void onFontsizeChanged(int n);
    void onFontcolorClicked();
    void onBackgroundcolorClicked();

    // Вставка горизонтальной линии в "пустой" абзац, где расположен курсор (пустой абзац заменяется на горизонтальную линию)
    void onInsertHorizontalLineClicked();

    void onLowerCase(); // "Строчные"
    void onUpperCase(); // "ПРОПИСНЫЕ"

private:

    void mergeFormat(const QTextCharFormat &format);
    void smartFormat(int formatType);
    QString replaceSpacesOnlyTags(QString htmlCode);
    QString clearTypeFace(QString htmlCode);
    QString replaceSpaces(QString htmlCode);
    void recurseReplaceSpaces(const QDomNode &node);
    void replaceReplacementCharacterToSpaceInSelectedText(int startCursorPos, int endCursorPos);
    int removeSpaces(int startCursorPos, int calculateEndCursorPos);
    int replaceSpaceAndParagraphSeparatorToParagraphSeparator(int startCursorPos, int calculateEndCursorPos);
    QString replaceTabs(QString htmlCode);
    void applyStandartFontForSelection();
    void applyPureBlockFormatForSelection();
    void clearColorForSelection();
    void clearBackgroundColorForSelection();
    void removeAnchorDataForSelection();
    QString htmlSimplyfier(QString htmlCode);
    void clearSimple(void);
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    // Обработка мягкого переноса
    void workingSoftCarryInSelection();
};

#endif // TYPEFACEFORMATTER_H
