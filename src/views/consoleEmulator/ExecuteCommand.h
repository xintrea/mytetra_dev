#ifndef _EXECUTECOMMAND_H_
#define	_EXECUTECOMMAND_H_

#include <QWidget>
#include <QDialog>
#include <QProcess>

class ExecuteCommand : public QDialog
{
 Q_OBJECT

public:
 ExecuteCommand(QWidget *parent=0);
 virtual ~ExecuteCommand();

 void setCommand(QString cmd);
 void run(void);

 void setWindowTitle(QString title);
 void setMessageText(QString text);

private slots:

 void closeProcess(void);

private:

 QString command;
 QString shell;
 QString windowTitle;
 QString messageText;

 QProcess *process;
};

#endif	/* _EXECUTECOMMAND_H_ */

