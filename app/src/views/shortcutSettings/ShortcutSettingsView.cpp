#include "ShortcutSettingsView.h"

#include "main.h"
#include "views/mainWindow/MainWindow.h"


ShortcutSettingsView::ShortcutSettingsView(QWidget *parent) : QTreeView(parent)
{
    int dialogWidth=int( 0.64 * static_cast<double>(find_object<MainWindow>("mainwindow")->width()) );
    int dialogHeight=int( 0.5 * static_cast<double>(find_object<MainWindow>("mainwindow")->height()) );
    this->setMinimumWidth( dialogWidth );
    this->setMinimumHeight( dialogHeight );
    this->resize( this->size() ); // Обновление размеров виджета
}


ShortcutSettingsView::~ShortcutSettingsView()
{

}


// Инит должен вызываться после установки модели,
// так как this->selectionModel() не определен если не установлена модель
void ShortcutSettingsView::init()
{
    connect(this->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &ShortcutSettingsView::onCurrentChanged);
}


void ShortcutSettingsView::onCurrentChanged(const QModelIndex &index, const QModelIndex &prevIndex)
{
    Q_UNUSED(prevIndex);

    // Курсор должен всегда перемещаться по левым ячейкам, так как если пользователь кликнет
    // на не самую левую ячейку, то курсор сможет перемещаться только в пределах группы
    // а нужно чтобы перемещение было возможно по всему дереву
    this->setCurrentIndex( index.siblingAtColumn(0) );
}
