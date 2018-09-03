#ifndef _FINDTABLEWIDGET_H_
#define	_FINDTABLEWIDGET_H_

#include <QWidget>

class QModelIndex;
class QTableView;
class QStandardItemModel;


class FindTableWidget : public QWidget
{
  Q_OBJECT

public:

  FindTableWidget(QWidget *parent=0);
  virtual ~FindTableWidget(void);

  void clearAll(void);
  void addRow(QString title, QString branchName, QString tags, QStringList path, QString recordId);
  int  getRowCount();
  void updateColumnsWidth(void);
  void setOverdrawMessage(const QString iOverdrawMessage); // Установка надписи, которая появляется поверх виджета

private slots:

  // void selectCell(int row, int column);
  void selectCell(const QModelIndex & index);

private:

  QTableView *findTableView;
  QStandardItemModel *findTableModel;
  QString overdrawMessage;

  void setupUI(void);
  void setupModels(void);
  void setupSignals(void);
  void assembly(void);

  void paintEvent(QPaintEvent *event);
};

#endif	/* _FINDTABLEWIDGET_H_ */

