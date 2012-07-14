#ifndef PRINTPREVIEW_H
#define PRINTPREVIEW_H

#include <QMainWindow>
#include <QPrinter>
#include <QVBoxLayout>
#include <QDialog>

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
    void setup();

    QTextDocument *doc;
    PreviewView *view;
    QPrinter printer;
    
    QVBoxLayout *centralLayout;
};

#endif // PRINTPREVIEW_H
