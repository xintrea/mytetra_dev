#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QKeySequence>
#include <QMap>
#include <QSettings>

class ShortcutManager
{
public:
    ShortcutManager();

    enum stringRepresentation {
        plain=0,
        brackets=1
    };

    static const QStringList availableSection;

    void init();
    QKeySequence getKeySequence(QString actionName);
    QString getKeySequenceHumanReadable(QString actionName, stringRepresentation mode=stringRepresentation::plain);

protected:

    void initDefaultKeyTable();
    void initKeyTable();
    void checkConfigFile();
    void saveConfig(QMap<QString, QKeySequence> table);

    QMap<QString, QKeySequence> keyTable;
    QMap<QString, QKeySequence> defaultKeyTable;

    QString configFileName;


};

#endif // SHORTCUTMANAGER_H
