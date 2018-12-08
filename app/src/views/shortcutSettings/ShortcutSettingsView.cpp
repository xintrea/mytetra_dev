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

    connect(this, &ShortcutSettingsView::currentChanged, // Исправить на сигнал (это слот)
            this, &ShortcutSettingsView::onCurrentChanged);
}


ShortcutSettingsView::~ShortcutSettingsView()
{

}


void ShortcutSettingsView::onCurrentChanged(const QModelIndex &index, const QModelIndex &prevIndex)
{
    Q_UNUSED(prevIndex);

    qDebug() << "Replace cursor position to 0 row";

    // this->selectionModel()->select( index.siblingAtRow(0), QItemSelectionModel::Clear );

    this->setCurrentIndex( index.siblingAtRow(0) );
}
