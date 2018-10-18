#include <QFile>
#include <QDebug>

#include "main.h"
#include "ShortcutManager.h"

#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


const QStringList ShortcutManager::availableSection={"note", "tree", "editor", "misc"};


ShortcutManager::ShortcutManager()
{

}


void ShortcutManager::init()
{
    configFileName=globalParameters.getWorkDirectory()+"/shortcut.ini";

    initDefaultKeyTable();
    checkConfigFile();
}


void ShortcutManager::initDefaultKeyTable()
{
    defaultKeyTable.insert("note-add", QKeySequence("Ctrl+Shift+N"));

    defaultKeyTable.insert("tree-add", QKeySequence("Ctrl+Alt+N"));

    defaultKeyTable.insert("editor-copy", QKeySequence("Ctrl+C"));
    defaultKeyTable.insert("editor-paste", QKeySequence("Ctrl+V"));

    defaultKeyTable.insert("misc-findInBase", QKeySequence("Ctrl+Shift+F"));
}


void ShortcutManager::checkConfigFile()
{
    // Если конфиг-файла не существует, создается стандартный
    if(!QFile(configFileName).exists()) {
        saveConfig(defaultKeyTable);
    }
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

}
