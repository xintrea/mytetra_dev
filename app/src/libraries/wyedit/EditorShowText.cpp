#include <QVBoxLayout>
#include <QDebug>
#include <QIcon>

#include "EditorShowText.h"
#include "libraries/helpers/DebugHelper.h"


EditorShowText::EditorShowText(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    mGeomX=0;
    mGeomY=0;
    mGeomW=0;
    mGeomH=0;

    setupUi();
    setupSignals();
    assembly();
}


EditorShowText::~EditorShowText()
{

}


void EditorShowText::setupUi()
{
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // Скрывается кнопка с вопросом

    // Задается иконка окна
    QIcon icon = QIcon(":/resource/pic/detach_window_icon.svg");
    this->setWindowIcon( icon );

    // Создается новая область редактора и запоминается указательна него
    mTextArea.reset(new QTextEdit(this));

    // Настраивается область редактора
    mTextArea.get()->setAcceptRichText(true);
    mTextArea.get()->setSizePolicy(sizePolicy);
    mTextArea.get()->setReadOnly(true); // Показываемый текст можно только просматривать
}


void EditorShowText::setupSignals()
{

}


void EditorShowText::assembly()
{
    QVBoxLayout *mainLayout=new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Добавляется область текста
    mainLayout->addWidget( mTextArea.get() );
}


void EditorShowText::setNoteId(const QString &noteId)
{
    mNoteId=noteId;
}


void EditorShowText::setHtml(QString text)
{
    mTextArea.get()->setHtml(text);
}


void EditorShowText::setDocument(QSharedPointer<QTextDocument> pDocument)
{
    // Очистка указателя внутри mTextArea на область документа, куда указывал mTextDocument
    // Данное действие необходимо производить перед вызовом mTextArea.get()->setDocument()
    // так как mTextArea является родителем mTextDocument, а согласно документации
    // в момент выполнения setDocument вначале происходит удаление предыдущего связанного
    // с QTextEdit документа, а потом установка нового документа.
    // Получается, что после удаления предыдущего связанного документа методом setDocument,
    // сырой указатель умного указателя начинает указывать на мусор, а потом в том же setDocument
    // этот сырой указатель используется для установки указателя на связываемый документ,
    // и это приводит к сегфолту. При установке nullptr, похоже, что предыдущий
    // документ не удаляется, и это используется для предотвращения порчи памяти
    // в последующем вызове setDocument(). Надо разобраться дальше
    mTextArea.get()->setDocument( nullptr );

    // Указатель на документ запоминается
    mTextDocument=pDocument;

    // Полученный документ устанавливается как содержимое области редактирования
    if(mTextArea.get()->document()!=mTextDocument.get())
    {
        mTextArea.get()->setDocument( mTextDocument.get() );
        mTextArea.get()->document()->setParent( mTextArea.get() );
    }
}


void EditorShowText::closeEvent(QCloseEvent *event)
{
    emit editorShowTextClose( mNoteId );

    event->accept();
}


void EditorShowText::hideEvent(QHideEvent *event)
{
    qDebug() << "Hide event of EditorShowText dialog, window X " << this->x() << " Y " << this->y() << " W " << this->width() << " H " << this->height();

    if( this->width()>0 && this->height()>0 && !(this->x()<=0 && this->y()<=0) )
    {
        // Запоминается геометрия
        QRect g=this->frameGeometry();
        qDebug() << "Frame geometry X " << g.x() << " Y " << g.y() << " W " << g.width() << " H " << g.height();
        mGeomX=g.x();
        mGeomY=g.y();
        mGeomW=g.width();
        mGeomH=g.height();
    }

    QWidget::hideEvent(event);
}


void EditorShowText::showEvent(QShowEvent *event)
{
    qDebug() << "Show event of EditorShowText dialog";

    // Если была запомнена геометрия окна, устанавливается прежняя геометрия
    if(!(mGeomX==0 && mGeomY==0 && mGeomW==0 && mGeomH==0))
    {
        this->move(mGeomX,mGeomY);
    }
    else
        qDebug() << "Previos geometry of EditorShowText dialog is not setted";

    QWidget::showEvent(event);
}


/*
// Говорят, что hideEvent/showEvent не относится к свертке/развертке, т.е. "свернутые" окна по-прежнему считаются видимыми.
// Поэтому надо ловить QWindowStateChangeEvent. Но это не подтвердилось в KDE.
// В нем нормально отрабатывают hideEvent/showEvent при сворачивании-разворачивании основного окна
// События hideEvent/showEvent не генерируются в Windows 7 (в других не проверено), но в нем решаемым этим кодом проблеммы нет
void EditorShowText::changeEvent(QEvent *event)
{
  qDebug() << "EditorShowText changeEvent: " << event->type();

  if(event->type() == QEvent::WindowStateChange) {
    QWindowStateChangeEvent* WindowStateChangeEvent = (QWindowStateChangeEvent*)event;
    if(!(WindowStateChangeEvent->oldState() & Qt::WindowMaximized) && isMaximized())
    {
      geomX=x();
      geomY=y();
      geomW=width();
      geomH=height();
    }
  }
}
*/
