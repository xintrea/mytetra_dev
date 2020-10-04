#include <QAction>
#include <QDebug>
#include <QtGui>

#include "EditorShowTextContextMenu.h"
#include "Editor.h"
#include "EditorCursorPositionDetector.h"
#include "libraries/ShortcutManager.h"


EditorShowTextContextMenu::EditorShowTextContextMenu(QWidget *parent) : QMenu(parent)
{
    setupActions();
    setupSignals();
    setupMenu();
}


EditorShowTextContextMenu::~EditorShowTextContextMenu(void)
{

}


void EditorShowTextContextMenu::setupActions(void)
{
    actionCopy=new QAction(this);
    actionCopy->setText( tr("Copy"));
    actionCopy->setShortcut( QKeySequence("Ctrl+C") );

    actionSelectAll=new QAction(this);
    actionSelectAll->setText( tr("Select All"));
    actionSelectAll->setShortcut( QKeySequence("Ctrl+A") );

    actionGotoNote=new QAction(this);
    actionGotoNote->setText( tr("Go to current note"));
}


void EditorShowTextContextMenu::update(void)
{
    actionCopy->setVisible(true);
    actionCopy->setEnabled( static_cast<EditorShowText*>(this->parent())->getTextArea()->textCursor().hasSelection() );

    actionSelectAll->setVisible(true);
    actionSelectAll->setEnabled(true);

    actionGotoNote->setVisible(true);
    actionGotoNote->setEnabled(true);
}


void EditorShowTextContextMenu::setupSignals(void)
{
    connect(actionCopy,      &QAction::triggered, this, &EditorShowTextContextMenu::onActionCopy);
    connect(actionSelectAll, &QAction::triggered, this, &EditorShowTextContextMenu::onActionSelectAll);
    connect(actionGotoNote,  &QAction::triggered, this, &EditorShowTextContextMenu::onActionGotoNote);
}


void EditorShowTextContextMenu::setupMenu(void)
{
    this->addAction(actionCopy);
    this->addAction(actionSelectAll);

    this->addSeparator();

    this->addAction(actionGotoNote);
}


// В этом слоте смысла особого нет, копирование сработает по-дефолту
void EditorShowTextContextMenu::onActionCopy()
{
    if(actionCopy->isEnabled()) {
        emit copy(); // В принципе, сигнал не имеет смылса испускать
    }
}


// В этом слоте смысла особого нет, "выделить все" сработает по-дефолту
void EditorShowTextContextMenu::onActionSelectAll()
{
    if(actionSelectAll->isEnabled()) {
        emit selectAll();  // В принципе, сигнал не имеет смылса испускать
    }
}


void EditorShowTextContextMenu::onActionGotoNote()
{
    if(actionGotoNote->isEnabled()) {
        emit gotoNote();
    }
}
