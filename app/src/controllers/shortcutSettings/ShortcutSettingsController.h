#ifndef SHORTCUTSETTINGSCONTROLLER_H
#define SHORTCUTSETTINGSCONTROLLER_H

#include <QObject>
#include "views/shortcutSettings/ShortcutSettingsView.h"
#include "models/shortcutSettings/ShortcutSettingsModel.h"

class ShortcutSettingsController : public QObject
{
    Q_OBJECT
public:
    ShortcutSettingsController(QObject *parent = nullptr);
    ~ShortcutSettingsController();

    void init();
    void applyChanges();

    ShortcutSettingsView* getView();

protected:

    ShortcutSettingsView *view;
    ShortcutSettingsModel *model;

};

#endif // SHORTCUTSETTINGSCONTROLLER_H
