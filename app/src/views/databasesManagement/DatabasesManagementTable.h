#ifndef DATABASESMANAGEMENTTABLE_H
#define DATABASESMANAGEMENTTABLE_H

#include <QTableView>
#include <QMenu>


// Отображение таблицы известных баз данных (только таблица)


class DatabasesManagementController;
class QWidget;

class DatabasesManagementTable : public QTableView
{
    Q_OBJECT

public:
    DatabasesManagementTable(QWidget *parent=nullptr);
    virtual ~DatabasesManagementTable();

    void init();
    void setController(DatabasesManagementController *pController);
    void setModel(QAbstractItemModel *model) override;

signals:

    void tapAndHoldGestureFinished(const QPoint &pos);

protected slots:

    void onCustomContextMenuRequested(const QPoint &pos);

protected:

    DatabasesManagementController *controller;

    QMenu contextMenu;

    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

    void assemblyContextMenu(void);
    void setupSignals(void);

};

#endif // DATABASESMANAGEMENTTABLE_H
