#ifndef EDITORTOOLBARALLBUTTONSCONTROLLER
#define EDITORTOOLBARALLBUTTONSCONTROLLER

#include <QObject>

#include "../../views/editorToolbarSettings/EditorToolbarCommandsListView.h"
#include "../../models/editorToolbarSettings/EditorToolbarAvailableCommandsModel.h"

/*********************************************************************************************************/
/* Контроллер для представления и модели всех доступных команд для панелей инструментов редактора текста */
/*********************************************************************************************************/

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
    EditorToolbarAvailableCommandsModel *getModel() const;

    // Получение SelectionModel
    QItemSelectionModel *getSelectionModel() const;

protected:

    EditorToolbarCommandsListView *view;
    EditorToolbarAvailableCommandsModel *model;

};

#endif // EDITORTOOLBARALLBUTTONSCONTROLLER
