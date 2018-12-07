#include "ShortcutSettingsController.h"


ShortcutSettingsController::ShortcutSettingsController(QObject *parent) : QObject(parent)
{

}


ShortcutSettingsController::~ShortcutSettingsController()
{

}


void ShortcutSettingsController::init()
{
    // Создается область с деревом настроек клавиатурных комбинаций
    view=new ShortcutSettingsView( qobject_cast<QWidget *>(parent()) ); // Вид размещается внутри виджета Screen
    view->setObjectName("shortcutSettingsView");
    // view->setController(this);

    // Создание модели данных
    model=new ShortcutSettingsModel(this);
    model->setObjectName("shortcutSettingsModel");

    view->setModel(model);
    view->expandAll();
    view->resizeColumnToContents(0);
    view->resizeColumnToContents(1);
    view->resizeColumnToContents(2);
}


void ShortcutSettingsController::applyChanges()
{

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

    return shortcutData;
}


void ShortcutSettingsController::setShortcut(QString shortcutFullName, QString sequenceText)
{

}


void ShortcutSettingsController::resetShortcutToDefault(QString shortcutFullName)
{

}


void ShortcutSettingsController::resetAllShortcutsToDefault()
{

}
