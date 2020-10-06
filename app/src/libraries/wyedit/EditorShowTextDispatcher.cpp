#include "EditorShowTextDispatcher.h"
#include "EditorShowText.h"
#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/record/MetaEditor.h"
#include "views/tree/KnowTreeView.h"
#include "models/tree/KnowTreeModel.h"
#include "models/recordTable/Record.h"
#include "libraries/helpers/ObjectHelper.h"
#include "models/appConfig/AppConfig.h"


extern AppConfig mytetraConfig;


EditorShowTextDispatcher::EditorShowTextDispatcher(QObject *parent) : QObject(parent)
{
    mThread=new QThread();
    this->moveToThread(mThread);

    mThread->start();
}


EditorShowTextDispatcher::~EditorShowTextDispatcher()
{
    // Корректное закрытие и удаление треда
    if(mThread!=nullptr)
    {
        mThread->quit();
        if(mThread->wait(1000))
        {
            mThread->terminate();
        }

        delete mThread;
    }
}


EditorShowTextDispatcher *EditorShowTextDispatcher::instance()
{
    static EditorShowTextDispatcher inst;
    return &inst;
}


void EditorShowTextDispatcher::createWindow(const QString &noteId, int x, int y, int w, int h, int vScroll)
{
    // Уже созданное окно не должно открываться дважды
    if( mWindowsList.contains( noteId ) )
    {
        // Если окно было свернуто, его надо показать
        if(mWindowsList[noteId].data()->window()->isMinimized())
        {
            mWindowsList[noteId].data()->window()->showNormal();
        }

        mWindowsList[noteId]->setWindowState(Qt::WindowNoState);
        mWindowsList[noteId]->setWindowState(Qt::WindowActive);

        return;
    }

    // Выясняется ссылка на модель дерева данных
    KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());

    // Если в дереве нет вообще записи с указанным идентификатором, окно с такой записью создать невозможно
    if( !dataModel->isRecordIdExists( noteId ))
    {
        return;
    }


    // Создается открепленное окно
    // Отсутствие родителя будет приводить к отдельному управлению
    // окном виджета при сворачивании/разворачивании.
    // Наличие родителя будет приводить к групповой работе с окном и родительским окном
    // при сворачивании/разворачивании окна виджета.
    // Примечание: нельзя в качестве parent указывать виджет редактора: если редактор
    // станет неактивным (например, когда запись не выбрана)
    // то данное окно тоже станет неактивным, и невозможно будет выделить в нем текст
    QPointer<EditorShowText> editorShowText;
    if(mytetraConfig.getDockableWindowsBehavior()=="single")
    {
        editorShowText=new EditorShowText(nullptr, Qt::Window);
    }
    else if(mytetraConfig.getDockableWindowsBehavior()=="together")
    {
        editorShowText=new EditorShowText( find_object<MainWindow>("mainwindow"), Qt::Dialog );
    }


    // Устанавливается идентификатор записи, которое отображает данное окно
    editorShowText->setNoteId( noteId );

    // Устанавливается флаг удаления диалога после закрытия его окна
    editorShowText->setAttribute( Qt::WA_DeleteOnClose );

    // Новое окно запоминается в список окон
    mWindowsList.insert( noteId, editorShowText );


    // Создается соединение, обрабатывающее закрытие окна
    connect(editorShowText.data(), &EditorShowText::editorShowTextClose,
            this, &EditorShowTextDispatcher::onCloseWindow);


    // Выясняется ссылка на объект записи
    Record *note=dataModel->getRecord(noteId);

    // Из записи заполняется содержимое открепляемого окна
    editorShowText->setDocument( note->getTextDocument() );
    editorShowText->setWindowTitle( note->getField("name") );

    // Установка координат окна
    if( !(x==-1 and y==-1 and w==-1 and h==-1) )
    {
        editorShowText->setGeometry(x, y, w, h);
    }

    editorShowText->show();

    // Установка прокрутки текста, должна быть после того, как отображаемое окно сформировано методом show()
    if(vScroll!=0)
    {
        editorShowText->setTextVerticalScroll(vScroll);
    }

    this->saveOpenWindows();
}


// Обновление содержимого открепляемого окна, содержащего запись с указанным ID
void EditorShowTextDispatcher::updateWindow(const QString &noteId)
{
    if( mWindowsList.contains( noteId ) )
    {
        // Выясняется ссылка на модель дерева данных
        KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());

        // Выясняется ссылка на объект записи
        Record *note=dataModel->getRecord(noteId);

        QSharedPointer<QTextDocument> doc=note->getTextDocument();

        // Открепляемое окно начинает отображать новый взятый из записи документ
        if( !mWindowsList[noteId].isNull() )
        {
            // Когда обновися текстовый документ, прокрутка будет сброшена, и надо будет ее восстановить
            int vScroll=mWindowsList[noteId]->getTextVerticalScroll();

            mWindowsList[noteId]->setDocument( doc ); // Текст окна
            mWindowsList[noteId]->setWindowTitle( note->getField("name") ); // Заголовок окна

            // Восстановление прокрутки
            mWindowsList[noteId]->setTextVerticalScroll(vScroll);
        }
        else
        {
            criticalError("Incorrect pointer in mWindowsList");
        }
    }
}


// Обновление содержимого всех открепляемых окон
void EditorShowTextDispatcher::updateAllWindows()
{
    for( auto noteId : mWindowsList.keys() )
    {
        this->updateWindow( noteId );
    }
}


// Закрытие указанного окна
void EditorShowTextDispatcher::closeWindow(const QString &noteId)
{
    if( mWindowsList.contains( noteId ) )
    {
        mWindowsList[ noteId ]->close();
        mWindowsList.remove( noteId );
    }
}


// Закрытие окон по списку
// Метод сделан в виде слота, чтобы он мог выполняться в отдельном потоке
// Так как списки могут быть длинными
void EditorShowTextDispatcher::closeWindowByIdVector(const QVector<QString> &ids)
{
    for( auto id : ids )
    {
        this->closeWindow(id);
    }
}


// Закрытие окон по списку
// Метод сделан в виде слота, чтобы он мог выполняться в отдельном потоке
// Так как списки могут быть длинными
void EditorShowTextDispatcher::closeWindowByIdList(const QStringList &ids)
{
    for( auto id : ids )
    {
        this->closeWindow(id);
    }
}


// Закрытие окон по списку в виде множества
// Метод сделан в виде слота, чтобы он мог выполняться в отдельном потоке
// Так как списки могут быть длинными
void EditorShowTextDispatcher::closeWindowByIdSet(const QSet<QString> &ids)
{
    for( auto id : ids )
    {
        this->closeWindow(id);
    }
}


// Закрытие окон, для которых не существует записи
// Используется при синхронизации, т.к. после синхронизации может оказаться
// что открепляемое окно не связано ни с какой записью
// Метод сделан в виде слота, чтобы он мог выполняться в отдельном потоке
void EditorShowTextDispatcher::closeWindowForNonExistentRecords()
{
    // Выясняется ссылка на модель дерева данных
    KnowTreeModel *knowTreeModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());

    QSharedPointer< QSet<QString> > availableIds=knowTreeModel->getAllRecordsIdList();

    for( auto noteId : mWindowsList.keys() )
    {
        if( !availableIds.data()->contains(noteId) )
        {
            this->closeWindow(noteId);
        }
    }
}


// Закрытие всех открепляемых окон
void EditorShowTextDispatcher::closeAllWindows()
{
    for( auto noteId : mWindowsList.keys() )
    {
        // Если существует connect() между
        // EditorShowText::editorShowTextClose и EditorShowTextDispatcher::onCloseWindow
        // то при выполнении метода close() открепляемого окна, должен будет вызваться слот
        // EditorShowTextDispatcher::onCloseWindow()
        // и в нем произойдет удаление окна из списка окон,
        // и новое состояние списка открепляемых окон будет сохранено в конфиг.
        // Объект окна должен будет сам удалиться, так как стоит флаг WA_DeleteOnClose
        mWindowsList[ noteId ]->close();
    }

    mWindowsList.clear();
}


// Закрытие всех открепляемых окон без сохранения списка удаляемых окон.
// Используется при закрытии программы, чтобы само закрытие открепляемых окон
// не повлияло на список открепляемых окон, сохраненный при выполнении предыдущих
// действий с открепляемыми окнами
void EditorShowTextDispatcher::closeAllWindowsForExit()
{
    for( auto noteId : mWindowsList.keys() )
    {
        mWindowsList[ noteId ]->disconnect(); // Все соединения закрываемого окна отключаются
        mWindowsList[ noteId ]->close();
    }

    mWindowsList.clear();
}


// Существует ли открепляемое окно с указанной записью
bool EditorShowTextDispatcher::isWindowPresent(const QString &noteId)
{
    return mWindowsList.contains( noteId );
}


// Сохранение списка ID записей, для которых открыты открепляемые окна с их координатами
void EditorShowTextDispatcher::saveOpenWindows()
{
    QStringList windowsState;

    for( auto noteId : mWindowsList.keys() )
    {
        QString state;
        state=noteId+",";

        QRect geom=mWindowsList[noteId]->geometry();
        state+=QString::number( geom.x() )+",";
        state+=QString::number( geom.y() )+",";
        state+=QString::number( geom.width() )+",";
        state+=QString::number( geom.height() )+",";
        state+=QString::number( mWindowsList[noteId]->getTextVerticalScroll() );

        windowsState << state;
    }

    // Формат запоминаемых данных
    // "IDокна1,x1,y1,w1,h1;IDокна2,x2,y2,w2,h2" и т. д.
    mytetraConfig.setDockableWindowsState( windowsState.join(";") );
}


// Восстановление открепляемых окон с записями
void EditorShowTextDispatcher::restoreOpenWindows()
{
    // Строка со всеми окнами
    QString state=mytetraConfig.getDockableWindowsState();

    // Строка со всеми окнами разделяется на подстроки с описанием одного окна
    QStringList windowsState=state.split(";");

    // Перебираются описания окон в виде ID и координат
    for( auto window : windowsState )
    {
        if(window.trimmed().size()>0) // Если описание существует, а не пустая строка
        {
            QStringList chunks=window.split(",");

            this->createWindow(chunks[0],
                               chunks[1].toInt(),
                               chunks[2].toInt(),
                               chunks[3].toInt(),
                               chunks[4].toInt(),
                               chunks[5].toInt() );
        }
    }
}


// Переключение поведения окон при сворачивыании/разворачивании
void EditorShowTextDispatcher::switchBehavior(const QString &mode)
{
    MainWindow *mainWindow=find_object<MainWindow>("mainwindow");

    for( auto widgetWindow : mWindowsList )
    {
        if(mode=="single")
        {
            widgetWindow->setParent(nullptr);
            widgetWindow->setWindowFlags(Qt::Window);
        }
        else if(mode=="together")
        {
            widgetWindow->setParent( mainWindow );
            widgetWindow->setWindowFlags(Qt::Dialog);
        }

        widgetWindow->hide();
        widgetWindow->show();
    }
}


// Обновление поведения окон согласно настройкам
// метод вызывается после разворачивания основного окна MyTetra из иконки трея
// так как при сворачивании окна MyTetra, все открепляемые окна
// теряют указатель на родителя, и, соответственно меняют свое поведение
void EditorShowTextDispatcher::updateBehavior()
{
    this->switchBehavior( mytetraConfig.getDockableWindowsBehavior() );

    qDebug() << "Update detached window behavior";
}


// При обнаружении события что одно из открепляемых окон закрывается
void EditorShowTextDispatcher::onCloseWindow(const QString &noteId)
{
    if( mWindowsList.contains( noteId ) )
    {
        // Все соединения закрываемого окна отключаются
        // так как код и так уже находится в обработчике события закрытия окна, и
        // больше никаких событий вызываться не должно
        mWindowsList[ noteId ]->disconnect();

        mWindowsList.remove(noteId); // Закрываемое окно убирается из списка окон
    }

    this->saveOpenWindows();
}

