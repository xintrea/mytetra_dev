#ifndef LISTFORMATTER_H
#define LISTFORMATTER_H

#include <QTextListFormat>

#include "Formatter.h"

// Класс форматирования текста как списка


class ListFormatter : public Formatter
{
  Q_OBJECT

public:
  ListFormatter();

public slots:

  void onNumericlistClicked(void);
  void onDotlistClicked(void);

protected:

 void formatToList(QTextListFormat::Style setFormat);

};

#endif // LISTFORMATTER_H
