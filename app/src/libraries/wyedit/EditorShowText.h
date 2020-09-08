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
  explicit EditorShowText(QWidget *parent = nullptr);
  virtual ~EditorShowText();

  void setHtml(QString text);
  void setDocument(QSharedPointer<QTextDocument> pDocument);

signals:

public slots:

private:

 QSharedPointer<QTextEdit> pTextArea=nullptr;
 QSharedPointer<QTextDocument> pTextDocument=nullptr;

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
