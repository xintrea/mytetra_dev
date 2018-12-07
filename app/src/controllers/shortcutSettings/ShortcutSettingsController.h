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

    QStringList getShortcutData(const QModelIndex &index);

public slots:

    void setShortcut(QString shortcutFullName, QString sequenceText);
    void resetShortcutToDefault(QString shortcutFullName);
    void resetAllShortcutsToDefault();

protected:

    ShortcutSettingsView *view;
    ShortcutSettingsModel *model;

};

#endif // SHORTCUTSETTINGSCONTROLLER_H
