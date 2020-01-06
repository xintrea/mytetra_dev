#include "EditorToolbarSettingsUsedToolsModel.h"

#include "main.h"
#include "libraries/wyedit/EditorConfig.h"
#include "libraries/wyedit/EditorToolBarAssistant.h"
#include "views/record/MetaEditor.h"
#include "libraries/ShortcutManager.h"

extern ShortcutManager shortcutManager;
extern GlobalParameters globalParameters;


EditorToolbarSettingsUsedToolsModel::EditorToolbarSettingsUsedToolsModel(QObject *parent) :
    EditorToolbarSettingsAbstractModel(parent)
{

}


void EditorToolbarSettingsUsedToolsModel::init(GlobalParameters::EditorToolbar tb)
{
    // Вначале модель полностью очищается, чтобы небыло наложений от предыдущих открытий окна настроек
    this->clear();

    // Создание команд для панели с учетом номера панели
    QStringList commandsInToolsLine = tb == GlobalParameters::EditorToolbar::First
            ? editorConfig->get_tools_line_1().split(',')
            : editorConfig->get_tools_line_2().split(',');

    EditorToolBarAssistant *editorToolBarAssistant = find_object<MetaEditor>("editorScreen")->editorToolBarAssistant;

    for (int i=0; i!=commandsInToolsLine.size(); ++i) {

        QString command=commandsInToolsLine[i];
        QStandardItem *newItem = new QStandardItem();

        newItem->setData(command, Qt::UserRole);

        if(command=="separator") {
            newItem->setData( tr("<Separator>"), Qt::DisplayRole );
        }
        else
        {
            newItem->setData( shortcutManager.getDescription( "editor-"+command ), Qt::DisplayRole );
        }

        newItem->setIcon( editorToolBarAssistant->getIcon(command) );

        this->appendRow(newItem); // Владение элементом передается модели
    }

    emit layoutChanged();
}
