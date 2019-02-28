#ifndef EDITORTOOLBARALLBUTTONSCONTROLLER
#define EDITORTOOLBARALLBUTTONSCONTROLLER

#include <QObject>

#include "views/editorToolbar/EditorToolbarAvailableCommandsView.h"
#include "models/editorToolbar/EditorToolbarAvailableCommandsModel.h"

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
    EditorToolbarAvailableCommandsView *getView() const;

    // Получение модели
    EditorToolbarAvailableCommandsModel *getModel() const;

    // Получение SelectionModel
    QItemSelectionModel *getSelectionModel() const;

protected:

    EditorToolbarAvailableCommandsView *view;
    EditorToolbarAvailableCommandsModel *model;

};

#endif // EDITORTOOLBARALLBUTTONSCONTROLLER
