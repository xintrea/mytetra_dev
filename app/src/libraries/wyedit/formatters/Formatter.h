#ifndef FORMATTER_H
#define FORMATTER_H

#include <QObject>

/*
#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
*/
// Основной класс "форматирОвщиков" текста. От него наследуются все форматировщики
// По сути, этот класс просто содержит указатель на область редактирования
// Унаследованные от него форматировщики реализуют ограниченную область форматирования:
// форматирование начертания, форматирование размещения текста, форматирование картинок и т. д.

class Editor;
class EditorConfig;
class EditorTextArea;

class Formatter : public QObject
{
  Q_OBJECT

public:
  explicit Formatter(QObject *parent = nullptr);
  virtual ~Formatter();

  void setEditor(Editor *iEditor);
  void setTextArea(EditorTextArea *iTextArea);

protected:

  Editor *editor;
  EditorConfig *editorConfig;

  EditorTextArea *textArea;

};

#endif // FORMATTER_H
