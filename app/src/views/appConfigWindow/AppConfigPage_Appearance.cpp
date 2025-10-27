#include <QWidget>
#include <QDebug>
#include <QVBoxLayout>

#include "AppConfigPage_Appearance.h"

#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "libraries/helpers/DiskHelper.h"
#include "libraries/wyedit/EditorShowTextDispatcher.h"


extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;
extern FixedParameters fixedParameters;


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

    // Выбор темы интерфейса
    themeLabel=new QLabel(this);
    themeLabel->setText(tr("Interface theme"));

    theme=new MtComboBox(this);

    // Размер списка
    theme->setMinimumContentsLength( fixedParameters.themesAvailableList.size() );

    // Таблица переводов элементов списка
    QMap<QString, QString> translateNames;
    translateNames[ "default" ] = tr("Default");
    translateNames[ "dark" ]    = tr("Dark");

    // Какая строка будет выбрана как текущая тема
    int count = -1;
    int currentIndex = -1;

    // Список тем наполняется и одновременно вычисляется индекс текущей темы
    for (auto name : fixedParameters.themesAvailableList )
    {
        QString translateName;

        if ( translateNames.contains(name) )
        {
            translateName = translateNames[name];
        }
        else
        {
            translateName = "Unknown";
        }

        theme->addItem(translateName);

        count++;
        if ( name == mytetraConfig.getInterfaceTheme() )
        {
            currentIndex = count;
        }
    }

    theme->setCurrentIndex( currentIndex );


    // Настройка запуска MyTetra в свернутом окне
    runInMinimizedWindow=new QCheckBox(this);
    runInMinimizedWindow->setText(tr("Run MyTetra in a minimized window"));
    runInMinimizedWindow->setChecked( mytetraConfig.get_runinminimizedwindow() );


    // Настройка поведения открепляемых окон, как они будут сворачиваться и разворачитьваться
    dockableWindowsBehavior=new QCheckBox(this);
    dockableWindowsBehavior->setText(tr("Hide detached windows if close main window"));
    dockableWindowsBehavior->setChecked( mytetraConfig.getDockableWindowsBehavior()=="together" );
}


void AppConfigPage_Appearance::setupSignals()
{

}


void AppConfigPage_Appearance::assembly()
{
    // Группировщик настроек интерфейса
    interfaceBox=new QGroupBox(this);
    interfaceBox->setTitle(tr("Interface"));

    // Выбор темы
    QHBoxLayout *themeLayout=new QHBoxLayout();
    themeLayout->addWidget(theme);

    // Виджеты вставляются в группировщик настроек курсора при навигации по истории
    QGridLayout *interfaceLayout = new QGridLayout;
    interfaceLayout->addWidget(themeLabel,0,0);
    interfaceLayout->addLayout(themeLayout,0,1);
    interfaceLayout->setColumnStretch(1,100);
    interfaceBox->setLayout(interfaceLayout);

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
    centralLayout->addWidget(interfaceBox);
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

    // Если был изменена тема
    if ( mytetraConfig.getInterfaceTheme() !=
         fixedParameters.themesAvailableList[ theme->currentIndex() ] )
    {
        mytetraConfig.setInterfaceTheme(
            fixedParameters.themesAvailableList[ theme->currentIndex() ] );

        result=1;
    }

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
