#include <QDebug>

#include "EditorToolbarSettingsAvailableToolsModel.h"

#include "main.h"
#include "views/record/MetaEditor.h"
#include "libraries/wyedit/EditorToolBarAssistant.h"
#include "libraries/wyedit/EditorConfig.h"
#include "libraries/ShortcutManager.h"
#include "libraries/helpers/ObjectHelper.h"

extern ShortcutManager shortcutManager;


EditorToolbarSettingsAvailableToolsModel::EditorToolbarSettingsAvailableToolsModel(QObject *parent) :
    EditorToolbarSettingsAbstractModel(parent)
{

}


void EditorToolbarSettingsAvailableToolsModel::init()
{
    // Вначале модель полностью очищается, чтобы небыло наложений от предыдущих открытий окна настроек
    this->clear();

    // Все уже используемые инструменты на обоих линиях панели
    QStringList commandsInToolsLine = editorConfig->get_tools_line_1().split(',');
    commandsInToolsLine.append(editorConfig->get_tools_line_2().split(','));

    // Список названий всех контролов (команд) панелей инструментов
    EditorToolBarAssistant *editorToolBarAssistant = find_object<MetaEditor>("editorScreen")->editorToolBarAssistant;
    QStringList controlsNameList = editorToolBarAssistant->getCommandNameList();
    for (int i=0; i!=controlsNameList.size(); ++i) {
        QString command = controlsNameList.at(i);

        qDebug() << "Add available command " << command;

        // Для десктопной версии пропускаем кнопки, нужные для мобильной версии
        // todo: подумать, а надо ли это действие
        if (editorToolBarAssistant->getViewMode() == Editor::WYEDIT_DESKTOP_MODE && (command=="back" || command=="findInBase")) {
            continue;
        }

        // Добавление только тех команд, которые отсутствуют на обоих панелях инструментов
        if (commandsInToolsLine.indexOf(command)==-1) {
            QStandardItem *item=new QStandardItem(command);
            item->setIcon( editorToolBarAssistant->getIcon(command) );
            item->setData( command, Qt::UserRole );
            item->setData( shortcutManager.getDescription( "editor-"+command), Qt::DisplayRole );
            this->appendRow(item); // Элемент отдается во владение модели
        }
    }

    // Создание неудаляемого элемента <Разделитель>
    QStandardItem *separatorItem=new QStandardItem();
    separatorItem->setData( "separator", Qt::UserRole );
    separatorItem->setData( tr("<Separator>"), Qt::DisplayRole );
    this->insertRow(0, separatorItem); // Элемент вставляется первым и отдается во владение модели

    emit layoutChanged();
}
