#include <QFile>
#include <QDebug>

#include "main.h"
#include "ShortcutManager.h"

#include "libraries/GlobalParameters.h"
#include "libraries/helpers/DebugHelper.h"

extern GlobalParameters globalParameters;


const QStringList ShortcutManager::availableSection={"note", "tree", "editor", "actionLog", "attach", "misc"};

const QStringList ShortcutManager::overloadSection={"actionLog:*", "attach:editor"};


ShortcutManager::ShortcutManager(QObject *parent) : QObject(parent)
{

}


ShortcutManager::~ShortcutManager()
{

}


ShortcutManager &ShortcutManager::operator=(const ShortcutManager &obj)
{
    if(this!=&obj) {
        keyTable=obj.keyTable;
        defaultKeyTable=obj.defaultKeyTable;

        configFileName=obj.configFileName;
    }

    return *this;
}


void ShortcutManager::init()
{
    configFileName=globalParameters.getWorkDirectory()+"/shortcut.ini";

    initDefaultKeyTable();

    checkConfigFile();

    initKeyTable();
}


void ShortcutManager::save()
{
    saveConfig(keyTable);
}


void ShortcutManager::updateShortcutInApplication()
{
    qDebug() << "Update shortcut in application";
    emit updateWidgetShortcut();
}


void ShortcutManager::initDefaultKeyTable()
{
    defaultKeyTable.clear();

    defaultKeyTable.insert("note-addNewToEnd",  Data{ QKeySequence("Ctrl+Alt+N"), tr("Add a new note"), tr("") });
    defaultKeyTable.insert("note-addNewBefore", Data{ QKeySequence("Ctrl+Alt+J"), tr("Add a note before"), tr("Add a note before current selected note") });
    defaultKeyTable.insert("note-addNewAfter",  Data{ QKeySequence("Ctrl+Alt+M"), tr("Add a note after"), tr("Add a note after current selected note") });
    defaultKeyTable.insert("note-editField",    Data{ QKeySequence("Ctrl+Alt+E"), tr("Edit properties"), tr("Edit note properties (name, author, tags...)") });
    defaultKeyTable.insert("note-block",        Data{ QKeySequence("Ctrl+Alt+B"), tr("Block/Unblock note"), tr("Block or unblock current selected note") });
    defaultKeyTable.insert("note-delete",       Data{ QKeySequence("Ctrl+Alt+R"), tr("Delete note(s)"), tr("") });
    defaultKeyTable.insert("note-cut",          Data{ QKeySequence("Ctrl+Alt+X"), tr("Cut notes(s)"), tr("Cut notes(s) to clipboard") });
    defaultKeyTable.insert("note-copy",         Data{ QKeySequence("Ctrl+Alt+C"), tr("Copy note(s)"), tr("Copy note(s) to clipboard") });
    defaultKeyTable.insert("note-paste",        Data{ QKeySequence("Ctrl+Alt+V"), tr("Paste note(s)"), tr("Paste note(s) from clipboard") });
    defaultKeyTable.insert("note-moveUp",       Data{ QKeySequence("Ctrl+Alt+Up"), tr("Move up"), tr("Move up current note") });
    defaultKeyTable.insert("note-moveDn",       Data{ QKeySequence("Ctrl+Alt+Down"), tr("Move down"), tr("Move down current note") });
    defaultKeyTable.insert("note-previousNote", Data{ QKeySequence("Ctrl+Alt+Left"), tr("Previous note"), tr("Previous note has been viewing") });
    defaultKeyTable.insert("note-nextNote",     Data{ QKeySequence("Ctrl+Alt+Right"), tr("Next note"), tr("Next note has been viewing") });

    defaultKeyTable.insert("tree-expandAllSubbranch",   Data{ QKeySequence("Ctrl+Shift+8"), tr("Expand all sub items"), tr("") });
    defaultKeyTable.insert("tree-collapseAllSubbranch", Data{ QKeySequence("Ctrl+Shift+9"), tr("Collapse all sub items"), tr("") });
    defaultKeyTable.insert("tree-moveUpBranch",         Data{ QKeySequence("Ctrl+Shift+PgUp"), tr("Move item up"), tr("") });
    defaultKeyTable.insert("tree-moveDownBranch",       Data{ QKeySequence("Ctrl+Shift+PgDown"), tr("Move item down"), tr("") });
    defaultKeyTable.insert("tree-insSubbranch",         Data{ QKeySequence("Ctrl+F7"), tr("Insert a new sub item"), tr("Insert a new sub item into selected") });
    defaultKeyTable.insert("tree-insBranch",            Data{ QKeySequence("Ctrl+Shift+F7"), tr("Insert a new sibling item"), tr("Insert a new sibling item after selected") });
    defaultKeyTable.insert("tree-editBranch",           Data{ QKeySequence("Shift+F6"), tr("Edit item name"), tr("Edit name of selected item") });
    defaultKeyTable.insert("tree-delBranch",            Data{ QKeySequence("Ctrl+Shift+F8"), tr("Delete item"), tr("Delete selected item and all sub items") });
    defaultKeyTable.insert("tree-cutBranch",            Data{ QKeySequence("Ctrl+F8"), tr("Cut item"), tr("Cut item including sub items") });
    defaultKeyTable.insert("tree-copyBranch",           Data{ QKeySequence("Ctrl+Shift+F3"), tr("Copy item"), tr("Copy item including sub items") });
    defaultKeyTable.insert("tree-pasteBranch",          Data{ QKeySequence("Ctrl+Shift+F5"), tr("Paste item"), tr("Paste sibling item after selected") });
    defaultKeyTable.insert("tree-pasteSubbranch",       Data{ QKeySequence("Ctrl+F5"), tr("Paste as sub item"), tr("Paste item as sub item for selected") });
    defaultKeyTable.insert("tree-encryptBranch",        Data{ QKeySequence("Ctrl+Shift+1"), tr("Encrypt item"), tr("Encrypt item and all subitem") });
    defaultKeyTable.insert("tree-decryptBranch",        Data{ QKeySequence("Ctrl+Shift+0"), tr("Decrypt item"), tr("Decrypt item and all subitem") });
    defaultKeyTable.insert("tree-setIcon",              Data{ QKeySequence("Ctrl+Shift+O"), tr("Set icon"), tr("Set item icon") });

    defaultKeyTable.insert("editor-selectAll",           Data{ QKeySequence("Ctrl+A"), tr("Select all"), tr("") });
    defaultKeyTable.insert("editor-copy",                Data{ QKeySequence("Ctrl+C"), tr("Copy"), tr("") });
    defaultKeyTable.insert("editor-paste",               Data{ QKeySequence("Ctrl+V"), tr("Paste"), tr("") });
    defaultKeyTable.insert("editor-pasteAsPlainText",    Data{ QKeySequence("Ctrl+W"), tr("Paste plain text"), tr("") });
    defaultKeyTable.insert("editor-cut",                 Data{ QKeySequence("Ctrl+X"), tr("Cut"), tr("") });
    defaultKeyTable.insert("editor-undo",                Data{ QKeySequence("Ctrl+Z"), tr("Undo"), tr("") });
    defaultKeyTable.insert("editor-redo",                Data{ QKeySequence("Ctrl+Y"), tr("Redo"), tr("") });

    defaultKeyTable.insert("editor-bold",                Data{ QKeySequence("Ctrl+B"), tr("Bold"), tr("") });
    defaultKeyTable.insert("editor-italic",              Data{ QKeySequence("Ctrl+I"), tr("Italic"), tr("") });
    defaultKeyTable.insert("editor-underline",           Data{ QKeySequence("Ctrl+U"), tr("Underline"), tr("") });
    defaultKeyTable.insert("editor-strikeout",           Data{ QKeySequence("Ctrl+Shift+S"), tr("Strike out"), tr("") });
    defaultKeyTable.insert("editor-superscript",         Data{ QKeySequence("Ctrl+Shift+P"), tr("Superscript"), tr("") });
    defaultKeyTable.insert("editor-subscript",           Data{ QKeySequence("Ctrl+Shift+B"), tr("Subscript"), tr("") });
    defaultKeyTable.insert("editor-monospace",           Data{ QKeySequence("Ctrl+T"), tr("Monospace"), tr("") });
    defaultKeyTable.insert("editor-code",                Data{ QKeySequence("Ctrl+M"), tr("Code"), tr("Select a whole paragraphs to format text as code") });
    defaultKeyTable.insert("editor-lowercase",           Data{ QKeySequence(""), tr("Lowercase"), tr("") });
    defaultKeyTable.insert("editor-uppercase",           Data{ QKeySequence(""), tr("Uppercase"), tr("") });
    defaultKeyTable.insert("editor-clear",               Data{ QKeySequence("Ctrl+K"), tr("Clear format"), tr("When selected whole paragraph both text and paragraph format is reset to default or just text format in other case") });
    defaultKeyTable.insert("editor-textOnly",            Data{ QKeySequence("Ctrl+Shift+K"), tr("Text only"), tr("") });
    defaultKeyTable.insert("editor-fixBreakSymbol",      Data{ QKeySequence("Ctrl+Shift+R"), tr("Return type replace"), tr("Replace soft carriage return to standard carriage return") });
    defaultKeyTable.insert("editor-numericList",         Data{ QKeySequence("F12"), tr("Numeric list"), tr("") });
    defaultKeyTable.insert("editor-dotList",             Data{ QKeySequence("Shift+F12"), tr("Marked list"), tr("") });
    defaultKeyTable.insert("editor-indentPlus",          Data{ QKeySequence("Ctrl+Alt+I"), tr("Increase indent"), tr("") });
    defaultKeyTable.insert("editor-indentMinus",         Data{ QKeySequence("Ctrl+Alt+U"), tr("Decrease indent"), tr("") });
    defaultKeyTable.insert("editor-alignLeft",           Data{ QKeySequence("Ctrl+L"), tr("Align left"), tr("") });
    defaultKeyTable.insert("editor-alignCenter",         Data{ QKeySequence("Ctrl+E"), tr("Align center"), tr("") });
    defaultKeyTable.insert("editor-alignRight",          Data{ QKeySequence("Ctrl+R"), tr("Align right"), tr("") });
    defaultKeyTable.insert("editor-alignWidth",          Data{ QKeySequence("Ctrl+J"), tr("Align width"), tr("") });
    defaultKeyTable.insert("editor-fontColor",           Data{ QKeySequence("Alt+Shift+C"), tr("Text color"), tr("") });
    defaultKeyTable.insert("editor-backgroundColor",     Data{ QKeySequence("Alt+Shift+B"), tr("Background color"), tr("") });
    defaultKeyTable.insert("editor-fontSelect",          Data{ QKeySequence("Alt+F"), tr("Select font"), tr("") });
    defaultKeyTable.insert("editor-fontSize",            Data{ QKeySequence("Alt+S"), tr("Select font size"), tr("") });
    defaultKeyTable.insert("editor-findText",            Data{ QKeySequence("Ctrl+F"), tr("Find text"), tr("Find text in current note") });
    defaultKeyTable.insert("editor-settings",            Data{ QKeySequence("Ctrl+Alt+G"), tr("Editor settings"), tr("") });
    defaultKeyTable.insert("editor-reference",           Data{ QKeySequence("Ctrl+Shift+U"), tr("Edit reference URL"), tr("") });
    defaultKeyTable.insert("editor-showHtml",            Data{ QKeySequence("Ctrl+Shift+H"), tr("Edit HTML code"), tr("") });
    defaultKeyTable.insert("editor-showFormatting",      Data{ QKeySequence("Ctrl+F10"), tr("Show special chars"), tr("") });
    defaultKeyTable.insert("editor-createTable",         Data{ QKeySequence("Ctrl+F12"), tr("Create a new table"), tr("") });
    defaultKeyTable.insert("editor-tableRemoveRow",      Data{ QKeySequence(""), tr("Remove row(s)"), tr("") });
    defaultKeyTable.insert("editor-tableRemoveCol",      Data{ QKeySequence(""), tr("Remove column(s)"), tr("") });
    defaultKeyTable.insert("editor-tableAddRow",         Data{ QKeySequence(""), tr("Add row(s)"), tr("") });
    defaultKeyTable.insert("editor-tableAddCol",         Data{ QKeySequence(""), tr("Add column(s)"), tr("") });
    defaultKeyTable.insert("editor-tableMergeCells",     Data{ QKeySequence(""), tr("Merge cells"), tr("") });
    defaultKeyTable.insert("editor-tableSplitCell",      Data{ QKeySequence(""), tr("Split cell"), tr("") });
    defaultKeyTable.insert("editor-tableProperties",     Data{ QKeySequence(""), tr("Table properties"), tr("") });
    defaultKeyTable.insert("editor-openImage",           Data{ QKeySequence("Ctrl+Shift+Y"), tr("Open image"), tr("Open selected image in external viewer") });
    defaultKeyTable.insert("editor-insertImageFromFile", Data{ QKeySequence("Ctrl+Shift+I"), tr("Insert/edit image"), tr("Insert image from file or edit selected image properties") });
    defaultKeyTable.insert("editor-insertHorizontalLine",Data{ QKeySequence("Ctrl+H"), tr("Insert horizontal line"), tr("Insert a horizontal line into the empty paragraph from cursor") });
    defaultKeyTable.insert("editor-mathExpression",      Data{ QKeySequence("Ctrl+Shift+M"), tr("Insert/edit math expression"), tr("") });
    defaultKeyTable.insert("editor-expandEditArea",      Data{ QKeySequence("Ctrl+Shift+E"), tr("Expand edit area"), tr("") });
    defaultKeyTable.insert("editor-expandToolsLines",    Data{ QKeySequence("Ctrl+Shift+L"), tr("Expand tools"), tr("") });
    defaultKeyTable.insert("editor-save",                Data{ QKeySequence("Ctrl+S"), tr("Forse save note"), tr("") });
    defaultKeyTable.insert("editor-showText",            Data{ QKeySequence("Ctrl+Shift+W"), tr("Show detached window"), tr("") });
    defaultKeyTable.insert("editor-toAttach",            Data{ QKeySequence("Ctrl+Shift+A"), tr("Attach files"), tr("") });
    defaultKeyTable.insert("editor-gotoReference",       Data{ QKeySequence("Alt+Shift+U"), tr("Go to URL or reference"), tr("") }); // Сочетание Alt+U перестает работать после того, как нажато на месте без URL-a

    defaultKeyTable.insert("actionLog-copy", Data{ QKeySequence("Ctrl+C"), tr("Copy selected rows"), tr("") });

    defaultKeyTable.insert("attach-addAttach",        Data{ QKeySequence("Ctrl+N"), tr("Attach file"), tr("") });
    defaultKeyTable.insert("attach-addAttachFromUrl", Data{ QKeySequence("Ctrl+U"), tr("Attach file from URL"), tr("") });
    defaultKeyTable.insert("attach-addLink",          Data{ QKeySequence("Ctrl+L"), tr("Add link"), tr("Add link without file copying") });
    defaultKeyTable.insert("attach-editFileName",     Data{ QKeySequence("Ctrl+E"), tr("Edit file name"), tr("") });
    defaultKeyTable.insert("attach-deleteAttach",     Data{ QKeySequence("Ctrl+D"), tr("Delete file"), tr("") });
    defaultKeyTable.insert("attach-openAttach",       Data{ QKeySequence("Ctrl+O"), tr("Preview file"), tr("") });
    defaultKeyTable.insert("attach-saveAsAttach",     Data{ QKeySequence("Ctrl+S"), tr("Save as..."), tr("") });
    defaultKeyTable.insert("attach-showAttachInfo",   Data{ QKeySequence("Ctrl+I"), tr("Attach info"), tr("") });
    defaultKeyTable.insert("attach-switchToEditor",   Data{ QKeySequence("Ctrl+Shift+A"), tr("Return to editor"), tr("") });

    defaultKeyTable.insert("misc-focusTree", Data{ QKeySequence("F7"), tr("Set focus to items tree"), tr("") });
    defaultKeyTable.insert("misc-focusNoteTable", Data{ QKeySequence("F6"), tr("Set focus to notes table"), tr("") });
    defaultKeyTable.insert("misc-focusEditor", Data{ QKeySequence("F4"), tr("Set focus to editor"), tr("") });
    defaultKeyTable.insert("misc-findInBase",  Data{ QKeySequence("Ctrl+Shift+F"), tr("Find in base"), tr("") });
    defaultKeyTable.insert("misc-syncro",      Data{ QKeySequence("F9"), tr("Synchronization"), tr("Run synchronization") });
    defaultKeyTable.insert("misc-editConfirm", Data{ QKeySequence(Qt::CTRL + Qt::Key_Return), tr("Ok"), tr("") });
    defaultKeyTable.insert("misc-print",       Data{ QKeySequence("Ctrl+P"), tr("Print"), tr("") });
    defaultKeyTable.insert("misc-exportPdf",   Data{ QKeySequence("Ctrl+Shift+D"), tr("Export PDF"), tr("") });
    defaultKeyTable.insert("misc-quit",        Data{ QKeySequence("Ctrl+Q"), tr("Quit"), tr("") });
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
                                 Data{ QKeySequence( config.value(shortcutName).toString() ),
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
                         Data{ defaultKeyTable[fullActionName].sequence,
                               defaultKeyTable[fullActionName].description,
                               defaultKeyTable[fullActionName].explanation }
                       );
    }

    config.sync();
}


// Запись всех шорткатов в виде конфиг-файла
void ShortcutManager::saveConfig(QMap<QString, Data> table)
{
    QSettings config(configFileName, QSettings::IniFormat);

    foreach (QString fullActionName, table.keys()) // Перебираются наименования действий
    {
        QKeySequence shortcutKeys=table[fullActionName].sequence; // Выясняются горячие клавиши для текущего действия
        // qDebug() << fullActionName << " = " << shortcutKeys.toString();

        // Имя раздела
        QString sectionName=fullActionName.section('-', 0, 0);
        if(!availableSection.contains(sectionName)) {
            criticalError("Not found available shortcut section name for action "+fullActionName+" and keys "+shortcutKeys.toString());
        }

        // Наименование действия в разделе
        QString shortActionName=fullActionName.section('-', 1, 1);
        if(shortActionName=="") {
            criticalError("Not found shortcut shortActionName for action "+fullActionName+" and keys "+shortcutKeys.toString());
        }

        // qDebug() << "Save section: "+sectionName+" action: "+shortActionName;
        config.setValue(sectionName+"/"+shortActionName, shortcutKeys.toString());
    }

    config.sync();
}


// Список коротких имен действий в разделе
QStringList ShortcutManager::getActionsNameList(QString sectionName)
{
    if(!availableSection.contains(sectionName)) {
        return QStringList();
    }

    QStringList list;

    foreach (QString fullActionName, keyTable.keys()) // Перебираются наименования действий
    {
        QString currentSectionName=fullActionName.section('-', 0, 0);
        QString currentShortActionName=fullActionName.section('-', 1, 1);

        if(currentSectionName==sectionName) {
            list << currentShortActionName;
        }
    }

    return list;
}


QKeySequence ShortcutManager::getKeySequence(QString actionName)
{
    if(keyTable.contains(actionName)) {
        return keyTable[actionName].sequence;
    } else {
        return QKeySequence();
    }
}


// Установка клавиатурной последовательности в памяти, без записи на диск
void ShortcutManager::setKeySequence(QString actionName, QString keySequence)
{
    if(keyTable.contains(actionName)) {
        // Текущая запись в keyTable
        Data data=keyTable.value(actionName);

        // Изменяется поле в записи на новое значение
        data.sequence=QKeySequence(keySequence);

        // Запоминается измененная запись
        keyTable[actionName]=data;
    } else {
        criticalError("Incorrect action name for set key sequence: "+actionName);
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


QKeySequence ShortcutManager::getDefaultKeySequence(QString actionName)
{
    if(defaultKeyTable.contains(actionName)) {
        return defaultKeyTable[actionName].sequence;
    }

    return QKeySequence();
}


QString ShortcutManager::getDefaultKeySequenceAsText(QString actionName)
{
    return getDefaultKeySequence(actionName).toString();
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
    }

}


// Проверка имен секций, допустимо ли одно и то же сочетание клавиш
// для указанных двух секций
bool ShortcutManager::isOverloadEnable(QString sectionNameA, QString sectionNameB)
{
    // В одной и той же секции повторения никогда недопустимы
    if( sectionNameA==sectionNameB )
    {
        return false;
    }

    // Прямая проверка
    if( this->isDirectOverloadEnable(sectionNameA, sectionNameB) )
    {
        return true;
    }

    // Обратная проверка
    if( this->isDirectOverloadEnable(sectionNameB, sectionNameA) )
    {
        return true;
    }

    return false;
}


bool ShortcutManager::isDirectOverloadEnable(QString sectionNameA, QString sectionNameB)
{
    // Перебор списка с настройками допустимости перекрытый
    for(auto sectionData : overloadSection)
    {
        QStringList pair=sectionData.split(":");

        if(pair.size()!=2)
        {
            criticalError("Incorrect format for overload section. Incorrect item: "+sectionData);
        }

        QString sectionName=pair[0];
        QString sectionMask=pair[1];

        if( sectionName==sectionNameA )
        {
            if(sectionMask=="*")
            {
                return true;
            }

            if(sectionMask==sectionNameB)
            {
                return true;
            }
        }
    }

    return false;
}
