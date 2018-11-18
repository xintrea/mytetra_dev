#include <QFile>
#include <QDebug>

#include "main.h"
#include "ShortcutManager.h"

#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


const QStringList ShortcutManager::availableSection={"note", "tree", "editor", "actionLog", "attach", "misc"};


ShortcutManager::ShortcutManager()
{

}


void ShortcutManager::init()
{
    configFileName=globalParameters.getWorkDirectory()+"/shortcut.ini";

    initDefaultKeyTable();

    checkConfigFile();

    initKeyTable();
}


void ShortcutManager::initDefaultKeyTable()
{
    defaultKeyTable.clear();

    defaultKeyTable.insert("note-addNewToEnd",  data{ QKeySequence("Ctrl+Alt+N"), QObject::tr("Add a new note"), QObject::tr("") });
    defaultKeyTable.insert("note-addNewBefore", data{ QKeySequence("Ctrl+Alt+J"), QObject::tr("Add a note before"), QObject::tr("Add a note before current selected note") });
    defaultKeyTable.insert("note-addNewAfter",  data{ QKeySequence("Ctrl+Alt+M"), QObject::tr("Add a note after"), QObject::tr("Add a note after current selected note") });
    defaultKeyTable.insert("note-editField",    data{ QKeySequence("Ctrl+Alt+E"), QObject::tr("Edit properties"), QObject::tr("Edit note properties (name, author, tags...)") });
    defaultKeyTable.insert("note-block",        data{ QKeySequence("Ctrl+Alt+B"), QObject::tr("Block/Unblock note"), QObject::tr("Block or unblock current selected note") });
    defaultKeyTable.insert("note-delete",       data{ QKeySequence("Ctrl+Alt+R"), QObject::tr("Delete note(s)"), QObject::tr("") });
    defaultKeyTable.insert("note-cut",          data{ QKeySequence("Ctrl+Alt+X"), QObject::tr("Cut notes(s)"), QObject::tr("Cut notes(s) to clipboard") });
    defaultKeyTable.insert("note-copy",         data{ QKeySequence("Ctrl+Alt+C"), QObject::tr("Copy note(s)"), QObject::tr("Copy note(s) to clipboard") });
    defaultKeyTable.insert("note-paste",        data{ QKeySequence("Ctrl+Alt+V"), QObject::tr("Paste note(s)"), QObject::tr("Paste note(s) from clipboard") });
    defaultKeyTable.insert("note-moveUp",       data{ QKeySequence("Ctrl+Alt+Up"), QObject::tr("Move up"), QObject::tr("Move up current note") });
    defaultKeyTable.insert("note-moveDn",       data{ QKeySequence("Ctrl+Alt+Down"), QObject::tr("Move down"), QObject::tr("Move down current note") });
    defaultKeyTable.insert("note-previousNote", data{ QKeySequence("Ctrl+Alt+Left"), QObject::tr("Previous note"), QObject::tr("Previous note has been viewing") });
    defaultKeyTable.insert("note-nextNote",     data{ QKeySequence("Ctrl+Alt+Right"), QObject::tr("Next note"), QObject::tr("Next note has been viewing") });

    defaultKeyTable.insert("tree-expandAllSubbranch",   data{ QKeySequence("Ctrl+Shift+8"), QObject::tr("Expand all sub items"), QObject::tr("") });
    defaultKeyTable.insert("tree-collapseAllSubbranch", data{ QKeySequence("Ctrl+Shift+9"), QObject::tr("Collapse all sub items"), QObject::tr("") });
    defaultKeyTable.insert("tree-moveUpBranch",         data{ QKeySequence("Ctrl+Shift+PgUp"), QObject::tr("Move item up"), QObject::tr("") });
    defaultKeyTable.insert("tree-moveDownBranch",       data{ QKeySequence("Ctrl+Shift+PgDown"), QObject::tr("Move item down"), QObject::tr("") });
    defaultKeyTable.insert("tree-insSubbranch",         data{ QKeySequence("Ctrl+F7"), QObject::tr("Insert a new sub item"), QObject::tr("Insert a new sub item into selected") });
    defaultKeyTable.insert("tree-insBranch",            data{ QKeySequence("F7"), QObject::tr("Insert a new sibling item"), QObject::tr("Insert a new sibling item after selected") });
    defaultKeyTable.insert("tree-editBranch",           data{ QKeySequence("Shift+F6"), QObject::tr("Edit item name"), QObject::tr("Edit name of selected item") });
    defaultKeyTable.insert("tree-delBranch",            data{ QKeySequence("F8"), QObject::tr("Delete item"), QObject::tr("Delete selected item and all sub items") });
    defaultKeyTable.insert("tree-cutBranch",            data{ QKeySequence("Ctrl+F8"), QObject::tr("Cut item"), QObject::tr("Cut item including sub items") });
    defaultKeyTable.insert("tree-copyBranch",           data{ QKeySequence("F3"), QObject::tr("Copy item"), QObject::tr("Copy item including sub items") });
    defaultKeyTable.insert("tree-pasteBranch",          data{ QKeySequence("F5"), QObject::tr("Paste item"), QObject::tr("Paste sibling item after selected") });
    defaultKeyTable.insert("tree-pasteSubbranch",       data{ QKeySequence("Ctrl+F5"), QObject::tr("Paste as sub item"), QObject::tr("Paste item as sub item for selected") });
    defaultKeyTable.insert("tree-encryptBranch",        data{ QKeySequence("Ctrl+Shift+1"), QObject::tr("Encrypt item"), QObject::tr("Encrypt item and all subitem") });
    defaultKeyTable.insert("tree-decryptBranch",        data{ QKeySequence("Ctrl+Shift+0"), QObject::tr("Decrypt item"), QObject::tr("Decrypt item and all subitem") });
    defaultKeyTable.insert("tree-setIcon",              data{ QKeySequence("Ctrl+Shift+O"), QObject::tr("Set icon"), QObject::tr("Set item icon") });

    defaultKeyTable.insert("editor-selectAll",           data{ QKeySequence("Ctrl+A"), QObject::tr("Select all"), QObject::tr("") });
    defaultKeyTable.insert("editor-copy",                data{ QKeySequence("Ctrl+C"), QObject::tr("Copy"), QObject::tr("") });
    defaultKeyTable.insert("editor-paste",               data{ QKeySequence("Ctrl+V"), QObject::tr("Paste"), QObject::tr("") });
    defaultKeyTable.insert("editor-pasteAsPlainText",    data{ QKeySequence("Ctrl+W"), QObject::tr("Paste as plain text"), QObject::tr("") });
    defaultKeyTable.insert("editor-cut",                 data{ QKeySequence("Ctrl+X"), QObject::tr("Cut"), QObject::tr("") });
    defaultKeyTable.insert("editor-undo",                data{ QKeySequence("Ctrl+Z"), QObject::tr("Undo"), QObject::tr("") });
    defaultKeyTable.insert("editor-redo",                data{ QKeySequence("Ctrl+Y"), QObject::tr("Redo"), QObject::tr("") });

    defaultKeyTable.insert("editor-bold",                data{ QKeySequence("Ctrl+B"), QObject::tr("Bold"), QObject::tr("") });
    defaultKeyTable.insert("editor-italic",              data{ QKeySequence("Ctrl+I"), QObject::tr("Italic"), QObject::tr("") });
    defaultKeyTable.insert("editor-underline",           data{ QKeySequence("Ctrl+U"), QObject::tr("Underline"), QObject::tr("") });
    defaultKeyTable.insert("editor-monospace",           data{ QKeySequence("Ctrl+T"), QObject::tr("Monospace"), QObject::tr("") });
    defaultKeyTable.insert("editor-code",                data{ QKeySequence("Ctrl+M"), QObject::tr("Code"), QObject::tr("Select a whole paragraphs to format text as code") });
    defaultKeyTable.insert("editor-clear",               data{ QKeySequence("Ctrl+K"), QObject::tr("Clear format"), QObject::tr("When selected whole paragraph both text and paragraph format is reset to default or just text format in other case") });
    defaultKeyTable.insert("editor-textOnly",            data{ QKeySequence("Ctrl+Shift+K"), QObject::tr("Text only"), QObject::tr("") });
    defaultKeyTable.insert("editor-fixBreakSymbol",      data{ QKeySequence("Ctrl+Shift+R"), QObject::tr("Return type replace"), QObject::tr("Replace soft carriage return to standard carriage return") });
    defaultKeyTable.insert("editor-numericList",         data{ QKeySequence("F12"), QObject::tr("Numeric list"), QObject::tr("") });
    defaultKeyTable.insert("editor-dotList",             data{ QKeySequence("Shift+F12"), QObject::tr("Marked list"), QObject::tr("") });
    defaultKeyTable.insert("editor-indentPlus",          data{ QKeySequence("Ctrl+Alt+I"), QObject::tr("Increase indent"), QObject::tr("") });
    defaultKeyTable.insert("editor-indentMinus",         data{ QKeySequence("Ctrl+Alt+U"), QObject::tr("Decrease indent"), QObject::tr("") });
    defaultKeyTable.insert("editor-alignLeft",           data{ QKeySequence("Ctrl+L"), QObject::tr("Align left"), QObject::tr("") });
    defaultKeyTable.insert("editor-alignCenter",         data{ QKeySequence("Ctrl+E"), QObject::tr("Align center"), QObject::tr("") });
    defaultKeyTable.insert("editor-alignRight",          data{ QKeySequence("Ctrl+R"), QObject::tr("Align right"), QObject::tr("") });
    defaultKeyTable.insert("editor-alignWidth",          data{ QKeySequence("Ctrl+J"), QObject::tr("Align width"), QObject::tr("") });
    defaultKeyTable.insert("editor-fontColor",           data{ QKeySequence("Ctrl+Alt+C"), QObject::tr("Text color"), QObject::tr("") });
    defaultKeyTable.insert("editor-findText",            data{ QKeySequence("Ctrl+F"), QObject::tr("Find text"), QObject::tr("Find text in current note") });
    defaultKeyTable.insert("editor-settings",            data{ QKeySequence("Ctrl+Alt+G"), QObject::tr("Editor settings"), QObject::tr("") });
    defaultKeyTable.insert("editor-reference",           data{ QKeySequence("Ctrl+Shift+U"), QObject::tr("Edit reference URL"), QObject::tr("") });
    defaultKeyTable.insert("editor-showHtml",            data{ QKeySequence("Ctrl+Shift+H"), QObject::tr("Edit HTML code"), QObject::tr("") });
    defaultKeyTable.insert("editor-showFormatting",      data{ QKeySequence("Ctrl+F10"), QObject::tr("Show special chars"), QObject::tr("") });
    defaultKeyTable.insert("editor-createTable",         data{ QKeySequence("Ctrl+F12"), QObject::tr("Create a new table"), QObject::tr("") });
    defaultKeyTable.insert("editor-insertImageFromFile", data{ QKeySequence("Ctrl+Shift+I"), QObject::tr("Insert/edit image"), QObject::tr("Insert image from file or edit image properties of selected image") });
    defaultKeyTable.insert("editor-mathExpression",      data{ QKeySequence("Ctrl+Shift+M"), QObject::tr("Insert/edit math expression"), QObject::tr("") });
    defaultKeyTable.insert("editor-expandEditArea",      data{ QKeySequence("Ctrl+Shift+E"), QObject::tr("Expand edit area"), QObject::tr("") });
    defaultKeyTable.insert("editor-expandToolsLines",    data{ QKeySequence("Ctrl+Shift+L"), QObject::tr("Expand tools"), QObject::tr("") });
    defaultKeyTable.insert("editor-save",                data{ QKeySequence("Ctrl+S"), QObject::tr("Forse save note"), QObject::tr("") });
    defaultKeyTable.insert("editor-showText",            data{ QKeySequence("Ctrl+Shift+W"), QObject::tr("Show detached window"), QObject::tr("") });
    defaultKeyTable.insert("editor-toAttach",            data{ QKeySequence("Ctrl+Shift+A"), QObject::tr("Attach files"), QObject::tr("") });
    defaultKeyTable.insert("editor-gotoReference",       data{ QKeySequence("Alt+Shift+U"), QObject::tr("Go to reference"), QObject::tr("") }); // Сочетание Alt+U перестает работать после того, как нажато на месте без URL-a

    defaultKeyTable.insert("actionLog-copy", data{ QKeySequence("Ctrl+C"), QObject::tr("Copy selected rows"), QObject::tr("") });

    defaultKeyTable.insert("attach-addAttach",        data{ QKeySequence("Ctrl+N"), QObject::tr("Attach file"), QObject::tr("") });
    defaultKeyTable.insert("attach-addAttachFromUrl", data{ QKeySequence("Ctrl+U"), QObject::tr("Attach file from URL"), QObject::tr("") });
    defaultKeyTable.insert("attach-addLink",          data{ QKeySequence("Ctrl+L"), QObject::tr("Add link"), QObject::tr("Add link without file copying") });
    defaultKeyTable.insert("attach-editFileName",     data{ QKeySequence("Ctrl+E"), QObject::tr("Edit file name"), QObject::tr("") });
    defaultKeyTable.insert("attach-deleteAttach",     data{ QKeySequence("Ctrl+D"), QObject::tr("Delete file"), QObject::tr("") });
    defaultKeyTable.insert("attach-openAttach",       data{ QKeySequence("Ctrl+O"), QObject::tr("Preview file"), QObject::tr("") });
    defaultKeyTable.insert("attach-saveAsAttach",     data{ QKeySequence("Ctrl+S"), QObject::tr("Save as..."), QObject::tr("") });
    defaultKeyTable.insert("attach-showAttachInfo",   data{ QKeySequence("Ctrl+I"), QObject::tr("Attach info"), QObject::tr("") });
    defaultKeyTable.insert("attach-switchToEditor",   data{ QKeySequence("Ctrl+Shift+A"), QObject::tr("Return to editor"), QObject::tr("") });

    defaultKeyTable.insert("misc-findInBase",  data{ QKeySequence("Ctrl+Shift+F"), QObject::tr("Find in base"), QObject::tr("") });
    defaultKeyTable.insert("misc-syncro",      data{ QKeySequence("F9"), QObject::tr("Synchronization"), QObject::tr("Run synchronization") });
    defaultKeyTable.insert("misc-editConfirm", data{ QKeySequence(Qt::CTRL + Qt::Key_Return), QObject::tr("Ok"), QObject::tr("") });
    defaultKeyTable.insert("misc-print",       data{ QKeySequence("Ctrl+P"), QObject::tr("Print"), QObject::tr("") });
    defaultKeyTable.insert("misc-exportPdf",   data{ QKeySequence("Ctrl+Shift+D"), QObject::tr("Export PDF"), QObject::tr("") });
    defaultKeyTable.insert("misc-quit",        data{ QKeySequence("Ctrl+Q"), QObject::tr("Quit"), QObject::tr("") });
}


void ShortcutManager::checkConfigFile()
{
    // Если конфиг-файла не существует, создается стандартный
    if(!QFile(configFileName).exists()) {
        saveConfig(defaultKeyTable);
    }
}


// Инициализация таблицы горячих клавиш из конфига
void ShortcutManager::initKeyTable()
{
    QSettings config(configFileName, QSettings::IniFormat);

    keyTable.clear();

    // Запоминается список стандартных клавиатурных сокращений, чтобы добавить те, которых нет в config-файле
    QStringList defaultActionNames=defaultKeyTable.keys();

    // Перебираются имена секций
    foreach(QString sectionName, availableSection) {
        config.beginGroup(sectionName); // Выбирается секция

        QStringList shortcuts=config.childKeys(); // Список горячих клавиш в секции

        // Перебираются короткие имена действий (в секции ini-файла они хранятся без префикса "имяСекции-")
        foreach (QString shortcutName, shortcuts) {
            // Если имя действия допустимо, т.е. оно есть в defaultKeyTable
            // Условие нужно чтобы в конфиг не попадали устаревшие действия
            if(defaultActionNames.contains(sectionName+"-"+shortcutName)) {
                QString actionName=sectionName+"-"+shortcutName;

                // Запоминается в таблицу полное имя действия и данные о комбинации клавиш
                keyTable.insert( actionName,
                                 data{ QKeySequence( config.value(shortcutName).toString() ),
                                       defaultKeyTable[actionName].description,
                                       defaultKeyTable[actionName].explanation }
                                );

                // Дейтвие удаляется из списка стандартных клавиатурных сокращений
                defaultActionNames.removeOne(actionName);
            }
        }

        config.endGroup(); // Закрывается текущая секция
    }

    // Добавляются стандартные клавиатурные сокращения, которых еще небыло в файле конфига
    // Срабатывает если в defaultKeyTable при очередном обновлении программы было что-то добавлено
    foreach(QString fullActionName, defaultActionNames) {
        QString sectionName=fullActionName.section('-', 0, 0);
        QString shortActionName=fullActionName.section('-', 1, 1);

        // Новое действие запоминается в конфиг
        config.setValue( sectionName+"/"+shortActionName, defaultKeyTable[fullActionName].sequence.toString() );

        // Запоминается в таблицу полное имя действия и данные о комбинации клавиш
        keyTable.insert( fullActionName,
                         data{ defaultKeyTable[fullActionName].sequence,
                               defaultKeyTable[fullActionName].description,
                               defaultKeyTable[fullActionName].explanation }
                       );
    }

    config.sync();
}


// Запись всех шорткатов в виде конфиг-файла
void ShortcutManager::saveConfig(QMap<QString, data> table)
{
    QSettings config(configFileName, QSettings::IniFormat);

    foreach (QString fullActionName, table.keys()) // Перебираются наименования действий
    {
        QKeySequence shortcutKeys=table[fullActionName].sequence; // Выясняются горячие клавиши для текущего действия
        qDebug() << fullActionName << " = " << shortcutKeys.toString();

        // Имя раздела
        QString sectionName=fullActionName.section('-', 0, 0);
        if(!availableSection.contains(sectionName)) {
            criticalError("Not found available shortcut section name for action "+fullActionName+" and keys "+shortcutKeys.toString());
            return;
        }

        // Наименование действия в разделе
        QString shortActionName=fullActionName.section('-', 1, 1);
        if(shortActionName=="") {
            criticalError("Not found shortcut shortActionName for action "+fullActionName+" and keys "+shortcutKeys.toString());
            return;
        }

        qDebug() << "Save section: "+sectionName+" action: "+shortActionName;
        config.setValue(sectionName+"/"+shortActionName, shortcutKeys.toString());
    }

    config.sync();
}


QKeySequence ShortcutManager::getKeySequence(QString actionName)
{
    if(keyTable.contains(actionName)) {
        return keyTable[actionName].sequence;
    } else {
        return QKeySequence();
    }
}


QString ShortcutManager::getDescription(QString actionName)
{
    if(keyTable.contains(actionName)) {
        return keyTable[actionName].description;
    } else {
        return "";
    }
}


QString ShortcutManager::getExplanation(QString actionName)
{
    if(keyTable.contains(actionName)) {
        return keyTable[actionName].explanation;
    } else {
        return "";
    }
}


QString ShortcutManager::getDescriptionWithShortcut(QString actionName)
{
    if(keyTable.contains(actionName)) {
        return keyTable[actionName].description+" "+getKeySequenceAsText(actionName, stringRepresentation::brackets);
    } else {
        return "";
    }
}


QString ShortcutManager::getFullDescription(QString actionName)
{
    if(keyTable.contains(actionName)) {
        QString suffix;

        if(keyTable[actionName].explanation.size()>0) {
            suffix=" - "+keyTable[actionName].explanation;
        }

        return keyTable[actionName].description+" "+getKeySequenceAsText(actionName, stringRepresentation::brackets)+suffix;
    } else {
        return "";
    }
}


// Описание шортката в таком виде, который отображается в интерфейсе для пользователя
QString ShortcutManager::getKeySequenceAsText(QString actionName, stringRepresentation mode)
{
    if(keyTable.contains(actionName)) {
        QString shortcut=keyTable[actionName].sequence.toString();

        if(shortcut.size()==0) {
            return "";
        }

        if(mode==stringRepresentation::plain) {
            return shortcut;
        }

        if(mode==stringRepresentation::brackets) {
            return "("+shortcut+")";
        }
    }

    return "";
}


void ShortcutManager::initAction(QString actionName, QAction *action)
{
    if(keyTable.contains(actionName)) {
        action->setShortcut(  getKeySequence(actionName) );
        action->setStatusTip( getFullDescription(actionName) );
        action->setToolTip(   getDescriptionWithShortcut(actionName) );
        action->setText(      getDescriptionWithShortcut(actionName) );
    } else {
        criticalError("Not found available action name "+actionName);
        return;
    }

}


void ShortcutManager::initToolButton(QString actionName, QToolButton *action)
{
    if(keyTable.contains(actionName)) {
        action->setShortcut(  getKeySequence(actionName) );
        action->setStatusTip( getFullDescription(actionName) );
        action->setToolTip(   getDescriptionWithShortcut(actionName) );
        action->setText(      getDescriptionWithShortcut(actionName) );
    } else {
        criticalError("Not found available action name "+actionName);
        return;
    }

}
