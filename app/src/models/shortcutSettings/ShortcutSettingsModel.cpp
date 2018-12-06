#include "ShortcutSettingsModel.h"
#include "libraries/ShortcutManager.h"

extern ShortcutManager shortcutManager;


ShortcutSettingsModel::ShortcutSettingsModel(QObject *parent) : QStandardItemModel(parent)
{
    // Создание разделов
    int i=0;
    foreach(QString sectionName, shortcutManager.availableSection) {
        QStandardItem *sectionItem=new QStandardItem(sectionName);
        this->appendRow(sectionItem);

        // Индекс только что созданного раздела шорткатов
        QModelIndex sectionIndex=this->index(i, 0); // Индекс элемента раздела

        // Создание строк с шорткатами
        this->insertRows(0, shortcutManager.getActionsNameList(sectionName).size(), sectionIndex);
        this->insertColumns(0, this->columnCount(), sectionIndex);

        // Заполнение строк с шорткатами
        int j=0;
        foreach(QString actionName, shortcutManager.getActionsNameList(sectionName) ) {
            QModelIndex index=this->index(j, 0, sectionIndex);
            this->setData(index, actionName);

            index=this->index(j, 1, sectionIndex);
            this->setData(index, shortcutManager.getDescription(sectionName+"-"+actionName));

            index=this->index(j, 2, sectionIndex);
            this->setData(index, shortcutManager.getKeySequenceAsText(sectionName+"-"+actionName));

            ++j;
        }

        ++i;
    }

}


ShortcutSettingsModel::~ShortcutSettingsModel()
{
    this->clear();
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


// Получение заголовка столбца
QVariant ShortcutSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if(role!=Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation==Qt::Horizontal) {

        if(section==0) {
            return tr("Command");
        }

        if(section==1) {
            return tr("Name");
        }

        if(section==2) {
            return tr("Shortcut");
        }
    }

    return QVariant();
}


int ShortcutSettingsModel::columnCount(const QModelIndex &itemIndex) const
{
    Q_UNUSED(itemIndex);

    return 3;
}


Qt::ItemFlags ShortcutSettingsModel::flags(const QModelIndex &index) const
{
    // Для разделов
    if (!index.isValid())
      return Qt::ItemIsEnabled;

    // Для строк с шорткатами
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


//bool ShortcutSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{

//}

