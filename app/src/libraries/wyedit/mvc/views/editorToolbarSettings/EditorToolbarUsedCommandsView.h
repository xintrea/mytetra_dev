#ifndef EDITORTOOLBARUSEDTOOLBUTTONSVIEW
#define EDITORTOOLBARUSEDTOOLBUTTONSVIEW

#include <QTreeView>

/**********************************************************************************************************/
/* Представление для модели списка используемых команд для выбранной панели инструментов редактора текста */
/**********************************************************************************************************/

class EditorToolbarUsedCommandsView : public QTreeView
{
    Q_OBJECT
public:
    EditorToolbarUsedCommandsView(QWidget *parent=nullptr);
    ~EditorToolbarUsedCommandsView();

    // Инициализация представления
    void init();

};

#endif // EDITORTOOLBARUSEDTOOLBUTTONSVIEW
