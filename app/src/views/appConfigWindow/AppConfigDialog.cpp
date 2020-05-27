#include <QStackedWidget>
#include <QObject>
#include <QDialog>
#include <QDebug>

#include "main.h"
#include "ConfigDialog.h"
#include "AppConfigDialog.h"
#include "AppConfigPage_Main.h"
#include "AppConfigPage_Crypt.h"
#include "AppConfigPage_Misc.h"
#include "AppConfigPage_Synchro.h"
#include "AppConfigPage_RecordTable.h"
#include "AppConfigPage_Attach.h"
#include "AppConfigPage_Keyboard.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;


AppConfigDialog::AppConfigDialog(const QString &firstPageName, QWidget *parent)
{
    // Создание диалога с parent обеспечивает отображение иконки приложения
    // в диалоговом окне и на панели задач
    configDialog=new ConfigDialog( parent );
    configDialog->setAttribute(Qt::WA_DeleteOnClose);
    configDialog->setWindowTitle(QObject::tr("MyTetra settings"));

    if(mytetraConfig.getInterfaceMode()=="mobile")
    {
        qDebug() << "Screen size X Y: " << getScreenSizeX() << getScreenSizeY();
        configDialog->setMinimumSize(getScreenSizeX(), getScreenSizeY());
        configDialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }

    // Наполнение диалога виджетами
    // В качестве родителя надо указывать parent а не configDialog (разобраться почему)
    pageMain       =configDialog->addWidget(new AppConfigPage_Main( parent ),
                                            QObject::tr("Main"));
    pageCrypt      =configDialog->addWidget(new AppConfigPage_Crypt( parent ),
                                            QObject::tr("Crypt"));
    pageSynchro    =configDialog->addWidget(new AppConfigPage_Synchro( parent ),
                                            QObject::tr("Synchro"));
    pageRecordTable=configDialog->addWidget(new AppConfigPage_RecordTable( parent ),
                                            QObject::tr("Note area"));
    pageAttach     =configDialog->addWidget(new AppConfigPage_Attach( parent ),
                                            QObject::tr("Attaches"));
    pageKeyboard   =configDialog->addWidget(new AppConfigPage_Keyboard( parent ),
                                            QObject::tr("Keyboard"));
    pageMisc       =configDialog->addWidget(new AppConfigPage_Misc( parent ),
                                            QObject::tr("Misc"));


    configDialog->updateListWidth();

    // Если указано имя виджета настроек, происходит переключение на него,
    // иначе будет выбран первый виджет (pageMain)
    if(firstPageName.size()>0)
        changePage(firstPageName);
}


AppConfigDialog::~AppConfigDialog()
{
    qDebug() << Q_FUNC_INFO;
}


void AppConfigDialog::exec()
{
    qDebug() << "Before config dialog exec";
    configDialog->exec();
    qDebug() << "Config dialog exec finish";
}


// Переход на нужную страницу настроек (переход совершается кодом извне)
// Данный метод нужен для возможности вызова конкретного окна настроек из любого места программы
void AppConfigDialog::changePage(QString name)
{
    QListWidgetItem *item=nullptr;

    if(name=="pageMain") item=pageMain;
    if(name=="pageCrypt") item=pageCrypt;
    if(name=="pageSynchro") item=pageSynchro;
    if(name=="pageRecordTable") item=pageRecordTable;
    if(name=="pageAttach") item=pageAttach;
    if(name=="pageKeyboard") item=pageKeyboard;
    if(name=="pageMisc") item=pageMisc;

    if(item!=nullptr)
        configDialog->externalChangePage(item);
    else
        qDebug() << "AppConfigDialog::changePage cant find item for name: " << name;
}
