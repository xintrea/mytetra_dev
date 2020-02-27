#ifndef EDITORTOOLBARCOMMANDSLISTVIEW
#define EDITORTOOLBARCOMMANDSLISTVIEW

#include <QTreeView>


// Список команд для строки инструментов редактора
// Используется и для списка доступных инструментов,
// и для списка инструментов на конкретной линии панели инструментов


class EditorToolbarCommandsListView : public QTreeView
{
    Q_OBJECT
public:
    EditorToolbarCommandsListView(QWidget *parent=nullptr);
    ~EditorToolbarCommandsListView();

    // Инициализация представления
    void init();

};

#endif // EDITORTOOLBARCOMMANDSLISTVIEW
