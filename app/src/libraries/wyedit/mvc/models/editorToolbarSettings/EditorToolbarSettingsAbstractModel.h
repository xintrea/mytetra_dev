#ifndef EDITORTOOLBARSETTINGSABSTRACTMODEL_H
#define EDITORTOOLBARSETTINGSABSTRACTMODEL_H

#include <QObject>
#include <QStandardItemModel>

// Универсальная модель для хранения левого и правого списка инструментов
// в окне настроек панели инструметов редактора


class EditorConfig;

class EditorToolbarSettingsAbstractModel : public QStandardItemModel
{
    Q_OBJECT

public:
    EditorToolbarSettingsAbstractModel(QObject *parent = nullptr);
    virtual ~EditorToolbarSettingsAbstractModel();

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    int columnCount(const QModelIndex &parent) const;

    // Нахождение индекса комманды по ее названию
    QModelIndex findCommand(const QString &command, bool toLower = false);

    // Получение всех данных модели (т. е. списка имен команд)
    // в виде строки с разделителем-запятая
    QString getCommandsList() const;

protected:

    EditorConfig *editorConfig; // Конфигурация редактора

};

#endif // EDITORTOOLBARSETTINGSABSTRACTMODEL_H
