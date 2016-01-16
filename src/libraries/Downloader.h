#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QDialog>
#include <QMap>

class QString;
class QStringList;
class QByteArray;
class QPushButton;
class QTableWidget;


class Downloader : public QDialog
{
  Q_OBJECT

public:
  Downloader();
  ~Downloader();

  enum saveModeType{disk, memory};

  void setSaveMode(int iMode);
  void setSaveDirectory(QString iDir);
  void setReferencesList(QStringList iReferencesList);

  QMap<QString, QByteArray> getMemoryFiles();
  QStringList getDiskFilesList();

  void run();

  bool isSuccess();

  QString getErrorLog();

protected:

  int saveMode;
  QString saveDirectory;
  QStringList referencesList;

  QMap<QString, QByteArray> memoryFiles;
  QStringList diskFilesList;

  bool isSuccessFlag;

  QString errorLog;

  QStringList colsName; // Список имен колонок в таблице, определяется в конструкторе
  int downloadReferenceCol; // Номер колонки со ссылкой
  int downloadPercentCol; // Номер колонки с процентами загрузки, определяется в конструкторе

  QPushButton *cancelButton;
  QTableWidget *table;

  void setupUI();
  void setupSignals();
  void assembly();
};

#endif // DOWNLOADER_H
