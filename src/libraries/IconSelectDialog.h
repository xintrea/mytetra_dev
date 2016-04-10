#ifndef ICONSELECTDIALOG_H
#define ICONSELECTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QComboBox>


class IconSelectDialog : public QDialog
{
  Q_OBJECT;

public:
  IconSelectDialog();
  ~IconSelectDialog();

  void setPath(QString iPath);
  void setDefaultSection(QString iSectionName);

  QString getSelectFileName(void);

protected:

  void setupUI();
  void setupSignals();
  void assembly();

  void okClick();

  QLabel sectionLabel;
  QComboBox sectionComboBox;
  QListWidget iconList;
  QDialogButtonBox buttonBox;

  QString path;

};

#endif // ICONSELECTDIALOG_H
