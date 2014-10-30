#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <QDialog>

class QDialogButtonBox;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QListWidgetItem;


class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog();
    QListWidgetItem *add_widget(QWidget *inswidget, QString name);
    void set_window_title(QString title);

    void updateListWidth(void);

public slots:
    void change_page(QListWidgetItem *current, QListWidgetItem *previous);

private slots:    
    void apply_changes(void);

private:
    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);

    QListWidgetItem *create_items(QString name);

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
    QDialogButtonBox *confirmButtons;
};

#endif // _CONFIGDIALOG_H_
