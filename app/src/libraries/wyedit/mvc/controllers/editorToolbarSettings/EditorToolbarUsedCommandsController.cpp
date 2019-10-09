#include <QMessageBox>

#include "EditorToolbarUsedCommandsController.h"

extern GlobalParameters globalParameters;

EditorToolbarUsedCommandsController::EditorToolbarUsedCommandsController(GlobalParameters::EditorToolbar tb, QObject *parent) : QObject(parent)
{
    // указание на обрабатываемую панель
    this->tb = tb;
}


EditorToolbarUsedCommandsController::~EditorToolbarUsedCommandsController()
{

}

// Инициализация представления
void EditorToolbarUsedCommandsController::init()
{
    // Создается вид со списком кнопок для ToolBar 1
    view = new EditorToolbarCommandsListView( qobject_cast<QWidget *>(parent()) );
    view->setObjectName("editorToolbarUsedCommandsView");

    // Создание модели данных
    model = new EditorToolbarUsedCommandsModel(tb, this);
    model->setObjectName("editorToolbarUsedCommandsModel");
    view->setModel(model);
    view->init();

    // Выбор первого элемента модели
    QStandardItem *item = model->item(0,0);
    if (item->index().isValid()) {
        // Выделение первого элемента модели
        this->getSelectionModel()->select(
              item->index(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows
        );

        // Фокус на первом элементе модели
        this->getView()->setCurrentIndex(item->index());
    }
}

// Получение представления
EditorToolbarCommandsListView *EditorToolbarUsedCommandsController::getView() const
{
    return view;
}

// Получение модели
EditorToolbarUsedCommandsModel *EditorToolbarUsedCommandsController::getModel() const
{
    return model;
}

// Получение SelectionModel
QItemSelectionModel *EditorToolbarUsedCommandsController::getSelectionModel() const
{
    return view->selectionModel();
}

// Перемещение выбранной команды в моделе команд выбранной рабочей панели инструментов
void EditorToolbarUsedCommandsController::moveCommandUpDown(CommandMove direction)
{
    // Получение строки выбранной команды
    int selectedRow = this->getSelectionModel()->currentIndex().row();

    // Проверка, выбран ли хоть один элемент списка
    if (selectedRow == -1 ) {
        QMessageBox msbox;
        msbox.setText(tr("Select the command from the toolbar commands list."));
        msbox.setIcon(QMessageBox::Icon::Warning);
        msbox.setStandardButtons(QMessageBox::Ok);
        msbox.exec();
        return;
    } else {
        // Перемещение, если есть куда перемещать
        if (direction==CommandMove::Up) {
            // Перемещение выделенной команды Вверх
            if (selectedRow != 0) {
                moveCommandUpDown(direction, selectedRow);
            }
        } else {
            // Перемещение выделенной команды Вниз
            if (selectedRow != this->getModel()->rowCount()-1) {
                moveCommandUpDown(direction, selectedRow);
            }
        }
    }
}

// Перемещение выбранной команды в моделе команд выбранной рабочей панели инструментов
void EditorToolbarUsedCommandsController::moveCommandUpDown(CommandMove direction, int selectedRow)
{
    // Перемещение выделенной команды
    QList<QStandardItem*> takedItemList = this->getModel()->takeRow(selectedRow);
    this->getModel()->insertRow(selectedRow+direction, takedItemList);

    // Выделение перемещенного элемента
    this->getSelectionModel()->select(
                takedItemList[0]->index(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows
    );

    // Фокус на перемещенный элемент
    this->getView()->setCurrentIndex(takedItemList[0]->index());
}
