#ifndef _FINDTABLEWIDGET_H_
#define	_FINDTABLEWIDGET_H_

#include <QWidget>

class QModelIndex;
class QTableView;
class QStandardItemModel;


class FindTableWidget : public QWidget {
    Q_OBJECT

public:

    FindTableWidget(QWidget *parent=0);
    virtual ~FindTableWidget(void);

    void clearAll(void);
    void addRow(QString title, QString branchName, QString tags, QStringList path, QString recordId);
    void updateColumnsWidth(void);

private slots:

// void selectCell(int row, int column);
    void selectCell(const QModelIndex & index);

private:

    QTableView *findTableView;
    QStandardItemModel *findTableModel;

    void setupUI(void);
    void setupModels(void);
    void setupSignals(void);
    void assembly(void);

    bool eventFilter( QObject * o, QEvent * e );

};

#endif	/* _FINDTABLEWIDGET_H_ */

