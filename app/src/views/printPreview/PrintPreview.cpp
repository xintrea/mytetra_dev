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
#include <QToolButton>

#include "PreviewView.h"
#include "PrintPreview.h"


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

    // add a nice 2 cm margin
    const qreal margin = inchesToPixels(mmToInches(20), this);
    QTextFrameFormat fmt = doc->rootFrame()->frameFormat();
    fmt.setMargin(margin);
    doc->rootFrame()->setFrameFormat(fmt);

    setup();

    QFont f(doc->defaultFont());
    f.setPointSize(10);
    doc->setDefaultFont(f);
    
    
    // Кнопки на панели инструментов
    QToolButton *button_print=new QToolButton(this);
    button_print->setText(tr("&Print..."));
    button_print->setShortcut(Qt::CTRL + Qt::Key_P);
    button_print->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(button_print, SIGNAL(clicked()), this, SLOT(print()));
    
    QToolButton *button_page_setup=new QToolButton(this);
    button_page_setup->setText(tr("Page Setup..."));
    button_page_setup->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(button_page_setup, SIGNAL(clicked()), this, SLOT(pageSetup()));
    
    QToolButton *button_zoom_in=new QToolButton(this);
    button_zoom_in->setText(tr("Zoom In"));
    button_zoom_in->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(button_zoom_in, SIGNAL(clicked()), view, SLOT(zoomIn()));

    QToolButton *button_zoom_out=new QToolButton(this);
    button_zoom_out->setText(tr("Zoom Out"));
    button_zoom_out->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(button_zoom_out, SIGNAL(clicked()), view, SLOT(zoomOut()));
    
    QToolButton *button_close=new QToolButton(this);
    button_close->setText(tr("&Close"));
    button_close->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(button_close, SIGNAL(clicked()), this, SLOT(close()));

    
    // Панель инструментов
    QHBoxLayout *toolsbox=new QHBoxLayout();
    toolsbox->addWidget(button_print);
    toolsbox->addWidget(button_page_setup);
    toolsbox->addWidget(button_zoom_in);
    toolsbox->addWidget(button_zoom_out);
    toolsbox->addWidget(button_close);
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


void PrintPreview::setup()
{
    QSizeF page = printer.pageRect().size();
    page.setWidth(page.width() * view->logicalDpiX() / printer.logicalDpiX());
    page.setHeight(page.height() * view->logicalDpiY() / printer.logicalDpiY());

    // add a nice 2 cm margin
    const qreal margin = inchesToPixels(mmToInches(20), this);
    QTextFrameFormat fmt = doc->rootFrame()->frameFormat();
    fmt.setMargin(margin);
    doc->rootFrame()->setFrameFormat(fmt);

    doc->setPageSize(page);
}


PrintPreview::~PrintPreview()
{
    delete doc;
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
        setup();
        view->updateLayout();
    }
}

// #include "PrintPreview.moc"
