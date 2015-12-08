#ifndef IMAGEFORMATTER_H
#define IMAGEFORMATTER_H

#include <QTextImageFormat>

#include "Formatter.h"

// Класс для работы с картинками в тексте


class ImageFormatter : public Formatter
{
  Q_OBJECT

public:
  ImageFormatter();

  QTextImageFormat imageFormatOnSelect(void);
  QTextImageFormat imageFormatOnCursor(void);

  void editImageProperties(void);

public slots:

  void onInsertImageFromFileClicked(void);
  void onContextMenuEditImageProperties(void);

};

#endif // IMAGEFORMATTER_H
