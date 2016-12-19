#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <QDialog>

class QListWidgetItem;

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget* parent = 0);
    ~ConfigDialog();

    QListWidgetItem* addWidget(QWidget *inswidget, QString name);

    void updateListWidth(void);
    void externalChangePage(QListWidgetItem *item);

private slots:
    void applyChanges(void);
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void setupSignals(void);

    QListWidgetItem *createItems(QString name);

private:
    Ui::ConfigDialog* ui;
};

#endif // _CONFIGDIALOG_H_
