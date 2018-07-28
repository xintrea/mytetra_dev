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


  // Полный заголовок математического выражения состоит из префикса и номера версии
  // "mytetraMathExpression:v0001:"

  // Префикс в строке описания в запоминаемом PNG-файле (поле Description),
  // после которого идет TeX-код математического выражения
  const QString mathExpDescriptionPrefix="mytetraMathExpression";

  const int mathExpVersion=1; // Допустимая рабочая версия формата хранения формулы
  const int mathExpVersionNumberLen=4; // Сколько символов занимает номер версии (только цифры)

};

#endif // MATHEXPRESSIONFORMATTER_H
