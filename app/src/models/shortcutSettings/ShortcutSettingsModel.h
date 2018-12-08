#ifndef SHORTCUTSETTINGSMODEL_H
#define SHORTCUTSETTINGSMODEL_H

#include <QObject>
#include <QStandardItemModel>

#include "libraries/ShortcutManager.h"

// Модель таблицы с настройками горячих клавиш


class ShortcutSettingsModel : public QStandardItemModel
{
public:
    ShortcutSettingsModel(QObject *parent = nullptr);
    ~ShortcutSettingsModel();

    QModelIndex findShortcut(const QString &shortcutFullName);

    void save();

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    int columnCount(const QModelIndex &itemIndex = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

protected:

    void updateShortcutManager();

    // Все настройки производятся над копией ShortcutManager, чтобы их можно было отменить
    ShortcutManager copyShortcutManager;

};

#endif // SHORTCUTSETTINGSMODEL_H
