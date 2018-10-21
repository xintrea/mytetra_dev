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

    defaultKeyTable.insert("note-addNewToEnd",  QKeySequence("Ctrl+Alt+N"));
    defaultKeyTable.insert("note-addNewBefore", QKeySequence("Ctrl+Alt+J"));
    defaultKeyTable.insert("note-addNewAfter",  QKeySequence("Ctrl+Alt+M"));
    defaultKeyTable.insert("note-editField",    QKeySequence("Ctrl+Alt+E"));
    defaultKeyTable.insert("note-block",        QKeySequence("Ctrl+Alt+B"));
    defaultKeyTable.insert("note-delete",       QKeySequence("Ctrl+Alt+R"));
    defaultKeyTable.insert("note-cut",          QKeySequence("Ctrl+Alt+T"));
    defaultKeyTable.insert("note-copy",         QKeySequence("Ctrl+Alt+C"));
    defaultKeyTable.insert("note-paste",         QKeySequence("Ctrl+Alt+V"));

    defaultKeyTable.insert("note-previousNote", QKeySequence("Ctrl+Alt+Left"));
    defaultKeyTable.insert("note-nextNote",     QKeySequence("Ctrl+Alt+Right"));

    defaultKeyTable.insert("tree-add", QKeySequence("Ctrl+Shift+N"));

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

}


void ShortcutManager::checkConfigFile()
{
    // Если конфиг-файла не существует, создается стандартный
    if(!QFile(configFileName).exists()) {
        saveConfig(defaultKeyTable);
    }
}


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
                // Запоминается в ассоциативный массив полное имя действия и комбинация клавиш
                keyTable.insert(sectionName+"-"+shortcutName, QKeySequence( config.value(shortcutName).toString() ));

                // Дейтвие удаляется из списка стандартных клавиатурных сокращений
                defaultActionNames.removeOne(sectionName+"-"+shortcutName);
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
        config.setValue( sectionName+"/"+shortActionName, defaultKeyTable.value(fullActionName).toString() );

        // Запоминается в ассоциативный массив полное имя действия и комбинация клавиш
        keyTable.insert( sectionName+"-"+shortActionName, defaultKeyTable.value(fullActionName) );
    }

    config.sync();
}


// Запись всех шорткатов в виде конфиг-файла
void ShortcutManager::saveConfig(QMap<QString, QKeySequence> table)
{
    QSettings config(configFileName, QSettings::IniFormat);

    foreach (QString fullActionName, table.keys()) // Перебираются наименования действий
    {
        QKeySequence shortcutKeys=table.value(fullActionName); // Выясняются горячие клавиши для текущего действия
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
        return keyTable.value(actionName);
    } else {
        return QKeySequence();
    }
}


// Описание шортката в таком виде, который отображается в интерфейсе для пользователя
QString ShortcutManager::getKeySequenceHumanReadable(QString actionName, stringRepresentation mode)
{
    if(keyTable.contains(actionName)) {
        QString shortcut=keyTable.value(actionName).toString();

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
