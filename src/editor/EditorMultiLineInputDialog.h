#ifndef _EDITORMULTILINEINPUTDIALOG_H_
#define	_EDITORMULTILINEINPUTDIALOG_H_

#include <QtGlobal>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>


class EditorMultiLineInputDialog : public QDialog
{
 Q_OBJECT

public:
 EditorMultiLineInputDialog(QWidget *parent=0);
 virtual ~EditorMultiLineInputDialog();

 void set_text(QString text);
 QString get_text();
 bool isModified();
 void set_window_title(QString title);

private:

 QTextEdit *textArea;
 QDialogButtonBox *buttonBox;

 void setup_ui(void);
 void setup_signals(void);
 void assembly(void);
};

#endif	/* _EDITORMULTILINEINPUTDIALOG_H_ */

