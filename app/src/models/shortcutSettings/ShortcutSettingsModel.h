#ifndef SHORTCUTSETTINGSMODEL_H
#define SHORTCUTSETTINGSMODEL_H

#include <QObject>
#include <QStandardItemModel>


class ShortcutSettingsModel : public QStandardItemModel
{
public:
    ShortcutSettingsModel(QObject *parent = nullptr);
    ~ShortcutSettingsModel();

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    int columnCount(const QModelIndex &itemIndex = QModelIndex()) const;


//    QVariant data(const QModelIndex &index, int role) const;
//    QVariant headerData(int section, Qt::Orientation orientation,
//                        int role = Qt::DisplayRole) const;

//    int rowCount(const QModelIndex &itemIndex = QModelIndex()) const;


//    Qt::ItemFlags flags(const QModelIndex &index) const;
//    bool setData(const QModelIndex &index, const QVariant &value,
//                 int role = Qt::EditRole);
};

#endif // SHORTCUTSETTINGSMODEL_H
