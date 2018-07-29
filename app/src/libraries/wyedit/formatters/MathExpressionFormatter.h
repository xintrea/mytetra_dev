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

signals:

public slots:

  void onMathExpressionClicked(void);
  void onContextMenuEditMathExpression(void);

protected:

  void addMathExpression(void);
  void editMathExpression(QString iMathExpressionText);

  QString getMathExpressionFromUser(QString iMathExpressionText="");
  void insertMathExpressionToTextArea(QString iMathExpressionText);

  QString getMathExpressionByImageName(QString resourceImageName);

  void createGifFromMathExpression(QString iMathExpression, QString iFileName);
};

#endif // MATHEXPRESSIONFORMATTER_H
