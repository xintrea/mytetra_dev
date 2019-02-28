#ifndef EDITORTOOLBARUSEDTOOLBUTTONSMODEL
#define EDITORTOOLBARUSEDTOOLBUTTONSMODEL

#include <QObject>
#include <QStandardItemModel>

#include "libraries/GlobalParameters.h"

/****************************************************************************************/
/* Модель списка используемых команд для выбранной панели инструментов редактора текста */
/****************************************************************************************/

class EditorConfig;

class EditorToolbarUsedCommandsModel : public QStandardItemModel
{
    Q_OBJECT

public:

    EditorToolbarUsedCommandsModel(GlobalParameters::EditorToolbar tb, QObject *parent = nullptr);
    ~EditorToolbarUsedCommandsModel();

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &itemIndex) const;

    int columnCount(const QModelIndex &itemIndex = QModelIndex()) const;

    // Нахождение индекса комманды
    QModelIndex findCommand(const QString &command, bool toLower = false);

    // Данные модели (команды) в виде строки с разделителем-запятая
    QString getCommandsList() const;

    // Редактор конфигурации
    EditorConfig *getEditorConfig() const;

private:

    // Создание модели
    void createModel(GlobalParameters::EditorToolbar tb);

    EditorConfig *editorConfig; // Редактор конфигурационного файла

};

#endif // EDITORTOOLBARUSEDTOOLBUTTONSMODEL
