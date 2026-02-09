#include <QWidget>
#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>

#include "AppConfigPage_Appearance.h"

#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "libraries/wyedit/EditorShowTextDispatcher.h"
#include "libraries/helpers/CssHelper.h"
#include "libraries/MtComboBox.h"


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

    this->setupThemeComboBox();
    this->setupIconSizeComboBox();


    // Настройка запуска MyTetra в свернутом окне
    runInMinimizedWindow=new QCheckBox(this);
    runInMinimizedWindow->setText(tr("Run MyTetra in a minimized window"));
    runInMinimizedWindow->setChecked( mytetraConfig.get_runinminimizedwindow() );


    // Настройка поведения открепляемых окон, как они будут сворачиваться и разворачитьваться
    dockableWindowsBehavior=new QCheckBox(this);
    dockableWindowsBehavior->setText(tr("Hide detached windows if close main window"));
    dockableWindowsBehavior->setChecked( mytetraConfig.getDockableWindowsBehavior()=="together" );
}


// Выбор темы интерфейса
void AppConfigPage_Appearance::setupThemeComboBox()
{
    themeLabel=new QLabel(this);
    themeLabel->setText(tr("Interface theme"));

    themeNameComboBox=new MtComboBox(this);

    // Размер списка
    // themeNameComboBox->setMinimumContentsLength( 10 );

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

        themeNameComboBox->addItem(translateName);

        count++;
        if ( name == mytetraConfig.getInterfaceTheme() )
        {
            currentIndex = count;
        }
    }

    themeNameComboBox->setCurrentIndex( currentIndex );

}


// Выбор размера иконок
void AppConfigPage_Appearance::setupIconSizeComboBox()
{
    iconSizeLabel=new QLabel(this);
    iconSizeLabel->setText(tr("Icon size"));

    iconSizeComboBox=new MtComboBox(this);

    // Первый элемент - системный размер (индекс 0)
    iconSizeComboBox->addItem( tr("System") );

    // Какая строка будет выбрана как текущий размер
    int count = 0; // Здесь счет с нуля, чтобы после count++ получился индекс 1
    int currentIndex = -1;

    // Перебираются имена названий размеров иконок
    for ( auto sizeName : fixedParameters.interfaceIconSizeAvailableMap.keys() )
    {
        // В первом аргументе значения QMap-таблицы хранится человеко-читаемое название
        QString name = fixedParameters.interfaceIconSizeAvailableMap.at( sizeName ).first;

        // Добавляется элемент
        iconSizeComboBox->addItem( name );

        count++;
        if ( sizeName == mytetraConfig.getInterfaceIconSize() )
        {
            currentIndex = count;
        }
    }

    if ( currentIndex==-1 or mytetraConfig.getInterfaceIconSize()=="" )
    {
        currentIndex = 0; // Будет выбран первый элемент
    }

    iconSizeComboBox->setCurrentIndex( currentIndex );
}


void AppConfigPage_Appearance::setupSignals()
{

}


void AppConfigPage_Appearance::assembly()
{
    // Выбор темы
    QHBoxLayout *themeLayout=new QHBoxLayout();
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeNameComboBox);
    themeLayout->addStretch();

    // Выбор размера иконок
    QHBoxLayout *iconSizeLayout=new QHBoxLayout();
    iconSizeLayout->addWidget(iconSizeLabel);
    iconSizeLayout->addWidget(iconSizeComboBox);
    iconSizeLayout->addStretch();

    QVBoxLayout *interfaceLayout = new QVBoxLayout;
    interfaceLayout->addLayout(themeLayout);
    interfaceLayout->addLayout(iconSizeLayout);

    // Группировщик настроек интерфейса
    interfaceBox=new QGroupBox(this);
    interfaceBox->setTitle(tr("Interface"));
    interfaceBox->setLayout(interfaceLayout);


    // Группировщик виджетов настройки поведения окна
    behaviorBox=new QGroupBox(this);
    behaviorBox->setTitle(tr("Windows behavior"));

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

    // Если была изменена тема
    if ( mytetraConfig.getInterfaceTheme() !=
         fixedParameters.themesAvailableList[ themeNameComboBox->currentIndex() ] )
    {
        if ( themeNameComboBox->currentText()!="Unknown")
        {
            mytetraConfig.setInterfaceTheme(
                fixedParameters.themesAvailableList[ themeNameComboBox->currentIndex() ] );

            CssHelper::loadCurrentTheme();
        }
    }


    // Если был изменен размер иконок
    QString iconSizeName;

    // Для номеров с индексом 1 и выше выставляется имя размера иконок
    // Если индекс -1 (не был сделан выбор) - имя размера останется пустой строкой
    // Если индекс 0 (это будет System) - имя размера тоже останется пустой строкой
    if ( iconSizeComboBox->currentIndex()>0 )
    {
        iconSizeName = fixedParameters.interfaceIconSizeAvailableMap.keys()[ iconSizeComboBox->currentIndex()-1 ];
    }

    // Обнаружено изменение размера иконок
    if ( mytetraConfig.getInterfaceIconSize() != iconSizeName )
    {
        mytetraConfig.setInterfaceIconSize( iconSizeName );

        // Для обновления размера иконок перечитывается тема оформления
        CssHelper::loadCurrentTheme();
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
