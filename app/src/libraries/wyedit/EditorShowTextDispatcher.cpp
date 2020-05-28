#include "EditorShowTextDispatcher.h"
#include "EditorShowText.h"
#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/record/MetaEditor.h"
#include "views/tree/KnowTreeView.h"
#include "models/tree/KnowTreeModel.h"
#include "models/recordTable/Record.h"


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
    // Нельзя в качестве parent указывать виджет редактора: если редактор
    // станет неактивным (например, когда запись не выбрана)
    // то данное окно тоже станет неактивным, и невозможно будет выделить в нем текст
    EditorShowText *showText=new EditorShowText( find_object<MainWindow>("mainwindow") );

    // Устанавливается флаг удаления диалога после закрытия его окна
    showText->setAttribute( Qt::WA_DeleteOnClose );


    // Выясняется ссылка на модель дерева данных
    KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());

    // Выясняется ссылка на объект записи
    Record *note=dataModel->getRecord(noteId);

    showText->setWindowTitle( note->getField("title") );
    showText->setHtml( note->getTextDirect() ); // showText->setHtml("In development....");

    /*
    find_object<MetaEditor>("mainwindow")

    editorScreen=new MetaEditor( this );
    editorScreen->setObjectName("editorScreen");

    if(getMiscField("title").length()>0)
      showText->setWindowTitle( getMiscField("title") );

    QTextDocument *cloneDocument=textArea->document()->clone(showText); // Для метода clone указан parent, который уничтожится при закрытии окна, и за собой уничтожит этот временный документ
    showText->setDocument( cloneDocument );


    // Устанавливается URL документа, с помощью него будут высчитываться
    // относительные ссылки при загрузке картинок
    textArea->document()->setMetaInformation( QTextDocument::DocumentUrl, "file:"+fileName );

    // Текст из файла вставляется в область редактирования
    textArea->setHtml( content );

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

