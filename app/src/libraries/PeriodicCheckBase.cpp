#include <QDateTime>

#include "main.h"
#include "TimerMonitoring.h"
#include "PeriodicCheckBase.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/helpers/MessageHelper.h"
#include "libraries/wyedit/EditorShowTextDispatcher.h"


extern AppConfig mytetraConfig;


void PeriodicCheckBase::init()
{
    TimerMonitoring::init();

    connect(this, &PeriodicCheckBase::doUpdateDetachedWindows,
            EditorShowTextDispatcher::instance(), &EditorShowTextDispatcher::closeWindowForNonExistentRecords,
            Qt::QueuedConnection);
}


bool PeriodicCheckBase::isStartEnabled()
{
    return mytetraConfig.getEnablePeriodicCheckBase();
}


// Действия, происходящие по таймеру
void PeriodicCheckBase::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    // qDebug() << "In timer working method";

    QDateTime lastSave=knowTreeModel->getLastSaveDateTime();
    QDateTime lastLoad=knowTreeModel->getLastLoadDateTime();

    // Если доступа к файла за текущий сеанс ни разу не производилось, нечего сравнивать
    if(lastSave.isNull() && lastLoad.isNull())
        return;

    // Время последнего действия с деревом (чтение или запись)
    QDateTime lastAccess = lastSave > lastLoad ? lastSave : lastLoad;

    // Время последнего изменения файла дерева
    QString fileName=mytetraConfig.get_tetradir()+"/mytetra.xml";
    QFileInfo fileInfo(fileName);
    QDateTime modifyDateTime=fileInfo.lastModified();

    // Если дерево на диске было изменено относительно действий с деревом в памяти
    if(modifyDateTime>lastAccess)
    {
        // Дерево перечитывается с диска
        (find_object<MainWindow>("mainwindow"))->reload();

        // Отправляется сигнал открепленным окнам чтобы они перечитали свои данные
        emit doUpdateDetachedWindows();

        // Если разрешена выдача сообщения о том, что база данных была изменена
        if(mytetraConfig.getEnablePeriodicCheckMessage())
            showMessageBox(tr("The database was changed by external application or services.\n"
                              "MyTetra reload the database tree to keep data consistency."));
    }
}
