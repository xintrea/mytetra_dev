#include "ShortcutSettingsController.h"


ShortcutSettingsController::ShortcutSettingsController(QObject *parent) : QObject(parent)
{

}


ShortcutSettingsController::~ShortcutSettingsController()
{

}


void ShortcutSettingsController::init()
{
    view->setModel(model);
}


void ShortcutSettingsController::applyChanges()
{

}


ShortcutSettingsView *ShortcutSettingsController::getView()
{
    return view;
}
