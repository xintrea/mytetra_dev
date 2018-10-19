#ifndef _PRINTPREVIEW_H_
#define _PRINTPREVIEW_H_

#include <QMainWindow>
#include <QPrinter>
#include <QVBoxLayout>
#include <QDialog>
#include <QToolButton>

class PreviewView;
class QTextDocument;

class PrintPreview : public QDialog
{
    Q_OBJECT
public:
    PrintPreview(const QTextDocument *document, QWidget *parent);
    virtual ~PrintPreview();

private slots:
    void print();
    void pageSetup();

private:
    void setupPrintDoc();
    void setupUI();
    void setupShortcuts();
    void setupSignals();
    void assembly();

    QTextDocument *doc;
    PreviewView *view;
    QPrinter printer;

    QToolButton *buttonPrint;
    QToolButton *buttonPageSetup;
    QToolButton *buttonZoomIn;
    QToolButton *buttonZoomOut;
    QToolButton *buttonClose;
    
    QVBoxLayout *centralLayout;
};

#endif // _PRINTPREVIEW_H_
