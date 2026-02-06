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
  void setDefaultSection(QString sectionName);
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

  QLabel m_sectionLabel;
  QComboBox m_sectionComboBox;
  QListWidget m_iconList;
  QPushButton m_buttonRemoveIcon;
  QPushButton m_buttonOk;
  QPushButton m_buttonCancel;
  QProgressBar m_progressBar;

  QString m_path;
  int m_defaultSectionIndex = -1;
  int m_currentSectionIndex;

  bool m_enableIconUpdate;

  QString m_currentFileName;
};

#endif // ICONSELECTDIALOG_H
