#ifndef EDITORTOOLBARUSEDTOOLBUTTONSCONTROLLER
#define EDITORTOOLBARUSEDTOOLBUTTONSCONTROLLER

#include <QObject>

#include "../../views/editorToolbarSettings/EditorToolbarUsedCommandsView.h"
#include "../../models/editorToolbarSettings/EditorToolbarUsedCommandsModel.h"
#include "libraries/GlobalParameters.h"

/***********************************************************************************************************************/
/* Контроллер для представления и модели списка используемых команд для выбранной панели инструментов редактора текста */
/***********************************************************************************************************************/

class EditorToolbarUsedCommandsController : public QObject
{
    Q_OBJECT

public:

    EditorToolbarUsedCommandsController(GlobalParameters::EditorToolbar tb, QObject *parent = nullptr);
    ~EditorToolbarUsedCommandsController();

    // Направление перемещения команды панели используемых комманд
    enum CommandMove {
        Up = -1,
        Down = 1
    };

    // Инициализация представления
    void init();

    // Получение представления
    EditorToolbarUsedCommandsView *getView() const;

    // Получение модели
    EditorToolbarUsedCommandsModel *getModel() const;

    // Получение SelectionModel
    QItemSelectionModel *getSelectionModel() const;

    // Перемещение выбранной команды в моделе команд выбранной рабочей панели инструментов
    void moveCommandUpDown(CommandMove direction);

protected:

    GlobalParameters::EditorToolbar tb; // Указание на обрабатываемую панель

    EditorToolbarUsedCommandsView *view;
    EditorToolbarUsedCommandsModel *model;

private:

    // Перемещение выбранной команды в моделе команд выбранной рабочей панели инструментов
    void moveCommandUpDown(CommandMove direction, int selectedRow);
};

#endif // EDITORTOOLBARUSEDTOOLBUTTONSCONTROLLER
