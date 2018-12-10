#include "ShortcutSettingsController.h"
#include "libraries/ShortcutManager.h"

extern ShortcutManager shortcutManager;


ShortcutSettingsController::ShortcutSettingsController(QObject *parent) : QObject(parent)
{

}


ShortcutSettingsController::~ShortcutSettingsController()
{

}


void ShortcutSettingsController::init()
{
    // Создается вид с деревом настроек клавиатурных комбинаций
    view=new ShortcutSettingsView( qobject_cast<QWidget *>(parent()) ); // Вид размещается внутри виджета Screen
    view->setObjectName("shortcutSettingsView");
    // view->setController(this);

    // Создание модели данных
    model=new ShortcutSettingsModel(this);
    model->setObjectName("shortcutSettingsModel");

    view->setModel(model);
    view->init(); // Инит вида должен вызываться после установки модели
}


void ShortcutSettingsController::applyChanges()
{
    // Сохранение изменений на диск
    model->save();

    // Переинициализация менеджера горячих клавиш
    shortcutManager.init();

    // Переинициализация всех шорткатов в программе
    shortcutManager.updateShortcutInApplication();

    // Закрытие окна настроек горячих клавиш
    static_cast<QWidget *>( parent() )->close();
}


ShortcutSettingsView *ShortcutSettingsController::getView()
{
    return view;
}


ShortcutSettingsController::ShortcutData ShortcutSettingsController::getShortcutData(const QModelIndex &index)
{
    ShortcutData shortcutData;

    shortcutData.section    =model->data( index.parent() ).toString();
    shortcutData.command    =model->data( index.siblingAtColumn(0) ) .toString();
    shortcutData.description=model->data( index.siblingAtColumn(1) ) .toString();
    shortcutData.keys       =model->data( index.siblingAtColumn(2) ) .toString();
    shortcutData.defaultKeys=model->data( index.siblingAtColumn(3) ) .toString();

    return shortcutData;
}


ShortcutSettingsController::ShortcutData ShortcutSettingsController::getEmptyShortcutData()
{
    ShortcutData shortcutData;

    shortcutData.section    ="";
    shortcutData.command    ="";
    shortcutData.description="";
    shortcutData.keys       ="";
    shortcutData.defaultKeys="";

    return shortcutData;
}


void ShortcutSettingsController::setShortcut(QString shortcutFullName, QString sequenceText)
{
    QModelIndex index=model->findShortcut( shortcutFullName );

    model->setData( index.siblingAtColumn(2), sequenceText );
}


void ShortcutSettingsController::resetAllShortcutsToDefault()
{
    model->resetAllShortcutsToDefault();
}

