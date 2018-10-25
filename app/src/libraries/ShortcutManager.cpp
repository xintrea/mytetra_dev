#include <QFile>
#include <QDebug>

#include "main.h"
#include "ShortcutManager.h"

#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


const QStringList ShortcutManager::availableSection={"note", "tree", "editor", "actionLog", "misc"};


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

    defaultKeyTable.insert("tree-add",     data{ QKeySequence("Ctrl+Shift+N"), QObject::tr("Add a tree item"), QObject::tr("") });

    defaultKeyTable.insert("editor-copy",     data{ QKeySequence("Ctrl+C"), QObject::tr("Copy"), QObject::tr("") }); // Не задействовано в коде
    defaultKeyTable.insert("editor-paste",    data{ QKeySequence("Ctrl+V"), QObject::tr("Paste"), QObject::tr("") }); // Не задействовано в коде
    defaultKeyTable.insert("editor-bold",    data{ QKeySequence("Ctrl+B"), QObject::tr("Bold"), QObject::tr("") });
    defaultKeyTable.insert("editor-italic",    data{ QKeySequence("Ctrl+I"), QObject::tr("Italic"), QObject::tr("") });
    defaultKeyTable.insert("editor-underline",    data{ QKeySequence("Ctrl+U"), QObject::tr("Underline"), QObject::tr("") });
    defaultKeyTable.insert("editor-monospace",    data{ QKeySequence("Ctrl+T"), QObject::tr("Monospace"), QObject::tr("") });
    defaultKeyTable.insert("editor-code",    data{ QKeySequence("Ctrl+M"), QObject::tr("Code"), QObject::tr("Select a whole paragraphs to format text as code") });
    defaultKeyTable.insert("editor-clear",    data{ QKeySequence("Ctrl+K"), QObject::tr("Clear format"), QObject::tr("When selected whole paragraph both text and paragraph format is reset to default or just text format in other case") });
    defaultKeyTable.insert("editor-textOnly",    data{ QKeySequence("Ctrl+Shift+K"), QObject::tr("Text only"), QObject::tr("") });
    defaultKeyTable.insert("editor-fixBreakSymbol",    data{ QKeySequence("Ctrl+Shift+R"), QObject::tr("Return type replace"), QObject::tr("Replace soft carriage return to standard carriage return") });
    defaultKeyTable.insert("editor-numericList",    data{ QKeySequence("F12"), QObject::tr("Numeric list"), QObject::tr("") });
    defaultKeyTable.insert("editor-dotList",    data{ QKeySequence("Shift+F12"), QObject::tr("Marked list"), QObject::tr("") });
    defaultKeyTable.insert("editor-indentPlus",    data{ QKeySequence("Ctrl+Alt+I"), QObject::tr("Increase indent"), QObject::tr("") });
    defaultKeyTable.insert("editor-indentMinus",    data{ QKeySequence("Ctrl+Alt+U"), QObject::tr("Decrease indent"), QObject::tr("") });
    defaultKeyTable.insert("editor-alignLeft",    data{ QKeySequence("Ctrl+L"), QObject::tr("Align left"), QObject::tr("") });
    defaultKeyTable.insert("editor-alignCenter",    data{ QKeySequence("Ctrl+E"), QObject::tr("Align center"), QObject::tr("") });
    defaultKeyTable.insert("editor-alignRight",    data{ QKeySequence("Ctrl+R"), QObject::tr("Align right"), QObject::tr("") });
    defaultKeyTable.insert("editor-alignWidth",    data{ QKeySequence("Ctrl+J"), QObject::tr("Align width"), QObject::tr("") });

    /*
    defaultKeyTable.insert("editor-findText",            QKeySequence("Ctrl+F"));
    defaultKeyTable.insert("editor-fontColor",           QKeySequence("Ctrl+Alt+C"));
    defaultKeyTable.insert("editor-settings",            QKeySequence("Ctrl+Alt+G"));
    defaultKeyTable.insert("editor-reference",           QKeySequence("Ctrl+Shift+U"));
    defaultKeyTable.insert("editor-showHtml",            QKeySequence("Ctrl+Shift+H"));
    defaultKeyTable.insert("editor-showFormatting",      QKeySequence("Ctrl+F10"));
    defaultKeyTable.insert("editor-createTable",         QKeySequence("Ctrl+F12"));
    defaultKeyTable.insert("editor-insertImageFromFile", QKeySequence("Ctrl+Shift+I"));
    defaultKeyTable.insert("editor-mathExpression",      QKeySequence("Ctrl+Shift+M"));
    defaultKeyTable.insert("editor-expandEditArea",      QKeySequence());
    defaultKeyTable.insert("editor-expandToolsLines",    QKeySequence());
    defaultKeyTable.insert("editor-save",                QKeySequence("Ctrl+S"));
    defaultKeyTable.insert("editor-showText",            QKeySequence("Ctrl+Shift+W"));
    defaultKeyTable.insert("editor-toAttach",            QKeySequence("Ctrl+Shift+A"));

    defaultKeyTable.insert("actionLog-copy", QKeySequence("Ctrl+C"));

    defaultKeyTable.insert("misc-findInBase",  QKeySequence("Ctrl+Shift+F"));
    defaultKeyTable.insert("misc-editConfirm", QKeySequence(Qt::CTRL + Qt::Key_Return));
    defaultKeyTable.insert("misc-print",       QKeySequence("Ctrl+P"));
    defaultKeyTable.insert("misc-exportPdf",   QKeySequence("Ctrl+D"));
    defaultKeyTable.insert("misc-quit",        QKeySequence("Ctrl+Q"));
    */

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
