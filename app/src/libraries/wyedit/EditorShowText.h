#ifndef EDITORSHOWTEXT_H
#define EDITORSHOWTEXT_H

#include <QDialog>
#include <QTextEdit>
#include <QEvent>


// Класс отдельного (открепляемого) окна для просмотра текста записи без возможности редактирования
class EditorShowText : public QDialog
{
  Q_OBJECT
public:
  explicit EditorShowText(QWidget *parent = 0);
  virtual ~EditorShowText();

  void setHtml(QString text);
  void setDocument(QTextDocument *document);

signals:

public slots:

private:

 QTextEdit *textArea;

 int geomX;
 int geomY;
 int geomW;
 int geomH;

 void setupUi(void);
 void setupSignals(void);
 void assembly(void);

 void hideEvent(QHideEvent *event);
 void showEvent(QShowEvent *event);
};

#endif // EDITORSHOWTEXT_H
