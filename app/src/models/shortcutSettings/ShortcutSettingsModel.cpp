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

            index=this->index(j, 3, sectionIndex);
            this->setData(index, copyShortcutManager.getDefaultKeySequenceAsText(sectionName+"-"+actionName));

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
    this->copyShortcutManager.save();
}


void ShortcutSettingsModel::updateShortcutManager()
{
    this->smartUpdate(updateMode::updateManager);
}


void ShortcutSettingsModel::resetAllShortcutsToDefault()
{
    this->smartUpdate(updateMode::resetToDefaultKeySequence);
}


bool ShortcutSettingsModel::checkShortcutDuplicate()
{
    return this->smartUpdate(updateMode::checkDuplicate);
}


bool ShortcutSettingsModel::smartUpdate(updateMode mode)
{
    // Перебор секций
    for(int i=0; i<this->rowCount( QModelIndex() ); ++i ) {
        QModelIndex sectionIndex=this->index(i, 0);

        // Имя секции
        QString sectionName=this->data( sectionIndex ).toString();

        // Перебор команд в секции
        for(int j=0; j<this->rowCount( sectionIndex ); ++j ) {

            // Короткое имя команды
            QModelIndex commandIndex=this->index(j, 0, sectionIndex);
            QString commandName=this->data( commandIndex ).toString();

            // Полное имя команды
            QString fullCommandName=sectionName+"-"+commandName;

            // Сочетание клавиш
            QModelIndex keysIndex=this->index(j, 2, sectionIndex);
            QString keysName=this->data( keysIndex ).toString();


            // Обновление copyShortcutManager значениями из текущей модели дерева
            if(mode==updateMode::updateManager) {
                // Если сочетание клавиш у данной команды имеет другое значение
                if(this->copyShortcutManager.getKeySequence(fullCommandName).toString()!=keysName) {
                    this->copyShortcutManager.setKeySequence(fullCommandName, keysName); // Устанавливается новое сочетание клавиш
                }
            }

            // Установка в copyShortcutManager и в модели дерева стандартных клавиатурных комбинаций
            if(mode==updateMode::resetToDefaultKeySequence) {

                QModelIndex defaultKeysIndex=this->index(j, 3, sectionIndex);
                QString defaultKeysName=this->data( defaultKeysIndex ).toString();

                // Если сочетание клавиш у данной команды не соответствует стандартному
                if(keysName!=defaultKeysName) {
                    this->setData( keysIndex, defaultKeysName); // Устанавливается в дереве
                    this->copyShortcutManager.setKeySequence(fullCommandName, defaultKeysName); // Устанавливается в менеджере
                }
            }

            // Проверка модели на повторы сочетаний клавиш
            if(mode==updateMode::checkDuplicate) {

                // Список для сочетаний клавиш и их названий
                static QMap<QString, QString> keysSequenceList;
                if(i==0 && j==0)
                {
                    keysSequenceList.clear();
                    duplicateError="";
                }

                // Если сочетание не пустое, его можно обрабатывать
                if( keysName!="" )
                {
                    // Если сочетания нет в списке, оно добавляется в список
                    if( !keysSequenceList.contains( keysName ) )
                    {
                        keysSequenceList[keysName]=fullCommandName;
                    }
                    else
                    {
                        // Иначе сочетание повторяется
                        QString altrnateFullCommandName=keysSequenceList[keysName];

                        QString sectionNameA=fullCommandName.left( fullCommandName.indexOf('-') );
                        QString sectionNameB=altrnateFullCommandName.left( altrnateFullCommandName.indexOf('-') );

                        // Проверка, допустимо ли повторение сочетания
                        if( !copyShortcutManager.isOverloadEnable(sectionNameA, sectionNameB) )
                        {
                            duplicateError=tr("Found duplicate key sequense <b>%3</b> for action <b>%1</b> and <b>%2</b>").arg(altrnateFullCommandName).arg(fullCommandName).arg(keysName);
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}


// Получение заголовка столбца
QVariant ShortcutSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

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

        if(section==3) {
            return tr("Default Shortcut"); // Скрытый столбец
        }

    }

    return QVariant();
}


int ShortcutSettingsModel::columnCount(const QModelIndex &itemIndex) const
{
    Q_UNUSED(itemIndex)

    return 4;
}


Qt::ItemFlags ShortcutSettingsModel::flags(const QModelIndex &index) const
{
    // Для разделов
    if (!index.isValid())
      return Qt::ItemIsEnabled;

    // Для строк с шорткатами
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


const QString &ShortcutSettingsModel::getDuplicateError() const
{
    return duplicateError;
}

