#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QDialog>
#include <QMap>
#include <QNetworkAccessManager>

class QString;
class QStringList;
class QByteArray;
class QPushButton;
class QLabel;
class QTableWidget;
class QNetworkReply;


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

  void setAboutText(QString iAboutText);

  QMap<int, QByteArray> getMemoryFiles() const;
  QMap<QString, QByteArray> getReferencesAndMemoryFiles() const;
  QStringList getDiskFilesList() const;
  QStringList getReferencesList();


  void run();

  bool isSuccess();

  QString getErrorLog();


private slots:
 void onFileDownloadFinished(QNetworkReply* pReply); // Слот, вызываемый при завершении загрузки очередного файла


protected:

  int saveMode;
  QString saveDirectory;
  QStringList referencesList;

  QMap<int, QByteArray> memoryFiles;
  QStringList diskFilesList;

  bool isSuccessFlag;

  QString errorLog;

  int currentReferenceNum;

  QStringList colsName; // Список имен колонок в таблице, определяется в конструкторе
  int downloadReferenceCol; // Номер колонки со ссылкой
  int downloadPercentCol; // Номер колонки с процентами загрузки, определяется в конструкторе

  QNetworkAccessManager webManager; // Объект для работы с HTTP

  QTableWidget *table;
  QPushButton *cancelButton;
  QLabel *aboutLabel;

  void setupUI();
  void setupSignals();
  void assembly();

  void startNextDownload();

};

#endif // DOWNLOADER_H
