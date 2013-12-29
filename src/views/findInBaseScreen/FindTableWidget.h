#ifndef _FINDTABLEWIDGET_H_
#define	_FINDTABLEWIDGET_H_

#include <QWidget>

class QTableWidget;
class QModelIndex;


class FindTableWidget : public QWidget
{
 Q_OBJECT

public:

 FindTableWidget(QWidget *parent=0);
 virtual ~FindTableWidget(void);

 void clearAll(void);
 void addRow(QString title, QString branchName, QString tags, QStringList path, int numInRecordtable);
 void updateColumnsWidth(void);

private slots:

 void selectCell(int row, int column);

private:

    QTableWidget *findTableView;
 
 void setupUI(void);
 void setupSignals(void);
 void assembly(void);
 
 bool eventFilter( QObject * o, QEvent * e );

};

#endif	/* _FINDTABLEWIDGET_H_ */

