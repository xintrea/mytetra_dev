#ifndef SHORTCUTSETTINGSSCREEN_H
#define SHORTCUTSETTINGSSCREEN_H

#include <QDialog>
#include <QWidget>
#include <QAction>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "controllers/shortcutSettings/ShortcutSettingsController.h"

class ShortcutSettingsScreen : public QDialog
{
    Q_OBJECT
public:
    ShortcutSettingsScreen(QWidget *parent = nullptr);
    ~ShortcutSettingsScreen();

protected:

    ShortcutSettingsController *shortcutSettingsController;

    QPushButton *buttonGrabShortcut;
    QPushButton *buttonResetShortcutToDefault;
    QPushButton *buttonResetAllShortcutsToDefault;

    QDialogButtonBox *dialogButtonBox;

    QVBoxLayout *screenLayout;

    void setupUI(void);
    void setupSignals(void);
    void assembly(void);

};

#endif // SHORTCUTSETTINGSSCREEN_H
