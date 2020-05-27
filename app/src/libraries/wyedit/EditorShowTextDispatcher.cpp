#include "EditorShowTextDispatcher.h"
#include "EditorShowText.h"
#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/record/MetaEditor.h"


EditorShowTextDispatcher::EditorShowTextDispatcher(QObject *parent) : QObject(parent)
{

}


EditorShowTextDispatcher::~EditorShowTextDispatcher()
{
    // todo: сделать удаление созданных окошек
}


EditorShowTextDispatcher *EditorShowTextDispatcher::instance()
{
    static EditorShowTextDispatcher inst;
    return &inst;
}


void EditorShowTextDispatcher::createWindow(const QString &noteId)
{
    // Создается открепленное окно
    // Нелья в качестве parent указывать виджет редактора: если редактор
    // станет неактивным (например, когда запись не выбрана)
    // то данное окно тоже станет неактивным, и невозможно будет выделить в нем текст
    EditorShowText *showText=new EditorShowText( find_object<MainWindow>("mainwindow") );

    // Устанавливается флаг удаления диалога после закрытия его окна
    showText->setAttribute( Qt::WA_DeleteOnClose );

    showText->setHtml("In development....");

    /*
    find_object<MetaEditor>("mainwindow")

    editorScreen=new MetaEditor( this );
    editorScreen->setObjectName("editorScreen");

    if(getMiscField("title").length()>0)
      showText->setWindowTitle( getMiscField("title") );

    QTextDocument *cloneDocument=textArea->document()->clone(showText); // Для метода clone указан parent, который уничтожится при закрытии окна, и за собой уничтожит этот временный документ
    showText->setDocument( cloneDocument );
    */

    showText->show();
}


void EditorShowTextDispatcher::updateWindow(const QString &noteId)
{

}


void EditorShowTextDispatcher::updateAllWindow()
{

}


bool EditorShowTextDispatcher::isWindowAlreadyCreate(const QString &noteId)
{

}

void EditorShowTextDispatcher::onCloseWindow(const QString &noteId)
{

}

