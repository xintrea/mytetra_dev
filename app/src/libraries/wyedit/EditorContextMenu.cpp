#include <QAction>
#include <QDebug>
#include <QtGui>

#include "EditorContextMenu.h"
#include "Editor.h"
#include "EditorCursorPositionDetector.h"
#include "libraries/ShortcutManager.h"


extern ShortcutManager shortcutManager;


EditorContextMenu::EditorContextMenu(QWidget *parent) : QMenu(parent)
{
    setupActions();
    setupShortcuts();
    setupSignals();
    setupMenu();
}


EditorContextMenu::~EditorContextMenu(void)
{

}


void EditorContextMenu::setupActions(void)
{
 actionUndo=new QAction(this);
 actionUndo->setIcon(QIcon(":/resource/pic/edit_undo.svg"));
 actionUndo->setEnabled(false); // undo недоступно при создании actionUndo еще не было ни одного действия с текстом
 actionRedo=new QAction(this);
 actionRedo->setIcon(QIcon(":/resource/pic/edit_redo.svg"));
 actionRedo->setEnabled(false); // redo недоступно при создании actionRedo еще не было ни одного действия с текстом

 actionCut=new QAction(this);
 actionCopy=new QAction(this);
 actionPaste=new QAction(this);
 actionPasteAsPlainText=new QAction(this);
 actionSelectAll=new QAction(this);

 actionOpenImage=new QAction(this);
 actionEditImageProperties=new QAction(this);
 actionEditMathExpression=new QAction(this);
 actionGotoReference=new QAction(this);

 actionLowercase=new QAction(this);
 actionUppercase=new QAction(this);
}


void EditorContextMenu::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << this->metaObject()->className();

    shortcutManager.initAction("editor-undo", actionUndo );
    shortcutManager.initAction("editor-redo", actionRedo );
    shortcutManager.initAction("editor-cut", actionCut );
    shortcutManager.initAction("editor-copy", actionCopy );
    shortcutManager.initAction("editor-paste", actionPaste );
    shortcutManager.initAction("editor-pasteAsPlainText", actionPasteAsPlainText );
    shortcutManager.initAction("editor-selectAll", actionSelectAll );

    // "Умное" действие Открыть изображение
    shortcutManager.initAction("editor-openImage", actionOpenImage);
    actionOpenImage->setText(tr("Open image"));

    // "Умное" действие Вставить изображение / Редактировать свойства изображения
    shortcutManager.initAction("editor-insertImageFromFile", actionEditImageProperties );
    actionEditImageProperties->setText(tr("Edit image properties")); // В контекстном меню это редактирование свойств изображения

    // "Умное" действие Вставить / Редактировать формулу
    shortcutManager.initAction("editor-mathExpression", actionEditMathExpression );
    actionEditMathExpression->setText(tr("Edit math expression")); // В контекстном меню это редактирование формулы

    shortcutManager.initAction("editor-gotoReference", actionGotoReference );

    shortcutManager.initAction("editor-lowercase", actionLowercase );
    shortcutManager.initAction("editor-uppercase", actionUppercase );
}


void EditorContextMenu::update(void)
{
    // Сначала скрываются пункты редактирования формулы и картинки
    setEditMathExpression( false );
    setImageProperties( false );

    setFormatToLowerCase( true );
    setFormatToUpperCase( true );

    // Если выбрана формула или курсор находится на позиции формулы
    if(static_cast<Editor*>(this->parent())->cursorPositionDetector->isMathExpressionSelect() ||
       static_cast<Editor*>(this->parent())->cursorPositionDetector->isCursorOnMathExpression()) {
        setEditMathExpression( true );
        setFormatToLowerCase( false );
        setFormatToUpperCase( false );
    } else {

        // Если выбрана картинка или курсор находится на позиции картинки
        if(static_cast<Editor*>(this->parent())->cursorPositionDetector->isImageSelect() ||
           static_cast<Editor*>(this->parent())->cursorPositionDetector->isCursorOnImage()) {
            setImageProperties( true );
            setFormatToLowerCase( false );
            setFormatToUpperCase( false );
        }
    }


    // Если курсор находится на ссылке (URL)
    if(static_cast<Editor*>(this->parent())->cursorPositionDetector->isCursorOnReference()) {
        setGotoReference( true );
    } else {
        setGotoReference( false );
    }

    // Если в буфере обмена есть текст
    if(QGuiApplication::clipboard()->text().size()>0) {
        setPasteAsPlainText( true );
    } else {
        setPasteAsPlainText( false );
    }
}


QList<QAction *> EditorContextMenu::getActionsList()
{
    QList<QAction *> list;

    // Следующие действия уже есть на панели инструментов, они не должны добавляться на виджет редактора
    // << actionEditImageProperties
    // << actionEditMathExpression

    list << actionUndo
         << actionRedo
         << actionCut
         << actionCopy
         << actionPaste
         << actionPasteAsPlainText
         << actionSelectAll
         << actionGotoReference;

    return list;
}


// Показывать или нет пункт редактирования свойств изображения
void EditorContextMenu::setImageProperties(bool flag)
{
    qDebug() << "In EditorContextMenu::setImageProperties() " << flag;

    actionOpenImage->setVisible(flag);
    actionOpenImage->setEnabled(flag);

    actionEditImageProperties->setVisible(flag);
    actionEditImageProperties->setEnabled(flag);
}


// Показывать или нет пункт редактирования математического выражения
void EditorContextMenu::setEditMathExpression(bool flag)
{
    qDebug() << "In EditorContextMenu::setEditMathExpression() " << flag;

    actionEditMathExpression->setVisible(flag);
    actionEditMathExpression->setEnabled(flag);

}


// Показывать или нет пункт перехода по ссылке
void EditorContextMenu::setGotoReference(bool flag)
{
  actionGotoReference->setVisible(flag);
  actionGotoReference->setEnabled(flag);
}


// Показывать или нет пункт "Вставить только текст"
void EditorContextMenu::setPasteAsPlainText(bool flag)
{
  actionPasteAsPlainText->setVisible(flag);
  actionPasteAsPlainText->setEnabled(flag);
}


// Показывать или нет пункт "Строчные"
void EditorContextMenu::setFormatToLowerCase(bool flag)
{
  actionLowercase->setVisible(flag);
  actionLowercase->setEnabled(flag);
}


// Показывать или нет пункт "ПРОПИСНЫЕ"
void EditorContextMenu::setFormatToUpperCase(bool flag)
{
  actionUppercase->setVisible(flag);
  actionUppercase->setEnabled(flag);
}


void EditorContextMenu::setupSignals(void)
{
    connect(actionUndo,            &QAction::triggered, this, &EditorContextMenu::onActionUndo);
    connect(actionRedo,            &QAction::triggered, this, &EditorContextMenu::onActionRedo);
    connect(actionCut,             &QAction::triggered, this, &EditorContextMenu::onActionCut);
    connect(actionCopy,            &QAction::triggered, this, &EditorContextMenu::onActionCopy);
    connect(actionPaste,           &QAction::triggered, this, &EditorContextMenu::onActionPaste);
    connect(actionPasteAsPlainText,&QAction::triggered, this, &EditorContextMenu::onActionPasteAsPlainText);
    connect(actionSelectAll,       &QAction::triggered, this, &EditorContextMenu::onActionSelectAll);

    connect(actionOpenImage,          &QAction::triggered, this, &EditorContextMenu::onActionContextMenuOpenImage);
    connect(actionEditImageProperties,&QAction::triggered, this, &EditorContextMenu::onActionContextMenuEditImageProperties);
    connect(actionEditMathExpression, &QAction::triggered, this, &EditorContextMenu::onActionContextMenuEditMathExpression);
    connect(actionGotoReference,      &QAction::triggered, this, &EditorContextMenu::onActionContextMenuGotoReference);

    connect(actionLowercase, &QAction::triggered, this, &EditorContextMenu::onActionLowercase);
    connect(actionUppercase, &QAction::triggered, this, &EditorContextMenu::onActionUppercase);

    // Обновление горячих клавиш, если они были изменены
    connect(&shortcutManager, &ShortcutManager::updateWidgetShortcut, this, &EditorContextMenu::setupShortcuts);
}


void EditorContextMenu::setupMenu(void)
{
    this->addAction(actionCut);
    this->addAction(actionCopy);
    this->addAction(actionPaste);
    this->addAction(actionPasteAsPlainText);

    this->addSeparator();

    this->addAction(actionUndo);
    this->addAction(actionRedo);

    this->addSeparator();

    this->addAction(actionLowercase);
    this->addAction(actionUppercase);

    this->addSeparator();

    this->addAction(actionSelectAll);
    this->addAction(actionOpenImage);
    this->addAction(actionEditImageProperties);
    this->addAction(actionEditMathExpression);
    this->addAction(actionGotoReference);
}


void EditorContextMenu::onActionUndo()
{
    update();
    if(actionUndo->isEnabled()) {
        emit undo();
    }
}

void EditorContextMenu::onActionRedo()
{
    update();
    if(actionRedo->isEnabled()) {
        emit redo();
    }
}

void EditorContextMenu::onActionCut()
{
    update();
    if(actionCut->isEnabled()) {
        emit cut();
    }
}

void EditorContextMenu::onActionCopy()
{
    update();
    if(actionCopy->isEnabled()) {
        emit copy();
    }
}

void EditorContextMenu::onActionPaste()
{
    update();
    if(actionPaste->isEnabled()) {
        emit paste();
    }
}

void EditorContextMenu::onActionPasteAsPlainText()
{
    update();
    if(actionPasteAsPlainText->isEnabled()) {
        emit pasteAsPlainText();
    }
}

void EditorContextMenu::onActionSelectAll()
{
    update();
    if(actionSelectAll->isEnabled()) {
        emit selectAll();
    }
}

void EditorContextMenu::onActionContextMenuOpenImage()
{
    update();
    if(actionOpenImage->isEnabled())
    {
        emit contextMenuOpenImage();
    }
}

void EditorContextMenu::onActionContextMenuEditImageProperties()
{
    update();
    if(actionEditImageProperties->isEnabled()) {
        emit contextMenuEditImageProperties();
    }
}

void EditorContextMenu::onActionContextMenuEditMathExpression()
{
    update();
    if(actionEditMathExpression->isEnabled()) {
        emit contextMenuEditMathExpression();
    }
}

void EditorContextMenu::onActionContextMenuGotoReference()
{
    update();
    if(actionGotoReference->isEnabled()) {
        emit contextMenuGotoReference();
    }
}

void EditorContextMenu::onActionLowercase()
{
    update();
    if(actionLowercase->isEnabled()) {
        emit lowercase();
    }
}

void EditorContextMenu::onActionUppercase()
{
    update();
    if(actionUppercase->isEnabled()) {
        emit uppercase();
    }
}

