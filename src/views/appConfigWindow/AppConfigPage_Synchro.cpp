#include <QWidget>
#include <QBoxLayout>
#include <QLabel>

#include "AppConfigPage_Synchro.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;


AppConfigPage_Synchro::AppConfigPage_Synchro(QWidget *parent) : ConfigPage(parent)
{
    qDebug() << "Create synchro config page";

    synchroCommand=new QLineEdit(this);
    synchroCommand->setText(mytetraConfig.get_synchrocommand());
    synchroCommand->setCursorPosition(0);


    synchroOnStartup=new QCheckBox(this);
    synchroOnStartup->setText(tr("Synchronize at MyTetra startup"));
    synchroOnStartup->setChecked(mytetraConfig.get_synchroonstartup());

    synchroOnExit=new QCheckBox(this);
    synchroOnExit->setText(tr("Synchronize when exit from MyTetra"));
    synchroOnExit->setChecked(mytetraConfig.get_synchroonexit());


    // Собирается основной слой
    QVBoxLayout *central_layout=new QVBoxLayout();

    // Область ввода команды синхронизации
    QLabel *commandText=new QLabel(this);
    commandText->setText(tr("Synchronization command"));

    QLabel *commandAboutText=new QLabel(this);
    commandAboutText->setText(tr("Use <b>%a</b> macro for get database directory path"));
    commandAboutText->setWordWrap(true);

    central_layout->addWidget(commandText);
    central_layout->addWidget(synchroCommand);
    central_layout->addWidget(commandAboutText);



    // Группировщик виджетов для настройки автоматического старта синхронизации
    synchroOnBox=new QGroupBox(this);
    synchroOnBox->setTitle(tr("Automatic start synchronization"));

    // Виджеты вставляются в группировщик
    QVBoxLayout *synchroOnLayout = new QVBoxLayout;
    synchroOnLayout->addWidget(synchroOnStartup);
    synchroOnLayout->addWidget(synchroOnExit);
    synchroOnBox->setLayout(synchroOnLayout);

    central_layout->addWidget(synchroOnBox);

    central_layout->addStretch();

    // Основной слой устанавливается
    setLayout(central_layout);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Synchro::apply_changes(void)
{
    qDebug() << "Apply changes synchro";

// Сохраняется строка с командой синхронизации
    if(mytetraConfig.get_synchrocommand()!=synchroCommand->text())
        mytetraConfig.set_synchrocommand(synchroCommand->text());

// Сохраняется настройка запуска синхронизации при старте
    if(mytetraConfig.get_synchroonstartup()!=synchroOnStartup->isChecked())
        mytetraConfig.set_synchroonstartup(synchroOnStartup->isChecked());

// Сохраняется настройка запуска синхронизации при выходе
    if(mytetraConfig.get_synchroonexit()!=synchroOnExit->isChecked())
        mytetraConfig.set_synchroonexit(synchroOnExit->isChecked());

    return 0;
}
