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
#include <QScrollBar>
#include <QPainter>
#include <QDebug>
#include <QPageSetupDialog>
#include <QStatusBar>
#include <QToolButton>

#include "PrintPreview.h"

#ifdef Q_WS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif


static inline int inchesToPixels(float inches, QPaintDevice *device)
{
    return qRound(inches * device->logicalDpiY());
}


static inline qreal mmToInches(double mm)
{
    return mm*0.039370147;
}


class PreviewView : public QAbstractScrollArea
{
    Q_OBJECT
public:
    PreviewView(QTextDocument *document);

    inline void updateLayout() { resizeEvent(0); viewport()->update(); }

public slots:
    void zoomIn();
    void zoomOut();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    void paintPage(QPainter *painter, int page);
    QTextDocument *doc;
    qreal scale;
    int interPageSpacing;
    QPoint mousePressPos;
    QPoint scrollBarValuesOnMousePress;
};


PreviewView::PreviewView(QTextDocument *document)
{
    verticalScrollBar()->setSingleStep(20);
    horizontalScrollBar()->setSingleStep(20);

    viewport()->setBackgroundRole(QPalette::Dark);

    doc = document;
    scale = 1.0;
    interPageSpacing = 30;
}


void PreviewView::zoomIn()
{
    scale += 0.2;
    resizeEvent(0);
    viewport()->update();
}


void PreviewView::zoomOut()
{
    scale -= 0.2;
    resizeEvent(0);
    viewport()->update();
}


void PreviewView::paintEvent(QPaintEvent *)
{
    QPainter p(viewport());

    p.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
    p.translate(interPageSpacing, interPageSpacing);

    const int pages = doc->pageCount();
    for (int i = 0; i < pages; ++i) {
        p.save();
        p.scale(scale, scale);

        paintPage(&p, i);

        p.restore();
        p.translate(0, interPageSpacing + doc->pageSize().height() * scale);
    }
}


void PreviewView::paintPage(QPainter *painter, int page)
{
    const QSizeF pgSize = doc->pageSize();

    QColor col(Qt::black);

    painter->setPen(col);
    painter->setBrush(Qt::white);
    painter->drawRect(QRectF(QPointF(0, 0), pgSize));
    painter->setBrush(Qt::NoBrush);

    col = col.light();
    painter->drawLine(QLineF(pgSize.width(), 1, pgSize.width(), pgSize.height() - 1));

    col = col.light();
    painter->drawLine(QLineF(pgSize.width(), 2, pgSize.width(), pgSize.height() - 2));

    QRectF docRect(QPointF(0, page * pgSize.height()), pgSize);
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = docRect;

    // don't use the system palette text as default text color, on HP/UX
    // for example that's white, and white text on white paper doesn't
    // look that nice
    ctx.palette.setColor(QPalette::Text, Qt::black);

    painter->translate(0, - page * pgSize.height());
    painter->setClipRect(docRect);
    doc->documentLayout()->draw(painter, ctx);
}


void PreviewView::resizeEvent(QResizeEvent *)
{
    const QSize viewportSize = viewport()->size();

    QSize docSize;
    docSize.setWidth(qRound(doc->pageSize().width() * scale + 2 * interPageSpacing));
    const int pageCount = doc->pageCount();
    docSize.setHeight(qRound(pageCount * doc->pageSize().height() * scale + (pageCount + 1) * interPageSpacing));

    horizontalScrollBar()->setRange(0, docSize.width() - viewportSize.width());
    horizontalScrollBar()->setPageStep(viewportSize.width());

    verticalScrollBar()->setRange(0, docSize.height() - viewportSize.height());
    verticalScrollBar()->setPageStep(viewportSize.height());
}


void PreviewView::mousePressEvent(QMouseEvent *e)
{
    mousePressPos = e->pos();
    scrollBarValuesOnMousePress.rx() = horizontalScrollBar()->value();
    scrollBarValuesOnMousePress.ry() = verticalScrollBar()->value();
    e->accept();
}


void PreviewView::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressPos.isNull()) {
        e->ignore();
        return;
    }

    horizontalScrollBar()->setValue(scrollBarValuesOnMousePress.x() - e->pos().x() + mousePressPos.x());
    verticalScrollBar()->setValue(scrollBarValuesOnMousePress.y() - e->pos().y() + mousePressPos.y());
    horizontalScrollBar()->update();
    verticalScrollBar()->update();
    e->accept();
}


void PreviewView::mouseReleaseEvent(QMouseEvent *e)
{
    mousePressPos = QPoint();
    e->accept();
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

#include "PrintPreview.moc"
