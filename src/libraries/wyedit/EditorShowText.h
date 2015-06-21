#ifndef EDITORSHOWTEXT_H
#define EDITORSHOWTEXT_H

#include <QDialog>
#include <QTextEdit>


// Класс отдельного (открепляемого) окна для просмотра текста записи без возможности редактирования
class EditorShowText : public QDialog
{
  Q_OBJECT
public:
  explicit EditorShowText(QWidget *parent = 0);
  virtual ~EditorShowText();

  void setHtml(QString text);

signals:

public slots:

private:

 QTextEdit *textArea;

 void setupUi(void);
 void setupSignals(void);
 void assembly(void);

};

#endif // EDITORSHOWTEXT_H
