#ifndef TYPEFACEFORMATTER_H
#define TYPEFACEFORMATTER_H

#include "Formatter.h"

// Класс форматирования для начертания текста


class TypefaceFormatter : public Formatter
{
  Q_OBJECT

public:
  TypefaceFormatter();

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

  void onFontselectChanged(const QFont &font);
  void onFontsizeChanged(int n);
  void onFontcolorClicked();
};

#endif // TYPEFACEFORMATTER_H
