#ifndef DOWNLOADER_H
#define DOWNLOADER_H

class QString;
class QStringList;
class QByteArray;
class QTableWidget;

class Downloader : public QDialog
{
public:
  Downloader();

  enum saveModeType{disk, memory};

  void setSaveMode(int iMode);
  void setSaveDirectory(QString iDir);
  void setReferencesList(QStringList iReferencesList);

  QMap<QString, QByteArray> getMemoryFiles();
  QStringList getDiskFilesList();

  bool isSuccess();

  QString getErrorLog();

protected:

  int saveMode;
  QString saveDirectory;
  QStringList referencesList;

  QMap<QString, QByteArray> memoryFiles;
  QStringList diskFilesList;

  bool isSuccess;

  QString errorLog;

  QTableWidget table;
  QButton

  void setupUI();
  void setupSignals();
  void assembly();
};

#endif // DOWNLOADER_H
