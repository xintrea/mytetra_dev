#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHeaderView>
#include <QLabel>
#include <QUrl>
#include <QFileInfo>

#include "Downloader.h"
#include "main.h"


Downloader::Downloader()
{
  downloadMode=disk;
  saveDirectory="";
  referencesList.clear();
  memoryFiles.clear();
  diskFilesNames.clear();
  isSuccessFlag=false;
  errorLog="";
  currentReferenceNum=-1;

  colsName << tr("Url") << tr("%");
  downloadReferenceCol=0;
  downloadPercentCol=1;

  setupUI();
  setupSignals();
  assembly();
}


Downloader::~Downloader()
{

}


void Downloader::setupUI()
{
  // Текст описания
  aboutLabel=new QLabel(this);
  aboutLabel->hide();

  // Создается таблица скачиваемых файлов
  table=new QTableWidget(0, colsName.count(), this);

  // Задаются заголовки таблицы
  table->setHorizontalHeaderLabels(colsName);

  table->horizontalHeader()->setSectionResizeMode(downloadReferenceCol, QHeaderView::Stretch);
  table->setColumnWidth(downloadPercentCol, 128);


  // Создается кнопка отмены загрузки
  cancelButton=new QPushButton(this);
  cancelButton->setText(tr("Cancel"));
}


void Downloader::setupSignals()
{
  connect(&webManager, SIGNAL (finished(QNetworkReply*)),
          this,        SLOT   (onFileDownloadFinished(QNetworkReply*)) );

  connect(&webManager, SIGNAL (sslErrors(QNetworkReply *, const QList<QSslError> &)),
        this, SLOT (onSslErrors(QNetworkReply *, const QList<QSslError> &)) );

  connect(cancelButton, SIGNAL (clicked()),
          this,         SLOT (onCancelClicked()));
}


void Downloader::assembly()
{
  QVBoxLayout *mainLayout=new QVBoxLayout();

  mainLayout->addWidget(aboutLabel);
  mainLayout->addWidget(table);
  mainLayout->addWidget(cancelButton);

  setLayout(mainLayout);
}


void Downloader::setDownloadMode(int iMode)
{
  downloadMode=iMode;
}


void Downloader::setSaveDirectory(QString iDir)
{
  saveDirectory=iDir;
}


QString Downloader::getSaveDirectory()
{
  return saveDirectory;
}


// Установка списка ссылок для закачивания
void Downloader::setReferencesList(QStringList iReferencesList)
{
  referencesList=iReferencesList;

  if(downloadMode==disk)
  {
    diskFilesNames.clear();
    diskFilesNames.resize(referencesList.size());
  }

  if(downloadMode==memory)
  {
    memoryFiles.clear();
    memoryFiles.resize(referencesList.size());
  }

  // Перебирается список ссылок
  for(int i=0; i<referencesList.count(); i++)
  {
    // Добавляется строка на экране
    table->insertRow(i);

    // Заполняется столбец со ссылкой
    QTableWidgetItem *referenceItem=new QTableWidgetItem(referencesList.at(i));
    table->setItem(i, downloadReferenceCol, referenceItem);

    // Создается виджет линейки наполняемости
    QProgressBar *progressBar=new QProgressBar();
    progressBar->setRange(0,100);
    table->setCellWidget(i, downloadPercentCol, progressBar);

    // Генерируются имена файлов на диске, куда будут сохранятся выкачиваемые файлы
    if(downloadMode==disk)
    {
      QUrl fileUrl( referencesList.at(i) );
      QString fileName=fileUrl.fileName(); // Имя файла из URL, содержащее расширение

      QFileInfo fileInfo(fileName);
      QString fileExtention = fileInfo.completeSuffix();

      diskFilesNames[i]=getUnicalId()+"."+fileExtention;
    }
  }
}


void Downloader::setAboutText(QString iAboutText)
{
  aboutLabel->setText(iAboutText);

  if(iAboutText.length()>0)
    aboutLabel->show();
  else
    aboutLabel->hide();
}


QString Downloader::getAboutText()
{
  return aboutLabel->text();
}


QVector<QByteArray> Downloader::getMemoryFiles() const
{
  if(downloadMode==disk)
    criticalError("Cant execute Downloader::getMemoryFiles() for disk mode. Current Download instance use memory mode.");

  return memoryFiles;
}


QMap<QString, QByteArray> Downloader::getReferencesAndMemoryFiles() const
{
  if(downloadMode==disk)
    criticalError("Cant execute Downloader::getReferencesAndMemoryFiles() for disk mode. Current Download instance use memory mode.");

  QMap<QString, QByteArray> tempReferencesAndMemoryFiles;

  for(int i=0; i<memoryFiles.count(); ++i)
    tempReferencesAndMemoryFiles[ referencesList.at(i) ]=memoryFiles.value(i);

  return tempReferencesAndMemoryFiles;
}


QMap<QString, QString> Downloader::getReferencesAndFileNames() const
{
  if(downloadMode==memory)
    criticalError("Cant execute Downloader::getReferencesAndFileNames() for memory mode. Current Download instance use disk mode.");

  QMap<QString, QString> tempReferencesAndFileNames;

  for(int i=0; i<referencesList.count(); ++i)
    tempReferencesAndFileNames[ referencesList.at(i) ]=diskFilesNames.value(i);

  return tempReferencesAndFileNames;
}


QStringList Downloader::getDiskFilesList() const
{
  if(downloadMode==memory)
    criticalError("Cant execute Downloader::getDiskFilesList() for memory mode. Current Download instance use disk mode.");

  return QStringList();
}


QStringList Downloader::getReferencesList() const
{
  return referencesList;
}


void Downloader::run()
{
  if(referencesList.count()>0)
  {
    startNextDownload();

    exec(); // Запускается цикл обработки сигналов-слотов для данного класса
  }
  else
    criticalError("Running downloader with empty references list.");
}


bool Downloader::isSuccess()
{
  return isSuccessFlag;
}


QString Downloader::getErrorLog()
{
  return errorLog;
}


void Downloader::reconnectSignalsNetworkReply(QNetworkReply *networkReply)
{
  disconnect( networkReply, SIGNAL(downloadProgress(qint64, qint64)), 0, 0 );
  connect(    networkReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)) );

  disconnect( this, SIGNAL(cancelDownload()), 0, 0 );
  connect(    this, SIGNAL(cancelDownload()), networkReply, SLOT(abort()) );
}


// Запуск загрузки очередной ссылки
void Downloader::startNextDownload()
{
  // Определяется, какую ссылку надо загружать
  currentReferenceNum++;
  QString currentReference=referencesList.at( currentReferenceNum );

  // Запуск загрузки
  qDebug() << "Start download" << currentReference;
  QNetworkRequest request(currentReference);

  // В конце загрузки автоматически будет вызван слот onFileDownloadFinished() ( см. связывание сингнал-слот в setupSignals() )
  networkReply=webManager.get(request);
  reconnectSignalsNetworkReply(networkReply);
}


// Слот, вызываемый в конце загрузки очередного файла
void Downloader::onFileDownloadFinished(QNetworkReply *reply)
{
  bool enableNextDownload=true;

  // Если при получении ответа небыло ошибок сети
  if(reply->error() == QNetworkReply::NoError)
  {
    // Определение, есть ли перенаправление (редирект) в ответе сервера
    QVariant possibleRedirectUrl=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QUrl urlRedirectedTo=checkedRedirectUrl( possibleRedirectUrl.toUrl() );

    // Если есть перенаправление
    if(!urlRedirectedTo.isEmpty())
    {
      qDebug() << "Redirected to " << urlRedirectedTo.toString();

      QNetworkRequest request(urlRedirectedTo);
      networkReply=webManager.get(request); // В конце загрузки будет вызван слот onFileDownloadFinished()
      reconnectSignalsNetworkReply(networkReply);

      enableNextDownload=false;
    }
    else
    {
      // Иначе перенаправления нет, и значит в ответе содержится принятый файл

      // Загруженные данные сохраняются в память
      if(downloadMode==memory)
        memoryFiles[currentReferenceNum]=reply->readAll();

      // Загруженные данные сохраняются на диск
      if(downloadMode==disk)
      {
        QString fileName=saveDirectory+"/"+diskFilesNames[currentReferenceNum];

        QFile file( fileName );
        if( file.open( QIODevice::WriteOnly ) )
        {
          file.write( reply->readAll() );
          file.close();
        }
        else
          showMessageBox( tr("Has problem with save file to directory %1").arg(saveDirectory) );
      }
    }
  }
  else
  {
    qDebug() << reply->errorString();
    addErrorLog("reply->errorString()");
    isSuccessFlag=false; // Если с одним файлом была проблема, флаг успешной загрузки снимается для всех
  }


  // Если разрешена загрузка следующей ссылки
  if(enableNextDownload)
  {
    // Если при загрузке текущей ссылки была какая-то ошибка, строка выделяется цветом
    if(reply->error() != QNetworkReply::NoError)
      table->item(currentReferenceNum, downloadReferenceCol)->setBackground( QApplication::palette().color(QPalette::Mid) );

    // На экране отмечается, что текущая ссылка загружена полностью
    qobject_cast<QProgressBar *>(table->cellWidget(currentReferenceNum, downloadPercentCol))->setValue(100);

    // Если еще не все ссылки загружены
    if(currentReferenceNum<(referencesList.count()-1))
      startNextDownload();
    else
    {
      // Иначе все загрузки завершены

      qDebug() << "All download successfull";

      isSuccessFlag=true;

      emit accept(); // Программно закрывается окно диалога, как будто нажали Ok
    }
  }

  reply->deleteLater();
}


void Downloader::onDownloadProgress(qint64 read, qint64 total)
{
  qint64 percent=0;

  if(total>0) // На 0 делить нельзя, а -1 означает что размер данных не известен
    percent=(read * 100) / total;

  // На экране изменяется процент загрузки ссылки
  qobject_cast<QProgressBar *>(table->cellWidget(currentReferenceNum, downloadPercentCol))->setValue(percent);
}


// Метод, отбрасывающий повторяющиеся ссылки при редиректе
QUrl Downloader::checkedRedirectUrl(const QUrl& possibleRedirectUrl) const
{
  static QUrl oldRedirectUrl;
  QUrl redirectUrl;

  if(!possibleRedirectUrl.isEmpty() &&
     possibleRedirectUrl != oldRedirectUrl) {
    redirectUrl = possibleRedirectUrl;
  }

  oldRedirectUrl=redirectUrl;

  return redirectUrl;
}


void Downloader::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
  Q_UNUSED( errors );

  reply->ignoreSslErrors();
}


void Downloader::onCancelClicked()
{
  emit cancelDownload();

  addErrorLog("Cancel download by user.");

  isSuccessFlag=false;

  reject();
}


void Downloader::addErrorLog(const QString text)
{
  if(errorLog.length()>0)
    errorLog+="\n";

  errorLog+=text;
}
