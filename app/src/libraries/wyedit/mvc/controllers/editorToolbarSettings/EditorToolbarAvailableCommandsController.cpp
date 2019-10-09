#include "EditorToolbarAvailableCommandsController.h"


EditorToolbarAvailableCommandsController::EditorToolbarAvailableCommandsController(QObject *parent) : QObject(parent)
{

}


EditorToolbarAvailableCommandsController::~EditorToolbarAvailableCommandsController()
{

}


// Инициализация представления
void EditorToolbarAvailableCommandsController::init()
{
    // Создается вид со списком кнопок для панели всех доступных кнопок
    view = new EditorToolbarCommandsListView( qobject_cast<QWidget *>(parent()) );
    view->setObjectName("editorToolbarAvailableCommandsView");

    // Создание модели данных
    model = new EditorToolbarAvailableCommandsModel(this);
    model->setObjectName("editorToolbarAvailableCommandsModel");
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
EditorToolbarCommandsListView *EditorToolbarAvailableCommandsController::getView() const
{
    return view;
}


// Получение модели
EditorToolbarAvailableCommandsModel *EditorToolbarAvailableCommandsController::getModel() const
{
    return model;
}


// Получение SelectionModel
QItemSelectionModel *EditorToolbarAvailableCommandsController::getSelectionModel() const
{
    return view->selectionModel();
}
