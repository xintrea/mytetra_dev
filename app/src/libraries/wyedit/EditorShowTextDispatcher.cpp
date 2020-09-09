#include "EditorShowTextDispatcher.h"
#include "EditorShowText.h"
#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/record/MetaEditor.h"
#include "views/tree/KnowTreeView.h"
#include "models/tree/KnowTreeModel.h"
#include "models/recordTable/Record.h"
#include "libraries/helpers/ObjectHelper.h"


EditorShowTextDispatcher::EditorShowTextDispatcher(QObject *parent) : QObject(parent)
{

}


EditorShowTextDispatcher::~EditorShowTextDispatcher()
{

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
    QPointer<EditorShowText> editorShowText( new EditorShowText( find_object<MainWindow>("mainwindow") ) );

    // Устанавливается идентификатор записи, которое отображает данное окно
    editorShowText->setNoteId( noteId );

    // Устанавливается флаг удаления диалога после закрытия его окна
    editorShowText->setAttribute( Qt::WA_DeleteOnClose );

    // Новое окно запоминается в список окон
    mWindowList.insert( noteId, editorShowText );


    // Создается соединение, обрабатывающее закрытие окна
    connect(editorShowText.data(), &EditorShowText::editorShowTextClose,
            this, &EditorShowTextDispatcher::onCloseWindow);


    // Выясняется ссылка на модель дерева данных
    KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());

    // Выясняется ссылка на объект записи
    Record *note=dataModel->getRecord(noteId);

    editorShowText->setDocument( note->getTextDocument() );
    editorShowText->setWindowTitle( note->getField("name") );
    editorShowText->show();
}


// Обновление содержимого открепляемого окна, содержащего запись с указанным ID
void EditorShowTextDispatcher::updateWindow(const QString &noteId)
{
    if( mWindowList.contains( noteId ) )
    {
        // Выясняется ссылка на модель дерева данных
        KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());

        // Выясняется ссылка на объект записи
        Record *note=dataModel->getRecord(noteId);

        // Открепляемое окно начинает отображать новый взятый из записи документ
        mWindowList[noteId]->setDocument( note->getTextDocument() );
    }
}


// Обновление содержимого всех открепляемых окон
void EditorShowTextDispatcher::updateAllWindow()
{
    for( auto noteId : mWindowList.keys() )
    {
        this->updateWindow( noteId );
    }
}


// Существует ли открепляемое окно с указанной записью
bool EditorShowTextDispatcher::isWindowPresent(const QString &noteId)
{
    return mWindowList.contains( noteId );
}


void EditorShowTextDispatcher::saveOpenWindows()
{

}


void EditorShowTextDispatcher::restoreOpenWindows()
{

}


// При обнаружении события что одно из открепляемых окон закрывается
void EditorShowTextDispatcher::onCloseWindow(const QString &noteId)
{
    if( mWindowList.contains( noteId ) )
    {
        mWindowList[ noteId ]->disconnect(); // Все соединения закрываемого окна отключаются

        mWindowList.remove(noteId); // Закрываемое окно убирается из списка окон
    }
}

