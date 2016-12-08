#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <QDialog>

class QDialogButtonBox;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QListWidgetItem;
class QScrollArea;


class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog();
    QListWidgetItem *addWidget(QWidget *inswidget, QString name);


    void updateListWidth(void);
    void externalChangePage(QListWidgetItem *item);

private slots:    
    void applyChanges(void);
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    QListWidgetItem *createItems(QString name);

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
    QDialogButtonBox *confirmButtons;
};

#endif // _CONFIGDIALOG_H_
