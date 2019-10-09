#include "EditorToolbarUsedCommandsModel.h"

#include "main.h"
#include "libraries/wyedit/EditorConfig.h"
#include "libraries/wyedit/EditorToolBarAssistant.h"
#include "views/record/MetaEditor.h"


extern GlobalParameters globalParameters;

EditorToolbarUsedCommandsModel::EditorToolbarUsedCommandsModel(GlobalParameters::EditorToolbar tb, QObject *parent) : QStandardItemModel(parent)
{
    // Указатель на объект работы с конфиг файлом
    editorConfig = find_object<EditorConfig>("editorconfig");

    // Создание модели
    createModel(tb);
}


EditorToolbarUsedCommandsModel::~EditorToolbarUsedCommandsModel()
{
    this->clear();
}


// Заголовок столбца
QVariant EditorToolbarUsedCommandsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

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


Qt::ItemFlags EditorToolbarUsedCommandsModel::flags(const QModelIndex &itemIndex) const
{
    if (!itemIndex.isValid()) {
        return Qt::ItemIsEnabled;
    } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}


int EditorToolbarUsedCommandsModel::columnCount(const QModelIndex &itemIndex) const
{
    Q_UNUSED(itemIndex)

    return 1;
}


// Нахождение индекса комманды
QModelIndex EditorToolbarUsedCommandsModel::findCommand(const QString &command, bool toLower)
{
    // Поиск команды
    for (int i=0; i<this->rowCount(QModelIndex()); ++i ) {
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
QString EditorToolbarUsedCommandsModel::getCommandsList() const
{
    QStringList commandsList;
    for (int i=0; i<this->rowCount(QModelIndex()); ++i) {
        commandsList.append(this->data( this->index(i, 0) ).toString());
    }
    return commandsList.join(',');
}

// Редактор конфигурации
EditorConfig *EditorToolbarUsedCommandsModel::getEditorConfig() const
{
    return editorConfig;
}

// Создание модели
void EditorToolbarUsedCommandsModel::createModel(GlobalParameters::EditorToolbar tb)
{
    // Создание команд для панели с учетом номера панели
    QStringList commandsInToolsLine = tb == GlobalParameters::EditorToolbar::First
            ? editorConfig->get_tools_line_1().split(',')
            : editorConfig->get_tools_line_2().split(',');

    EditorToolBarAssistant *editorToolBarAssistant = find_object<MetaEditor>("editorScreen")->editorToolBarAssistant;

    for (int i=0; i!=commandsInToolsLine.size(); ++i) {
        QString command(commandsInToolsLine[i]);
        QStandardItem *newItem = new QStandardItem();
        newItem->setData(command, Qt::DisplayRole);
        newItem->setIcon( editorToolBarAssistant->getIcon(command) );
        this->appendRow(newItem); // Владение элементом передается модели
    }
    emit layoutChanged();
}
