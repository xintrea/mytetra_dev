#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QKeySequence>
#include <QMap>

class ShortcutManager
{
public:
    ShortcutManager();

    void init();

    QKeySequence getKeySequence(QString actionName);

protected:

    void initDefaultKeyTable();

    QMap<QString, QKeySequence> keyTable;
    QMap<QString, QKeySequence> defaultKeyTable;

};

#endif // SHORTCUTMANAGER_H
