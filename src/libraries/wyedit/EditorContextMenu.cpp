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

 actionSelectAll=new QAction(tr("Select All"),this);

 actionEditImageProperties=new QAction(tr("Edit image properties"),this);

 actionGotoReference=new QAction(tr("Go to URL or reference"),this);
}


// Показывать или нет пункт редактирования свойств изображения
void EditorContextMenu::setImageProperties(bool isImageSelect)
{
 qDebug() << "In EditorContextMenu::set_edit_image_properties() " << isImageSelect;

 actionEditImageProperties->setVisible(isImageSelect);
}


// Показывать или нет пункт перехода по ссылке
void EditorContextMenu::setGotoReference(bool isReference)
{
  actionGotoReference->setVisible(isReference);
}


void EditorContextMenu::setup_signals(void)
{
 connect(actionUndo,SIGNAL(triggered()),this,SIGNAL(undo()));
 connect(actionRedo,SIGNAL(triggered()),this,SIGNAL(redo()));

 connect(actionCut,SIGNAL(triggered()),this,SIGNAL(cut()));
 connect(actionCopy,SIGNAL(triggered()),this,SIGNAL(copy()));
 connect(actionPaste,SIGNAL(triggered()),this,SIGNAL(paste()));

 connect(actionSelectAll,SIGNAL(triggered()),this,SIGNAL(selectAll()));

 connect(actionEditImageProperties,SIGNAL(triggered()),this,SIGNAL(contextMenuEditImageProperties()));
 connect(actionGotoReference,      SIGNAL(triggered()),this,SIGNAL(contextMenuGotoReference()));
}


void EditorContextMenu::setup_menu(void)
{
 this->addAction(actionUndo);
 this->addAction(actionRedo);

 this->addSeparator();

 this->addAction(actionCut);
 this->addAction(actionCopy);
 this->addAction(actionPaste);

 this->addSeparator();

 this->addAction(actionSelectAll);
 this->addAction(actionEditImageProperties);
 this->addAction(actionGotoReference);
}

