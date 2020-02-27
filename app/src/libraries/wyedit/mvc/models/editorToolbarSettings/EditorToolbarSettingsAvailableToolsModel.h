#ifndef EDITORTOOLBARALLBUTTONSMODEL_H
#define EDITORTOOLBARALLBUTTONSMODEL_H

#include <QObject>
#include <QStandardItemModel>

#include "EditorToolbarSettingsAbstractModel.h"

// Модель списка всех доступных инструментов редактора


class EditorToolbarSettingsAvailableToolsModel : public EditorToolbarSettingsAbstractModel
{
    Q_OBJECT

public:

    EditorToolbarSettingsAvailableToolsModel(QObject *parent = nullptr);

    // Первичное наполнение модели
    void init();

};

#endif // EDITORTOOLBARALLBUTTONSMODEL_H
