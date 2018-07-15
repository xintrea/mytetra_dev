#ifndef __RECORDINFOFIELDSEDITOR_H__
#define __RECORDINFOFIELDSEDITOR_H__

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

class InfoFieldEnter;

// Окно редактирования инфополей записи

class RecordInfoFieldsEditor : public QDialog
{
  Q_OBJECT

public:
#if QT_VERSION < 0x050000
  RecordInfoFieldsEditor( QWidget * parent = 0, Qt::WFlags f = 0 );
#else
  RecordInfoFieldsEditor( QWidget * parent = 0, Qt::WindowFlags f = 0 );
#endif

  ~RecordInfoFieldsEditor();

  QString getField(QString name);
  void setField(QString name, QString value);

  void setReadOnly(bool state);
  bool isReadOnly();

private slots:

  void okClick(void);

private:

  // Виджет ввода инфополей записи
  InfoFieldEnter *infoField;

  QDialogButtonBox *buttonBox;

  void setupUI(void);
  void setupSignals(void);
  void assembly(void);
};

#endif // __RECORDINFOFIELDSEDITOR_H__
