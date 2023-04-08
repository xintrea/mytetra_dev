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
  Q_OBJECT

public:
  IconSelectDialog();
  ~IconSelectDialog();

  enum resultCode{RemoveIconCode=2};

  void setPath(QString iPath);
  void setDefaultSection(QString iSectionName);
  QString getCurrentSection();

  QString getSelectFileName(void);

  int exec();

  // Проверка наличия коллекции иконок
  static void iconsCollectionCheck();

protected slots:

  void onRemoveIconClick();
  void onOkClick();
  void onCancelClick();
  void onSectionCurrentIndexChanged(int idx);
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
  int defaultSectionIndex = -1;
  int currentSectionIndex;

  bool enableIconUpdate;

  QString currentFileName;
};

#endif // ICONSELECTDIALOG_H
