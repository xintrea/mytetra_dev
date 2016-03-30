#ifndef INFOFIELDENTER_H_
#define INFOFIELDENTER_H_

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>
#include <QToolButton>

// Виджет ввода инфополей Title, Author, Url, Tags...


class InfoFieldEnter : public QWidget
{
  Q_OBJECT

public:
  InfoFieldEnter(QWidget *parent=0);
  ~InfoFieldEnter();

  void setFocusToStart(void);

  bool checkFieldName(QString name);
  QString getField(QString name);
  void setField(QString name,QString value);

  void setReadOnly(bool state);
  bool isReadOnly();

public slots:

  void expandInfoClick(void);

private:

  // Ввод названия записи
  QLabel    *recordNameLabel;
  QLineEdit *recordName;

  // Ввод автора
  QLabel    *recordAuthorLabel;
  QLineEdit *recordAuthor;

  // Ввод Url
  QLabel    *recordUrlLabel;
  QLineEdit *recordUrl;

  // Ввод текстовых меток
  QLabel    *recordTagsLabel;
  QLineEdit *recordTags;

  // Кнопка, раскрывающая и скрывающая поля author, url, tags
  QToolButton *expandInfo;

  // Размещалка элементов
  QGridLayout *infoFieldLayout;

  void setup_ui(void);
  void setup_signals(void);
  void assembly(void);

  void expandInfoOnDisplay(QString expand);

};

#endif /* INFOFIELDENTER_H_ */
