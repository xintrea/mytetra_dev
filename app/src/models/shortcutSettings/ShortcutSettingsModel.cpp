#include "ShortcutSettingsModel.h"


extern ShortcutManager shortcutManager;


ShortcutSettingsModel::ShortcutSettingsModel(QObject *parent) : QStandardItemModel(parent)
{
    copyShortcutManager=shortcutManager;

    // Создание разделов
    int i=0;
    foreach(QString sectionName, copyShortcutManager.availableSection) {
        QStandardItem *sectionItem=new QStandardItem(sectionName);
        this->appendRow(sectionItem);

        // Индекс только что созданного раздела шорткатов
        QModelIndex sectionIndex=this->index(i, 0); // Индекс элемента раздела

        // Создание строк с шорткатами
        this->insertRows(0, copyShortcutManager.getActionsNameList(sectionName).size(), sectionIndex);
        this->insertColumns(0, this->columnCount(), sectionIndex);

        // Заполнение строк с шорткатами
        int j=0;
        foreach(QString actionName, copyShortcutManager.getActionsNameList(sectionName) ) {
            QModelIndex index=this->index(j, 0, sectionIndex);
            this->setData(index, actionName);

            index=this->index(j, 1, sectionIndex);
            this->setData(index, copyShortcutManager.getDescription(sectionName+"-"+actionName));

            index=this->index(j, 2, sectionIndex);
            this->setData(index, copyShortcutManager.getKeySequenceAsText(sectionName+"-"+actionName));

            ++j;
        }

        ++i;
    }

}


ShortcutSettingsModel::~ShortcutSettingsModel()
{
    this->clear();
}


// Индекс первой ячейки с записью о шорткате
QModelIndex ShortcutSettingsModel::findShortcut(const QString &shortcutFullName)
{
    QStringList chunk=shortcutFullName.split("-");
    QString shortcutSection=chunk[0];
    QString shortcutCommand=chunk[1];

    // Поиск секции
    for(int i=0; i<this->rowCount( QModelIndex() ); ++i ) {
        QModelIndex sectionIndex=this->index(i, 0);

        // Если секция найдена
        if( this->data( sectionIndex ).toString()==shortcutSection) {

            // Поиск команды
            for(int j=0; j<this->rowCount( sectionIndex ); ++j ) {
                QModelIndex commandIndex=this->index(j, 0, sectionIndex);

                // Если команда найдена
                if(this->data( commandIndex ).toString()==shortcutCommand) {
                    return commandIndex;
                }
            }
        }
    }

    return QModelIndex(); // Возвращается пустой индекс
}


void ShortcutSettingsModel::save()
{
    this->updateShortcutManager();
    copyShortcutManager.save();
}


// Обновление значений из таблицы в copyShortcutManager
void ShortcutSettingsModel::updateShortcutManager()
{
    // Перебор секций
    for(int i=0; i<this->rowCount( QModelIndex() ); ++i ) {
        QModelIndex sectionIndex=this->index(i, 0);

        // Имя секции
        QString sectionName=this->data( sectionIndex ).toString();

        // Перебор команд
        for(int j=0; j<this->rowCount( sectionIndex ); ++j ) {

            // Имя команды
            QModelIndex commandIndex=this->index(j, 0, sectionIndex);
            QString commandName=this->data( commandIndex ).toString();

            // Сочетание клавиш
            QModelIndex keysIndex=this->index(j, 2, sectionIndex);
            QString keysName=this->data( keysIndex ).toString();

            QString fullCommandName=sectionName+"-"+commandName;

            // Если сочетание клавиш у данной команды имеет другое значение
            if(copyShortcutManager.getKeySequence(fullCommandName).toString()!=keysName) {
                copyShortcutManager.setKeySequence(fullCommandName, keysName); // Устанавливается новое сочетание клавиш
            }
        }
    }
}


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

