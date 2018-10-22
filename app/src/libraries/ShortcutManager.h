#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QKeySequence>
#include <QMap>
#include <QSettings>
#include <QAction>


class ShortcutManager
{
public:
    ShortcutManager();

    enum stringRepresentation {
        plain=0,
        brackets=1
    };

    struct data {
        QKeySequence sequence;
        QString description; // Описание (короткая строка)
        QString explanation; // Пояснение (длинная строка)
    };

    static const QStringList availableSection;

    void init();
    QKeySequence getKeySequence(QString actionName);
    QString getDescription(QString actionName);
    QString getExplanation(QString actionName);

    QString getDescriptionWithShortcut(QString actionName);
    QString getFullDescription(QString actionName);
    QString getKeySequenceAsText(QString actionName, stringRepresentation mode=stringRepresentation::plain);

    void initAction(QString actionName, QAction *action);

protected:

    void initDefaultKeyTable();
    void initKeyTable();
    void checkConfigFile();
    void saveConfig(QMap<QString, data> table);

    QMap<QString, data> keyTable;
    QMap<QString, data> defaultKeyTable;

    QString configFileName;


};

#endif // SHORTCUTMANAGER_H
