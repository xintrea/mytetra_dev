#ifndef DATABASESMANAGEMENTTABLE_H
#define DATABASESMANAGEMENTTABLE_H

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QMenu>
#include <QGestureEvent>
#include <QTapAndHoldGesture>

// Отображение таблицы известных баз данных (только таблица)


class DatabasesManagementController;

class DatabasesManagementTable : public QTableView
{
    Q_OBJECT

public:
    DatabasesManagementTable(QWidget *parent=nullptr);
    virtual ~DatabasesManagementTable();

    void init();
    void setController(DatabasesManagementController *pController);

signals:

    void tapAndHoldGestureFinished(const QPoint &pos);

protected slots:

    void onCustomContextMenuRequested(const QPoint &pos);

protected:

    DatabasesManagementController *controller;

    QMenu contextMenu;

    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    void assemblyContextMenu(void);
    void setupSignals(void);

};

#endif // DATABASESMANAGEMENTTABLE_H
