#include "EditorToolbarUsedCommandsView.h"


EditorToolbarUsedCommandsView::EditorToolbarUsedCommandsView(QWidget *parent) : QTreeView(parent)
{

}


EditorToolbarUsedCommandsView::~EditorToolbarUsedCommandsView()
{

}

// Инициализация представления
void EditorToolbarUsedCommandsView::init()
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
