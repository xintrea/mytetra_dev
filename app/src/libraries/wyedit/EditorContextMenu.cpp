#include <QAction>
#include <QDebug>

#include "EditorContextMenu.h"


EditorContextMenu::EditorContextMenu(QWidget *parent) : QMenu(parent)
{
 setup_actions();
 setup_signals();
 setup_menu();
}


EditorContextMenu::~EditorContextMenu(void)
{

}


void EditorContextMenu::setup_actions(void)
{
 actionUndo=new QAction(tr("Undo"),this);

 actionRedo=new QAction(tr("Redo"),this);

 actionCut=new QAction(tr("Cut"),this);

 actionCopy=new QAction(tr("Copy"),this);

 actionPaste=new QAction(tr("Paste"),this);

 actionPasteAsPlainText=new QAction(tr("Paste plain text"),this);

 actionSelectAll=new QAction(tr("Select All"),this);

 actionEditImageProperties=new QAction(tr("Edit image properties"),this);

 actionEditMathExpression=new QAction(tr("Edit math expression"),this);

 actionGotoReference=new QAction(tr("Go to URL or reference"),this);
}


// Показывать или нет пункт редактирования свойств изображения
void EditorContextMenu::setImageProperties(bool flag)
{
    qDebug() << "In EditorContextMenu::setImageProperties() " << flag;

    actionEditImageProperties->setVisible(flag);
}


// Показывать или нет пункт редактирования математического выражения
void EditorContextMenu::setEditMathExpression(bool flag)
{
    qDebug() << "In EditorContextMenu::setEditMathExpression() " << flag;

    actionEditMathExpression->setVisible(flag);
}


// Показывать или нет пункт перехода по ссылке
void EditorContextMenu::setGotoReference(bool flag)
{
  actionGotoReference->setVisible(flag);
}


// Показывать или нет пункт "Вставить только текст"
void EditorContextMenu::setPasteAsPlainText(bool flag)
{
  actionPasteAsPlainText->setVisible(flag);
}


void EditorContextMenu::setup_signals(void)
{
 connect(actionUndo, &QAction::triggered, this, &EditorContextMenu::undo);
 connect(actionRedo, &QAction::triggered, this, &EditorContextMenu::redo);

 connect(actionCut,             &QAction::triggered, this, &EditorContextMenu::cut);
 connect(actionCopy,            &QAction::triggered, this, &EditorContextMenu::copy);
 connect(actionPaste,           &QAction::triggered, this, &EditorContextMenu::paste);
 connect(actionPasteAsPlainText,&QAction::triggered, this, &EditorContextMenu::pasteAsPlainText);

 connect(actionSelectAll,       &QAction::triggered, this, &EditorContextMenu::selectAll);

 connect(actionEditImageProperties,&QAction::triggered, this, &EditorContextMenu::contextMenuEditImageProperties);
 connect(actionEditMathExpression, &QAction::triggered, this, &EditorContextMenu::contextMenuEditMathExpression);
 connect(actionGotoReference,      &QAction::triggered, this, &EditorContextMenu::contextMenuGotoReference);
}


void EditorContextMenu::setup_menu(void)
{
 this->addAction(actionUndo);
 this->addAction(actionRedo);

 this->addSeparator();

 this->addAction(actionCut);
 this->addAction(actionCopy);
 this->addAction(actionPaste);
 this->addAction(actionPasteAsPlainText);

 this->addSeparator();

 this->addAction(actionSelectAll);
 this->addAction(actionEditImageProperties);
 this->addAction(actionEditMathExpression);
 this->addAction(actionGotoReference);
}

