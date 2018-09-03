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
class QUrl;

class Downloader : public QDialog
{
  Q_OBJECT

public:
  Downloader();
  ~Downloader();

  enum downloadModeType{disk, memory};

  void setDownloadMode(int iMode);

  void setSaveDirectory(QString iDir);
  QString getSaveDirectory();

  void setReferencesList(QStringList iReferencesList);

  void setAboutText(QString iAboutText);
  QString getAboutText(void);

  QVector<QByteArray> getMemoryFiles() const;
  QMap<QString, QByteArray> getReferencesAndMemoryFiles() const;
  QMap<QString, QString> getReferencesAndFileNames() const;
  QStringList getDiskFilesList() const;
  QStringList getReferencesList() const;

  void run();

  bool isSuccess();

  QString getErrorLog();

signals:

  void cancelDownload(void);

private slots:

 void onFileDownloadFinished(QNetworkReply* pReply); // Слот, вызываемый при завершении загрузки очередного файла
 void onDownloadProgress(qint64 read, qint64 total);
 void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
 void onCancelClicked();

protected:

  int downloadMode;
  QString saveDirectory;
  QStringList referencesList;

  QVector<QByteArray> memoryFiles; // Содержимое файлов при скачивании в режиме memory
  QVector<QString> diskFilesNames; // Имена файлов на диске при скачивании в режиме disk

  bool isSuccessFlag;

  QString errorLog;

  int currentReferenceNum;

  QStringList colsName; // Список имен колонок в таблице, определяется в конструкторе
  int downloadReferenceCol; // Номер колонки со ссылкой
  int downloadPercentCol; // Номер колонки с процентами загрузки, определяется в конструкторе

  QNetworkAccessManager webManager; // Объект для работы с HTTP
  QNetworkReply *networkReply;

  QTableWidget *table;
  QPushButton *cancelButton;
  QLabel *aboutLabel;

  void setupUI();
  void setupSignals();
  void assembly();

  void startNextDownload();
  QUrl checkedRedirectUrl(const QUrl& possibleRedirectUrl) const;

  void addErrorLog(const QString text);

  void reconnectSignalsNetworkReply(QNetworkReply *networkReply);
};

#endif // DOWNLOADER_H
