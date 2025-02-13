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
  InfoFieldEnter(QWidget *parent=nullptr);
  ~InfoFieldEnter();

  void setFocusToStart(void);

  bool checkFieldName(QString name);
  QString getField(QString name);
  void setField(QString name,QString value);

  void setReadOnly(bool state);
  bool isReadOnly();

  void setDisplayOnlyEditableFields(bool value);

public slots:

  void expandInfoClick(void);

private:

  bool isDisplayOnlyEditableFields;

  // Id записи
  QLabel    *recordIdLabel;
  QLineEdit *recordId;

  // Название каталога
  QLabel    *dirNameLabel;
  QLineEdit *dirName;

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
  void updateReadOnlyFieldsVisibility();

};

#endif /* INFOFIELDENTER_H_ */
