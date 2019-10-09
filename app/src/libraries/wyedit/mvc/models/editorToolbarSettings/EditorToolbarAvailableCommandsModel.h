#ifndef EDITORTOOLBARALLBUTTONSMODEL_H
#define EDITORTOOLBARALLBUTTONSMODEL_H

#include <QObject>
#include <QStandardItemModel>

// Модель списка всех доступных инструментов редактора


class EditorConfig;

class EditorToolbarAvailableCommandsModel : public QStandardItemModel
{
    Q_OBJECT

public:

    EditorToolbarAvailableCommandsModel(QObject *parent = nullptr);
    ~EditorToolbarAvailableCommandsModel();

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    int columnCount(const QModelIndex &itemIndex = QModelIndex()) const;

    // Нахождение индекса комманды
    QModelIndex findCommand(const QString &command, bool toLower = false);

    // Данные модели (команды) в виде строки с разделителем-запятая
    QString getCommandsList() const;

    // Редактор конфигурации
    EditorConfig *getEditorConfig() const;

protected:

    // Создание модели
    void createModel();

    EditorConfig *editorConfig; // Редактор конфигурационного файла

};

#endif // EDITORTOOLBARALLBUTTONSMODEL_H
