#ifndef _PREVIEWVIEW_H_
#define _PREVIEWVIEW_H_

#include <QAbstractScrollArea>
#include <QTextDocument>


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

#endif // _PREVIEWVIEW_H_
