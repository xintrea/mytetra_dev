#include "ShortcutManager.h"

ShortcutManager::ShortcutManager()
{

}


void ShortcutManager::init()
{
    initDefaultKeyTable();
}


void ShortcutManager::initDefaultKeyTable()
{
    defaultKeyTable.insert("note-add", QKeySequence("Ctrl+Shift+N"));

    defaultKeyTable.insert("tree-add", QKeySequence("Ctrl+Alt+N"));

    defaultKeyTable.insert("editor-copy", QKeySequence("Ctrl+C"));
    defaultKeyTable.insert("editor-paste", QKeySequence("Ctrl+V"));

    defaultKeyTable.insert("misc-findInBase", QKeySequence("Ctrl+Shift+F"));
}


QKeySequence ShortcutManager::getKeySequence(QString actionName)
{

}
