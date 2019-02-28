#ifndef EDITORTOOLBARALLBUTTONSVIEW
#define EDITORTOOLBARALLBUTTONSVIEW

#include <QTreeView>

/********************************************************************************************/
/* Представление для модели всех доступных команд для панелей инструментов редактора текста */
/********************************************************************************************/

class EditorToolbarAvailableCommandsView : public QTreeView
{
    Q_OBJECT
public:
    EditorToolbarAvailableCommandsView(QWidget *parent=nullptr);
    ~EditorToolbarAvailableCommandsView();

    // Инициализация представления
    void init();

};

#endif // EDITORTOOLBARALLBUTTONSVIEW
