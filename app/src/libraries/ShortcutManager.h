#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QKeySequence>
#include <QMap>
#include <QSettings>

class ShortcutManager
{
public:
    ShortcutManager();

    void init();

    QKeySequence getKeySequence(QString actionName);

protected:

    void initDefaultKeyTable();
    void checkConfigFile();
    void saveConfig(QMap<QString, QKeySequence> table);

    QMap<QString, QKeySequence> keyTable;
    QMap<QString, QKeySequence> defaultKeyTable;

    QString configFileName;


};

#endif // SHORTCUTMANAGER_H
