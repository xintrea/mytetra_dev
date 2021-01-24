#ifndef EDITORTOOLBARALLBUTTONSCONTROLLER
#define EDITORTOOLBARALLBUTTONSCONTROLLER

#include <QObject>

#include "../../views/editorToolbarSettings/EditorToolbarCommandsListView.h"
#include "../../models/editorToolbarSettings/EditorToolbarSettingsAvailableToolsModel.h"

// Контроллер для представления и модели всех доступных
// инструментов редактора текста


class EditorToolbarAvailableCommandsController : public QObject
{
    Q_OBJECT

public:

    EditorToolbarAvailableCommandsController(QObject *parent = nullptr);
    ~EditorToolbarAvailableCommandsController();

    // Инициализация представления
    void init();

    // Получение представления
    EditorToolbarCommandsListView *getView() const;

    // Получение модели
    EditorToolbarSettingsAvailableToolsModel *getModel() const;

    // Получение SelectionModel
    QItemSelectionModel *getSelectionModel() const;

protected:

    EditorToolbarCommandsListView *view;
    EditorToolbarSettingsAvailableToolsModel *model;

};

#endif // EDITORTOOLBARALLBUTTONSCONTROLLER
