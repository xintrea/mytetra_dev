#ifndef REFERENCEFORMATTER_H
#define REFERENCEFORMATTER_H

#include "Formatter.h"

// Класс форматирования ссылок


class ReferenceFormatter : public Formatter
{
  Q_OBJECT

public:
  ReferenceFormatter();

signals:

public slots:

  void onReferenceClicked(void);
};


#endif // REFERENCEFORMATTER_H
