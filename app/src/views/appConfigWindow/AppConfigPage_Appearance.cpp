#include <QDebug>
#include <QVBoxLayout>

#include "AppConfigPage_Appearance.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "libraries/wyedit/EditorShowTextDispatcher.h"


extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


AppConfigPage_Appearance::AppConfigPage_Appearance(QWidget *parent) : ConfigPage(parent)
{
    setupUi();
    setupSignals();
    assembly();
}


AppConfigPage_Appearance::~AppConfigPage_Appearance()
{

}


void AppConfigPage_Appearance::setupUi()
{
    qDebug() << "Create appearance config page";

    // Настройка запуска MyTetra в свернутом окне
    runInMinimizedWindow=new QCheckBox(this);
    runInMinimizedWindow->setText(tr("Run MyTetra in a minimized window"));
    runInMinimizedWindow->setChecked( mytetraConfig.get_runinminimizedwindow() );


    // Настройка поведения открепляемых окон, как они будут сворачиваться и разворачиваться
    dockableWindowsBehavior=new QCheckBox(this);
    dockableWindowsBehavior->setText(tr("Hide detached windows if close main window"));
    dockableWindowsBehavior->setChecked( mytetraConfig.getDockableWindowsBehavior()=="together" );
}


void AppConfigPage_Appearance::setupSignals()
{

}


void AppConfigPage_Appearance::assembly()
{
    // Группировщик виджетов настройки поведения окна
    behaviorBox=new QGroupBox(this);
    behaviorBox->setTitle(tr("Windows behavior"));

    // Виджеты вставляются в группировщик настроек курсора при навигации по истории
    QVBoxLayout *behaviorLayout = new QVBoxLayout;
    behaviorLayout->addWidget(runInMinimizedWindow);
    behaviorLayout->addWidget(dockableWindowsBehavior);
    behaviorBox->setLayout(behaviorLayout);

    // Собирается основной слой
    QVBoxLayout *centralLayout=new QVBoxLayout();
    centralLayout->addWidget(behaviorBox);
    centralLayout->addStretch();

    // Основной слой устанавливается
    setLayout(centralLayout);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Appearance::applyChanges()
{
    qDebug() << "Apply changes appearance";

    int result=0;

    // Сохраняется настройка режима запуска MyTetra - обычный или свернутый
    if(mytetraConfig.get_runinminimizedwindow()!=runInMinimizedWindow->isChecked())
    {
        mytetraConfig.set_runinminimizedwindow(runInMinimizedWindow->isChecked());
    }

    // Сохраняется настройка поведения открепляемых окон
    QString mode=dockableWindowsBehavior->isChecked() ? "together" : "single";
    if(mytetraConfig.getDockableWindowsBehavior()!=mode)
    {
        mytetraConfig.setDockableWindowsBehavior(mode);

        EditorShowTextDispatcher::instance()->switchBehavior(mode);
    }

    return result;
}
