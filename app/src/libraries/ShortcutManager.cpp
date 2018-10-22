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

    /*
    defaultKeyTable.insert("editor-copy",                QKeySequence("Ctrl+C")); // Не задействовано в коде
    defaultKeyTable.insert("editor-paste",               QKeySequence("Ctrl+V")); // Не задействовано в коде
    defaultKeyTable.insert("editor-bold",                QKeySequence("Ctrl+B"));
    defaultKeyTable.insert("editor-italic",              QKeySequence("Ctrl+I"));
    defaultKeyTable.insert("editor-underline",           QKeySequence("Ctrl+U"));
    defaultKeyTable.insert("editor-monospace",           QKeySequence("Ctrl+T"));
    defaultKeyTable.insert("editor-code",                QKeySequence("Ctrl+M"));
    defaultKeyTable.insert("editor-clear",               QKeySequence("Ctrl+K"));
    defaultKeyTable.insert("editor-textOnly",            QKeySequence("Ctrl+Shift+K"));
    defaultKeyTable.insert("editor-fixBreakSymbol",      QKeySequence("Ctrl+Shift+R"));
    defaultKeyTable.insert("editor-numericList",         QKeySequence("F12"));
    defaultKeyTable.insert("editor-dotList",             QKeySequence("Shift+F12"));
    defaultKeyTable.insert("editor-indentPlus",          QKeySequence("Ctrl+Alt+I"));
    defaultKeyTable.insert("editor-indentMinus",         QKeySequence("Ctrl+Alt+U"));
    defaultKeyTable.insert("editor-alignLeft",           QKeySequence("Ctrl+L"));
    defaultKeyTable.insert("editor-alignCenter",         QKeySequence("Ctrl+E"));
    defaultKeyTable.insert("editor-alignRight",          QKeySequence("Ctrl+R"));
    defaultKeyTable.insert("editor-alignWidth",          QKeySequence("Ctrl+J"));
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
