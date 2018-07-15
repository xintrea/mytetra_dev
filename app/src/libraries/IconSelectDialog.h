#ifndef ICONSELECTDIALOG_H
#define ICONSELECTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QProgressBar>


class IconSelectDialog : public QDialog
{
  Q_OBJECT;

public:
  IconSelectDialog();
  ~IconSelectDialog();

  enum resultCode{RemoveIconCode=2};

  void setPath(QString iPath);
  void setDefaultSection(QString iSectionName);
  QString getCurrentSection();

  QString getSelectFileName(void);

  int exec();

protected slots:

  void onRemoveIconClick();
  void onOkClick();
  void onCancelClick();
  void onSectionCurrentIndexChanged(const QString &iText);
  void updateIcons();

  void onIconItemSelectionChanged();

protected:

  void setupUI();
  void setupSignals();
  void assembly();

  QLabel sectionLabel;
  QComboBox sectionComboBox;
  QListWidget iconList;
  QPushButton buttonRemoveIcon;
  QPushButton buttonOk;
  QPushButton buttonCancel;
  QProgressBar progressBar;

  QString path;
  QString defaultSectionName;
  QString currentSectionName;

  bool enableIconUpdate;

  QString currentFileName;
};

#endif // ICONSELECTDIALOG_H
