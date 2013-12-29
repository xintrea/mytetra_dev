#ifndef _EDITORTEXTAREA_H_
#define _EDITORTEXTAREA_H_

#include <QTextEdit>
#include <QPaintEvent>
#include <QMimeData>

class EditorTextArea : public QTextEdit
{
 Q_OBJECT

 public:
  EditorTextArea(QWidget *parent=0);
  ~EditorTextArea(void);

  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);

  virtual bool canInsertFromMimeData(const QMimeData *source) const;
  virtual void insertFromMimeData(const QMimeData *source);

  bool get_showformatting(void);
  void set_showformatting(bool i);

  int get_indent_started_left(void);
  int get_indent_started_right(void);

 public slots:
  void show_indetedge(bool i);
  void set_indentedge_pos(int i);

 private:

  bool flagShowFormatting; // Рисовать ли символы форматирования

  bool flagShowIndentEdge; // Рисовать ли линию отступа
  int  posIndentEdge; // По какой координате рисовать линию отступа

  virtual bool eventFilter(QObject *o, QEvent *e);
};

#endif // _EDITORTEXTAREA_H_
