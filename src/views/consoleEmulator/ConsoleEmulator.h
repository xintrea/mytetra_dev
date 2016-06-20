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
// |                   [Close]                      |
// +------------------------------------------------+

class ConsoleEmulator : public QDialog
{
 Q_OBJECT

public:
 ConsoleEmulator(QWidget *parent=0);
 ~ConsoleEmulator();

 void setMessageText(QString text);

 void setConsoleOutput(QString text);
 void clearConsoleOutput(void);
 void addConsoleOutput(QString text);

 // Преобразование внешнего вида консольного эмулятора к отображению ошибки
 void switchToErrorView(void);

public slots:

 // Слот срабатывающий при нажатии Cancel или при принудительном закрытии окна
 void onCancelClick(void);

 // Слот срабатывающий при клике на кнопку скрыть/показать консольный вывод
 void onDetailsClick(void);

signals:

 // Сигнал, вырабатывающийся в слоте onCancelConsole()
 void cancelConsole(void);

 // Сигнал, вырабатывающийся при закрытии окна
 void dialogHide(void);

private:

 void setupUI(void);
 void setupSignals(void);
 void assembly(void);

 // Переопределенный метод, срабатывает при принудительном закрытии окна
 void closeEvent(QCloseEvent *event);

 bool event(QEvent *event);

 WaitClock *waitClock;
 QLabel *messageLabel;
 QPushButton *buttonCancel;
 QToolButton *buttonDetails;

 // Верхний блок вида "Картинка |Тут какая-то надпись| [v] | [Cancel]"
 QHBoxLayout *upToolbar;

 QTextEdit *consoleOutput;

 QShortcut *escShortcut;

 // Возникала ли ошибка при выполнении команд
 bool isError;
};

#endif /* _CONSOLEEMULATOR_H_ */
