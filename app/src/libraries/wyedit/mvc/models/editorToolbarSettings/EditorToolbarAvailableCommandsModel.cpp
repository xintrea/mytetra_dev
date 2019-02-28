#include "EditorToolbarAvailableCommandsModel.h"

#include "main.h"
#include "views/record/MetaEditor.h"
#include "libraries/wyedit/EditorToolBarAssistant.h"
#include "libraries/wyedit/EditorConfig.h"

EditorToolbarAvailableCommandsModel::EditorToolbarAvailableCommandsModel(QObject *parent) : QStandardItemModel(parent)
{
    // Указатель на объект работы с конфиг файлом
    editorConfig = find_object<EditorConfig>("editorconfig");

    // Создание модели
    createModel();
}


EditorToolbarAvailableCommandsModel::~EditorToolbarAvailableCommandsModel()
{
    this->clear();
}


// Получение заголовка столбца
QVariant EditorToolbarAvailableCommandsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {

        if (section == 0) {
            return tr("Command");
        }

    }

    return QVariant();
}


Qt::ItemFlags EditorToolbarAvailableCommandsModel::flags(const QModelIndex &index) const
{
    // Для родительских веток
    if (!index.isValid())
      return Qt::ItemIsEnabled;

    // Для строк с командами
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


int EditorToolbarAvailableCommandsModel::columnCount(const QModelIndex &itemIndex) const
{
    Q_UNUSED(itemIndex);

    return 1;
}

// Нахождение индекса комманды
QModelIndex EditorToolbarAvailableCommandsModel::findCommand(const QString &command, bool toLower)
{
    // Поиск команды
    for (int i=0; i<this->rowCount( QModelIndex() ); ++i ) {
        QModelIndex commandIndex = this->index(i, 0);
        if (toLower) {
            // Если надо сравнивать с учетом регистра
            if (this->data( commandIndex ).toString().toLower() == command.toLower()) {
                return commandIndex;
            }
        } else {
            // Если надо сравнивать без учета регистра
            if (this->data( commandIndex ).toString() == command) {
                return commandIndex;
            }
        }
    }

    return QModelIndex(); // Возвращается пустой индекс
}

// Данные модели (команды) в виде строки с разделителем-запятая
QString EditorToolbarAvailableCommandsModel::getCommandsList() const
{
    QStringList commandsList;
    for (int i=0; i<this->rowCount(QModelIndex()); ++i) {
        commandsList.append(this->data( this->index(i, 0) ).toString());
    }
    return commandsList.join(',');
}

// Редактор конфигурации
EditorConfig *EditorToolbarAvailableCommandsModel::getEditorConfig() const
{
    return editorConfig;
}

// Создание модели
void EditorToolbarAvailableCommandsModel::createModel()
{
    // Все используемые коммнды на 2-х панелях инструментов
    QStringList commandsInToolsLine = editorConfig->get_tools_line_1().split(',');
    commandsInToolsLine.append(editorConfig->get_tools_line_2().split(','));

    // Список названий всех контролов (команд) панелей инструментов
    EditorToolBarAssistant *editorToolBarAssistant = find_object<MetaEditor>("editorScreen")->editorToolBarAssistant;
    QStringList *commandNameList = editorToolBarAssistant->getCommandNameList();
    for (int i=0; i!=commandNameList->size(); ++i) {
        QString command = commandNameList->at(i);
        // Для десктопной версии пропускаем кнопки, нужные для мобильной версии
        if (editorToolBarAssistant->getViewMode() == Editor::WYEDIT_DESKTOP_MODE && (command=="back" || command=="find_in_base")) {
            continue;
        }
        if (commandsInToolsLine.indexOf(command)==-1) {
            // Добавление только тех команд, которые отсутствуют на обоих панелях инструментов
            QList<QStandardItem *> items;
            items.append(new QStandardItem(command)); // Комманда
            this->appendRow(items);
        }
    }

    // Создание неудаляемого элемента <РАЗДЕЛИТЕЛЬ>
    this->insertRow(0, new QStandardItem(tr("<SEPARATOR>")));

    emit layoutChanged();
}
