#include "ShortcutSettingsView.h"

#include "main.h"
#include "views/mainWindow/MainWindow.h"


ShortcutSettingsView::ShortcutSettingsView(QWidget *parent) : QTreeView(parent)
{
    int dialogWidth=int( 0.7 * static_cast<double>(find_object<MainWindow>("mainwindow")->width()) );
    int dialogHeight=int( 0.7 * static_cast<double>(find_object<MainWindow>("mainwindow")->height()) );
    this->setMinimumWidth( dialogWidth );
    this->setMinimumHeight( dialogHeight );
    this->resize( this->size() ); // Обновление размеров виджета
}


ShortcutSettingsView::~ShortcutSettingsView()
{

}
