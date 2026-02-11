#ifndef __ACTIONLOGVIEW_H__
#define __ACTIONLOGVIEW_H__

#include <QObject>
#include <QTableView>
#include <QMenu>

// Отображение лога в виде таблицы (только таблица)

class ActionLogController;
class QWidget;

class ActionLogView : public QTableView
{
    Q_OBJECT

public:
    ActionLogView(QWidget *parent=nullptr);
    virtual ~ActionLogView();

    void init();
    void setController(ActionLogController *pController);

signals:

    void tapAndHoldGestureFinished(const QPoint &pos);

protected slots:

    void onCustomContextMenuRequested(const QPoint &pos);

protected:

    ActionLogController *controller;

    QMenu contextMenu;

    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    void assemblyContextMenu(void);
    void setupSignals(void);

};

#endif // __ACTIONLOGVIEW_H__
