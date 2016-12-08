#ifndef EDITORCURSORPOSITIONDETECTOR_H
#define EDITORCURSORPOSITIONDETECTOR_H

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
  bool isCursorOnSpaceLine(void);
  bool isCursorOnEmptyLine(void);
  bool isBlockSelect(void);

  bool isCursorOnReference(void);
  QString referenceHref(void);


private:

  EditorTextArea *textArea;

  bool isCursorOnReferenceSmart(QString &resultHref);


};

#endif // EDITORCURSORPOSITIONDETECTOR_H
