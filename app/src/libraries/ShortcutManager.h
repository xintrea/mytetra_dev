#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QObject>
#include <QKeySequence>
#include <QMap>
#include <QSettings>
#include <QAction>
#include <QToolButton>


class ShortcutManager : public QObject
{
    Q_OBJECT

public:
    ShortcutManager(QObject *parent=nullptr);
    virtual ~ShortcutManager();

    // Оператор копирования нужен из-за того, что в диалоге настройки шорткатов
    // используется копия шорткат менеджера, чтобы настройки можно было отменить.
    // А в QObject по-умолчнию оператор копирования отключен
    ShortcutManager &operator=(const ShortcutManager &obj);

    enum stringRepresentation {
        plain=0,
        brackets=1
    };

    struct Data {
        QKeySequence sequence;
        QString description; // Описание (короткая строка)
        QString explanation; // Пояснение (длинная строка)
    };

    static const QStringList availableSection;

    void init();
    void save();
    void updateShortcutInApplication();

    QKeySequence getKeySequence(QString actionName);
    void setKeySequence(QString actionName, QString keySequence);
    QString getDescription(QString actionName);
    QString getExplanation(QString actionName);

    QString getDescriptionWithShortcut(QString actionName);
    QString getFullDescription(QString actionName);
    QString getKeySequenceAsText(QString actionName, stringRepresentation mode=stringRepresentation::plain);

    QKeySequence getDefaultKeySequence(QString actionName);
    QString getDefaultKeySequenceAsText(QString actionName);

    void initAction(QString actionName, QAction *action);
    void initToolButton(QString actionName, QToolButton *action);

    // Получение списка коротких имен действий в секции
    QStringList getActionsNameList(QString sectionName);

signals:

    void updateWidgetShortcut();

protected:

    void initDefaultKeyTable();
    void initKeyTable();
    void checkConfigFile();
    void saveConfig(QMap<QString, Data> table);

    QMap<QString, Data> keyTable;
    QMap<QString, Data> defaultKeyTable;

    QString configFileName;
};

#endif // SHORTCUTMANAGER_H
