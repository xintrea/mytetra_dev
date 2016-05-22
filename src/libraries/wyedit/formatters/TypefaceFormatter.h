#ifndef TYPEFACEFORMATTER_H
#define TYPEFACEFORMATTER_H

#include <QColor>

#include "Formatter.h"

// Класс форматирования для начертания текста

class QDomNode;

class TypefaceFormatter : public Formatter
{
  Q_OBJECT

public:
  TypefaceFormatter();

  enum EasyFormatType {Bold, Italic, Underline};

signals:

  void updateOutlineButtonHiglight();

  void changeFontselectOnDisplay(QString fontFamily);
  void changeFontsizeOnDisplay(int n);
  void changeFontFamily(QString fontFamily);
  void changeFontPointSize(int n);
  void changeFontcolor(QColor color);

public slots:

  // Действия в области редактирования
  void onBoldClicked(void);
  void onItalicClicked(void);
  void onUnderlineClicked(void);
  void onMonospaceClicked(void);
  void onCodeClicked(void);
  void onClearClicked(void);
  void onTextOnlyClicked(void);
  void onFixBreakSymbolClicked(void);

  void onFontselectChanged(const QFont &font);
  void onFontsizeChanged(int n);
  void onFontcolorClicked();

private:

  void smartFormat(int formatType);
  QString clearTypeFace(QString htmlCode);
  QString replaceSpaces(QString htmlCode);
  void recurseReplaceSpaces(const QDomNode &node);

};

#endif // TYPEFACEFORMATTER_H
