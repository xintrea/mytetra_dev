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

 void setText(QString text);
 QString getText();
 bool isModified(); // Выяснение, был ли изменен текст, показанный в диалоге

 void setWordWrapMode(QTextOption::WrapMode mode);
 void setSizeCoefficient(float f);

private:

 float sizeCoefficient;
 QTextEdit *textArea;
 QDialogButtonBox *buttonBox;

 void setupUi(void);
 void setupSignals(void);
 void assembly(void);

 void updateSize();
};

#endif	/* _EDITORMULTILINEINPUTDIALOG_H_ */

