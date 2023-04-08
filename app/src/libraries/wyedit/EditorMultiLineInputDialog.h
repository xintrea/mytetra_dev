#ifndef EDITORMULTILINEINPUTDIALOG_H
#define	EDITORMULTILINEINPUTDIALOG_H

#include <QtGlobal>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>


class EditorMultiLineInputDialog : public QDialog
{
    Q_OBJECT

public:
    EditorMultiLineInputDialog(QWidget *parent=nullptr);
    virtual ~EditorMultiLineInputDialog();

    void setText(QString text);
    QString getText();
    bool isModified(); // Выяснение, был ли изменен текст, показанный в диалоге

    void setWordWrapMode(QTextOption::WrapMode mode);
    void setSizeCoefficient(double f);

public slots:

    void setupShortcuts(void);

private:

    double sizeCoefficient;
    QTextEdit *textArea;
    QDialogButtonBox *buttonBox;

    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    void updateSize();
};

#endif // EDITORMULTILINEINPUTDIALOG_H

