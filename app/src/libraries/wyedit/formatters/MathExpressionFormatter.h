#ifndef MATHEXPRESSIONFORMATTER_H
#define MATHEXPRESSIONFORMATTER_H

#include <QTextImageFormat>
#include <QTextDocumentFragment>

#include "Formatter.h"

// Класс для работы с математическими выражениями

class MathExpressionFormatter : public Formatter
{
  Q_OBJECT

public:
  MathExpressionFormatter();

  QString mathExpressionOnSelect(void);
  QString mathExpressionOnCursor(void);

  void editMathExpression(void);

signals:

public slots:

  void onMathExpressionClicked(void);
  void onContextMenuEditMathExpression(void);
};

#endif // MATHEXPRESSIONFORMATTER_H
