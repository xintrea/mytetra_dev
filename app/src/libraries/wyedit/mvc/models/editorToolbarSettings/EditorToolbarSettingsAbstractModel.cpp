#include "main.h"
#include "EditorToolbarSettingsAbstractModel.h"
#include "libraries/wyedit/EditorConfig.h"


EditorToolbarSettingsAbstractModel::EditorToolbarSettingsAbstractModel(QObject *parent) :
    QStandardItemModel(parent)
{
    // Запоминается указатель на объект работы с конфиг файлом
    editorConfig = find_object<EditorConfig>("editorconfig");
}


EditorToolbarSettingsAbstractModel::~EditorToolbarSettingsAbstractModel()
{
    this->clear();
}


QVariant EditorToolbarSettingsAbstractModel::headerData(int section, Qt::Orientation orientation, int role) const
{
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


Qt::ItemFlags EditorToolbarSettingsAbstractModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}



int EditorToolbarSettingsAbstractModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED( parent )

    return 1;
}


// Нахождение индекса комманды по ее названию
QModelIndex EditorToolbarSettingsAbstractModel::findCommand(const QString &command, bool toLower)
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


QString EditorToolbarSettingsAbstractModel::getCommandsList() const
{
    QStringList commandsList;

    for (int i=0; i<this->rowCount(QModelIndex()); ++i) {
        commandsList.append( this->data( this->index(i, 0) ).toString());
    }

    return commandsList.join(',');
}

