#ifndef SHORTCUTSETTINGSCONTROLLER_H
#define SHORTCUTSETTINGSCONTROLLER_H

#include <QObject>

class ShortcutSettingsController : QObject
{
    Q_OBJECT
public:
    ShortcutSettingsController(QObject *parent = nullptr);
    ~ShortcutSettingsController();

};

#endif // SHORTCUTSETTINGSCONTROLLER_H
