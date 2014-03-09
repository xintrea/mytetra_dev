#ifndef _CONSOLEEMULATOR_H_
#define _CONSOLEEMULATOR_H_

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QTextEdit>
#include <QCloseEvent>
#include <QShortcut>

#include "views/waitClock/WaitClock.h"


// Виджет, показывающий окно с консольным выводом
// Выглядит так:
// +----------+--------------------+-----+----------+
// | Картинка |Тут какая-то надпись| [v] | [Cancel] |
// +----------+--------------------+-----+----------+
// | # ls -1                                        |
// | file1.txt                                      |
// | file2.txt                                      |
// | file3.txt                                      |
// +------------------------------------------------+

class ConsoleEmulator : public QWidget
{
 Q_OBJECT

public:
 ConsoleEmulator(QWidget *parent=0);
 ~ConsoleEmulator();

 void setMessageText(QString text);

 void setConsoleOutput(QString text);
 void clearConsoleOutput(void);
 void addConsoleOutput(QString text);

public slots:

 // Слот срабатывающий при нажатии Cancel или при принудительном закрытии окна
 void onCancelClick(void);

 // Слот срабатывающий при клике на кнопку скрыть/показать консольный вывод
 void onDetailsClick(void);

signals:

 // Сигнал, вырабатывающийся в слоте onCancelConsole()
 void cancelConsole(void);

private:

 void setupUI(void);
 void setupSignals(void);
 void assembly(void);

 // Переопределенный метод, срабатывает при принудительном закрытии окна
 void closeEvent(QCloseEvent *event);

 WaitClock *waitClock;
 QLabel *messageLabel;
 QPushButton *buttonCancel;
 QToolButton *buttonDetails;

 QTextEdit *consoleOutput;

 QShortcut *escShortcut;

};

#endif /* _CONSOLEEMULATOR_H_ */
