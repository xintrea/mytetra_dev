#ifndef __ADDNEWRECORD_H__
#define __ADDNEWRECORD_H__

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

#include "libraries/wyedit/Editor.h"
#include "views/record/InfoFieldEnter.h"


// Окно добавления новой записи

class AddNewRecord : public QDialog
{
Q_OBJECT

public:

 #if QT_VERSION < 0x050000
  AddNewRecord( QWidget * parent = 0, Qt::WFlags f = 0 );
 #else
  AddNewRecord( QWidget * parent = 0, Qt::WindowFlags f = 0 );
 #endif

 ~AddNewRecord();

 QString getField(QString name);
 QString getImagesDirectory(void);

private slots:

 void okClick(void);

private:

 // Ввод инфополей записи
 InfoFieldEnter infoField;

 // Ввод текста записи
 Editor recordTextEditor;

 QDialogButtonBox buttonBox;

 QString imagesDirName;

 void setupUI(void);
 void setupSignals(void);
 void assembly(void);

 void setupEventFilter(void);

 virtual bool eventFilter(QObject *object, QEvent *event);
};

#endif // __ADDNEWRECORD_H__
