#include "EditorToolbarAvailableCommandsView.h"


EditorToolbarAvailableCommandsView::EditorToolbarAvailableCommandsView(QWidget *parent) : QTreeView(parent)
{

}


EditorToolbarAvailableCommandsView::~EditorToolbarAvailableCommandsView()
{

}

// Инициализация представления
void EditorToolbarAvailableCommandsView::init()
{
    this->setAlternatingRowColors(true);
    this->setAutoFillBackground(true);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->resizeColumnToContents(0);

    // Обновление размеров виджета
    this->setMinimumWidth( this->columnWidth(0) + 30 );
    this->resize(this->size());
}
