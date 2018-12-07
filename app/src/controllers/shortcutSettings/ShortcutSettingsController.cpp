#include "ShortcutSettingsController.h"


ShortcutSettingsController::ShortcutSettingsController(QObject *parent) : QObject(parent)
{

}


ShortcutSettingsController::~ShortcutSettingsController()
{

}


void ShortcutSettingsController::init()
{
    // Создается вид с деревом настроек клавиатурных комбинаций
    view=new ShortcutSettingsView( qobject_cast<QWidget *>(parent()) ); // Вид размещается внутри виджета Screen
    view->setObjectName("shortcutSettingsView");
    // view->setController(this);

    // Создание модели данных
    model=new ShortcutSettingsModel(this);
    model->setObjectName("shortcutSettingsModel");

    view->setModel(model);
    view->expandAll();
    view->resizeColumnToContents(0);
    view->resizeColumnToContents(1);
    view->resizeColumnToContents(2);
}


void ShortcutSettingsController::applyChanges()
{

}


ShortcutSettingsView *ShortcutSettingsController::getView()
{
    return view;
}


ShortcutSettingsController::ShortcutData ShortcutSettingsController::getShortcutData(const QModelIndex &index)
{
    ShortcutData shortcutData;

    shortcutData.section    =model->data( index.parent() ).toString();
    shortcutData.command    =model->data( index.siblingAtColumn(0) ) .toString();
    shortcutData.description=model->data( index.siblingAtColumn(1) ) .toString();
    shortcutData.keys       =model->data( index.siblingAtColumn(2) ) .toString();

    return shortcutData;
}


void ShortcutSettingsController::setShortcut(QString shortcutFullName, QString sequenceText)
{
    QModelIndex index=findShortcut( shortcutFullName );

    model->setData( index.siblingAtColumn(2), sequenceText );
}


void ShortcutSettingsController::resetShortcutToDefault(QString shortcutFullName)
{

}


void ShortcutSettingsController::resetAllShortcutsToDefault()
{

}


// Индекс первой ячейки с записью о шорткате
QModelIndex ShortcutSettingsController::findShortcut(const QString &shortcutFullName)
{
    QStringList chunk=shortcutFullName.split("-");
    QString shortcutSection=chunk[0];
    QString shortcutCommand=chunk[1];

    // Поиск секции
    for(int i=0; i<model->rowCount( QModelIndex() ); ++i ) {
        QModelIndex sectionIndex=model->index(i, 0);

        // Если секция найдена
        if( model->data( sectionIndex ).toString()==shortcutSection) {

            // Поиск команды
            for(int j=0; j<model->rowCount( sectionIndex ); ++j ) {
                QModelIndex commandIndex=model->index(j, 0, sectionIndex);

                // Если команда найдена
                if(model->data( commandIndex ).toString()==shortcutCommand) {
                    return commandIndex;
                }
            }
        }
    }

    return QModelIndex(); // Возвращается пустой ииндекс
}
