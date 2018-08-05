#ifndef EDITORCURSORPOSITIONDETECTOR_H
#define EDITORCURSORPOSITIONDETECTOR_H

#include <QTextImageFormat>

// Вспомогательный класс, определяющий положение/состояние курсора в тексте
// в зависимости от окружающего форматирования

class EditorTextArea;

class EditorCursorPositionDetector
{

public:
  EditorCursorPositionDetector();

  void setTextArea(EditorTextArea *iTextArea);

  bool isCursorOnImage(void);
  bool isImageSelect(void);
  QTextImageFormat getImageSelectFormat(void);
  int whereImageAtCursor(void);

  bool isCursorOnMathExpression(void);
  bool isMathExpressionSelect(void);

  bool isCursorOnSpaceLine(void);
  bool isCursorOnEmptyLine(void);
  bool isBlockSelect(void);

  bool isCursorOnReference(void);
  QString referenceHref(void);


private:

  EditorTextArea *textArea;

  bool isCursorOnReferenceSmart(QString &resultHref);
  bool isMathExpressionSmartDetect(QString &resourceName);


};

#endif // EDITORCURSORPOSITIONDETECTOR_H
