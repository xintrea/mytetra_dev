#include <QAbstractScrollArea>
#include <QPrintDialog>
#include <QPrinter>
#include <QToolBar>
#include <QAction>
#include <QTextFormat>
#include <QMouseEvent>
#include <QTextFrame>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QDebug>
#include <QPageSetupDialog>

#include "PreviewView.h"
#include "PrintPreview.h"
#include "libraries/ShortcutManager.h"


extern ShortcutManager shortcutManager;


static inline int inchesToPixels(float inches, QPaintDevice *device)
{
    return qRound(inches * device->logicalDpiY());
}


static inline qreal mmToInches(double mm)
{
    return mm*0.039370147;
}


PrintPreview::PrintPreview(const QTextDocument *document, QWidget *parent)
    : QDialog(parent), printer(QPrinter::HighResolution)
{
    setWindowTitle(tr("MyTetra - Print Preview"));

    printer.setFullPage(true);
    doc = document->clone();

    view = new PreviewView(doc);
    doc->setUseDesignMetrics(true);
    doc->documentLayout()->setPaintDevice(view->viewport());

    setupPrintDoc();
    setupUI();
    setupShortcuts();
    setupSignals();
    assembly();
}


PrintPreview::~PrintPreview()
{
    delete doc; // todo: Проверить на утечку памяти, добавить объекты для удаления если необходимо
}


void PrintPreview::setupPrintDoc()
{
    QSizeF page = printer.pageLayout().paintRectPoints().size();
    page.setWidth(page.width() * view->logicalDpiX() / printer.logicalDpiX());
    page.setHeight(page.height() * view->logicalDpiY() / printer.logicalDpiY());

    // Add a nice 2 cm margin
    // todo: Доработать размер полей согласно настройкам принтера (вызов из pageSetup())
    const qreal margin = inchesToPixels(mmToInches(20), this);
    QTextFrameFormat fmt = doc->rootFrame()->frameFormat();
    fmt.setMargin(margin);
    doc->rootFrame()->setFrameFormat(fmt);

    doc->setPageSize(page);

    QFont f(doc->defaultFont());
    f.setPointSize(10);
    doc->setDefaultFont(f);
}


void PrintPreview::setupUI()
{
    // Кнопки на панели инструментов
    buttonPrint=new QToolButton(this);
    buttonPrint->setText(tr("Print..."));
    buttonPrint->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    buttonPageSetup=new QToolButton(this);
    buttonPageSetup->setText(tr("Page Setup..."));
    buttonPageSetup->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    buttonZoomIn=new QToolButton(this);
    buttonZoomIn->setText(tr("Zoom In"));
    buttonZoomIn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    buttonZoomOut=new QToolButton(this);
    buttonZoomOut->setText(tr("Zoom Out"));
    buttonZoomOut->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    buttonClose=new QToolButton(this);
    buttonClose->setText(tr("Close"));
    buttonClose->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}


void PrintPreview::setupShortcuts()
{
    qDebug() << "Setup shortcut for" << staticMetaObject.className();

    buttonPrint->setShortcut( shortcutManager.getKeySequence("misc-print") ); // Устанавливается шорткат
    buttonPrint->setToolTip( shortcutManager.getKeySequenceAsText("misc-print") ); // ToolTip зависит от шортката
}


void PrintPreview::setupSignals()
{
    connect(buttonPrint, &QToolButton::clicked, this, &PrintPreview::print);
    connect(buttonPageSetup, &QToolButton::clicked, this, &PrintPreview::pageSetup);
    connect(buttonZoomIn, &QToolButton::clicked, view, &PreviewView::zoomIn);
    connect(buttonZoomOut, &QToolButton::clicked, view, &PreviewView::zoomOut);
    connect(buttonClose, &QToolButton::clicked, this, &PrintPreview::close);

    // Обновление горячих клавиш, если они были изменены
    connect(&shortcutManager, &ShortcutManager::updateWidgetShortcut, this, &PrintPreview::setupShortcuts);
}


void PrintPreview::assembly()
{
    // Панель инструментов
    QHBoxLayout *toolsbox=new QHBoxLayout();
    toolsbox->addWidget(buttonPrint);
    toolsbox->addWidget(buttonPageSetup);
    toolsbox->addWidget(buttonZoomIn);
    toolsbox->addWidget(buttonZoomOut);
    toolsbox->addWidget(buttonClose);
    toolsbox->addStretch();


    // Сборка содержимого окна
    centralLayout=new QVBoxLayout();
    centralLayout->addLayout(toolsbox);
    centralLayout->addWidget(view);
    centralLayout->setSpacing(1);
    centralLayout->setContentsMargins(1,1,1,1);

    setLayout(centralLayout);
    resize(800, 600);
}



void PrintPreview::print()
{
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() == QDialog::Accepted) {
        doc->print(&printer);
    }
    delete dlg;
}


void PrintPreview::pageSetup()
{
    QPageSetupDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        setupPrintDoc(); // setup();
        view->updateLayout();
    }
}

// #include "PrintPreview.moc"
