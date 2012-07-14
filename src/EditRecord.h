#ifndef __EDITRECORD_H__
#define __EDITRECORD_H__

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

class InfoFieldEnter;

// Окно редактирования записи
// В данный момент редактирутся только инфополя

class EditRecord : public QDialog
{
Q_OBJECT

public:
 EditRecord( QWidget * parent = 0, Qt::WFlags f = 0 );
 ~EditRecord();

 QString getField(QString name);
 void setField(QString name, QString value);

private slots:

 void okClick(void);

private:

 // Ввод инфополей записи
 InfoFieldEnter *infoField;

 QDialogButtonBox *buttonBox;

 void setupUI(void);
 void setupSignals(void);
 void assembly(void);
};

#endif // _EDITRECORD_H__
