#ifndef TYPEFACEFORMATTER_H
#define TYPEFACEFORMATTER_H

#include "Formatter.h"

// Класс форматирования для начертания текста


class TypefaceFormatter : public Formatter
{
public:
  TypefaceFormatter();

public slots:

  // Действия в области редактирования
  void onBoldClicked(void);
  void onItalicClicked(void);
  void onUnderlineClicked(void);
  void onMonospaceClicked(void);
  void onCodeClicked(void);
  void onClearClicked(void);

  void onFontselectChanged(const QFont &font);
  void onFontsizeChanged(int i);
  void onFontcolorClicked();
};

#endif // TYPEFACEFORMATTER_H
