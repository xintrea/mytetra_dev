#include "ShortcutSettingsModel.h"
#include "libraries/ShortcutManager.h"

extern ShortcutManager shortcutManager;


ShortcutSettingsModel::ShortcutSettingsModel(QObject *parent) : QStandardItemModel(parent)
{
    // Создание разделов
    foreach(QString sectionName, shortcutManager.availableSection) {
        QStandardItem *sectionItem=new QStandardItem(sectionName);
        this->appendRow(sectionItem);
    }
}

ShortcutSettingsModel::~ShortcutSettingsModel()
{

}


//// Получение данных из модели
//QVariant ShortcutSettingsModel::data(const QModelIndex &index, int role) const
//{
//    // Если индекс невалиден, возвращается несуществующий элемент
//    if(!index.isValid()) {
//        return QVariant();
//    }

//    // Если запрашивается окраска текста элемента
//    // if(role==Qt::ForegroundRole)
//    // {
//    //     TreeItem *item = getItem(index);

//    //     if(item->recordtableGetRowCount()>0)
//    //         return QColor(Qt::black);// Если узел содержит таблицу конечных записей
//    //     else
//    //         return QColor(Qt::darkGray); // Ветка без таблицы конечных записей
//    // }


//    // Окраска заднего фона
//    if(role==Qt::BackgroundRole)
//    {
//        // Сделать выделенный фон для групп из шорткатов
//        // if(index==cursorOverIndex)
//        //   return QColor(Qt::gray);

//        if(index.row()%2==1) {
//            return QColor(Qt::gray);
//        }
//    }


//    // Если запрашивается содержимое текста элемента
//    if(role==Qt::DisplayRole || role== Qt::EditRole)
//    {
//     return QVariant( QString("Shortcut") ); // Запрашивается строка имени с количеством элементов
//    }

//    // Все прочие случаи
//    return QVariant();
//}


//// Получение заголовка столбца
//QVariant ShortcutSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    Q_UNUSED(orientation);
//    Q_UNUSED(role);

//    if(section==0) {
//        return tr("Command");
//    }

//    if(section==1) {
//        return tr("Name");
//    }

//    if(section==2) {
//        return tr("Shortcut");
//    }

//    return "";
//}


//int ShortcutSettingsModel::rowCount(const QModelIndex &itemIndex) const
//{
//    return 10;
//}


//int ShortcutSettingsModel::columnCount(const QModelIndex &itemIndex) const
//{
//    return 3;
//}


//Qt::ItemFlags ShortcutSettingsModel::flags(const QModelIndex &index) const
//{
//    if (!index.isValid())
//      return Qt::ItemIsEnabled;

//    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
//}


//bool ShortcutSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{

//}

