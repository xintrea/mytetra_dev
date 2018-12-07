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


QStringList ShortcutSettingsController::getShortcutData(const QModelIndex &index)
{
    QString shortcutSectionName=model->data( index.parent() ).toString();

    QString shortcutName       =model->data( index.siblingAtColumn(0) ) .toString();
    QString shortcutDescription=model->data( index.siblingAtColumn(1) ) .toString();
    QString shortcutKeys       =model->data( index.siblingAtColumn(2) ) .toString();

    QStringList result={ shortcutSectionName,
                         shortcutName,
                         shortcutDescription,
                         shortcutKeys };

    return result;
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
