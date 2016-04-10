#ifndef ICONSELECTDIALOG_H
#define ICONSELECTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QComboBox>
#include <QProgressBar>


class IconSelectDialog : public QDialog
{
  Q_OBJECT;

public:
  IconSelectDialog();
  ~IconSelectDialog();

  void setPath(QString iPath);
  void setDefaultSection(QString iSectionName);

  QString getSelectFileName(void);

protected slots:

  void okClick();
  void onSectionCurrentIndexChanged(const QString &iText);

protected:

  void setupUI();
  void setupSignals();
  void assembly();

  QLabel sectionLabel;
  QComboBox sectionComboBox;
  QListWidget iconList;
  QDialogButtonBox buttonBox;
  QProgressBar progressBar;

  QString path;
  QString defaultSectionName;
};

#endif // ICONSELECTDIALOG_H
