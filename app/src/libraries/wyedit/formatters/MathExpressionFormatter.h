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

  void createGifFromMathExpression(QString iMathExpression, QString iFileName, bool removeTeXFileToTrash=true);

signals:

public slots:

  void onMathExpressionClicked(void);
  void onContextMenuEditMathExpression(void);
  void onDoubleClickOnImage(void);

protected:

  void addMathExpression(void);
  void editMathExpression(QString iMathExpressionText);

  QString getMathExpressionFromUser(QString iMathExpressionText="");
  void insertMathExpressionToTextArea(QString iMathExpressionText);

  QString getMathExpressionByImageName(QString resourceImageName);

};

#endif // MATHEXPRESSIONFORMATTER_H
