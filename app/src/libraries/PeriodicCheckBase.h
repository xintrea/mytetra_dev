#ifndef PERIODICCHECKBASE_H
#define PERIODICCHECKBASE_H

#include "TimerMonitoring.h"

// Класс, который запускает периодическую проверку того,
// что сторонняя программа изменила файл дерева на диске.
// Функционал класа работает только в случае, если в настройках
// был установлен флажок
// "Периодическая проверка дерева записей на изменение сторонними программами"
// получаемый через mytetraConfig.getEnablePeriodicCheckBase()


class PeriodicCheckBase : public TimerMonitoring
{
    Q_OBJECT

public:

    void init();

signals:

    void doUpdateDetachedWindows();

protected:

    bool isStartEnabled();
    void timerEvent(QTimerEvent *event);

};

#endif	/* PERIODICCHECKBASE_H */

