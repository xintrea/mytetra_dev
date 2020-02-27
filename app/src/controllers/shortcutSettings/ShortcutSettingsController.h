#ifndef SHORTCUTSETTINGSCONTROLLER_H
#define SHORTCUTSETTINGSCONTROLLER_H

#include <QObject>
#include "views/shortcutSettings/ShortcutSettingsView.h"
#include "models/shortcutSettings/ShortcutSettingsModel.h"

class ShortcutSettingsController : public QObject
{
    Q_OBJECT
public:

    struct ShortcutData {
        QString section;
        QString command;
        QString description;
        QString keys;
        QString defaultKeys;
    };

    ShortcutSettingsController(QObject *parent = nullptr);
    ~ShortcutSettingsController();

    void init();
    void applyChanges();

    ShortcutSettingsView* getView();

    ShortcutData getShortcutData(const QModelIndex &index);
    ShortcutData getEmptyShortcutData();

    void setShortcut(QString shortcutFullName, QString sequenceText);
    void resetAllShortcutsToDefault();

protected:

    ShortcutSettingsView *view;
    ShortcutSettingsModel *model;

};

#endif // SHORTCUTSETTINGSCONTROLLER_H
