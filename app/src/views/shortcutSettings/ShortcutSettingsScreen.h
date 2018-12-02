#ifndef SHORTCUTSETTINGSSCREEN_H
#define SHORTCUTSETTINGSSCREEN_H

#include <QDialog>
#include <QWidget>

class ShortcutSettingsScreen : public QDialog
{
    Q_OBJECT
public:
    ShortcutSettingsScreen(QWidget *parent = nullptr);
    ~ShortcutSettingsScreen();


};

#endif // SHORTCUTSETTINGSSCREEN_H
