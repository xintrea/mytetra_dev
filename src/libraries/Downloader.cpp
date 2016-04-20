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

#include "Downloader.h"
#include "main.h"


Downloader::Downloader()
{
  saveMode=disk;
  saveDirectory="";
  referencesList.clear();
  memoryFiles.clear();
  diskFilesList.clear();
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


void Downloader::setSaveMode(int iMode)
{
  saveMode=iMode;
}


void Downloader::setSaveDirectory(QString iDir)
{
  saveDirectory=iDir;
}


// Установка списка ссылок для закачивания
void Downloader::setReferencesList(QStringList iReferencesList)
{
  referencesList=iReferencesList;

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


QMap<int, QByteArray> Downloader::getMemoryFiles() const
{
  return memoryFiles;
}


QMap<QString, QByteArray> Downloader::getReferencesAndMemoryFiles() const
{
  QMap<QString, QByteArray> tempReferencesAndMemoryFiles;

  for(int i=0; i<memoryFiles.count(); i++)
    tempReferencesAndMemoryFiles[ referencesList.at(i) ]=memoryFiles.value(i);

  return tempReferencesAndMemoryFiles;
}


QStringList Downloader::getDiskFilesList() const
{
  return QStringList();
}


QStringList Downloader::getReferencesList()
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


// Запуск загрузки очередной ссылки
void Downloader::startNextDownload()
{
  // Определяется, какую ссылку надо загружать
  currentReferenceNum++;
  QString currentReference=referencesList.at( currentReferenceNum );

  // Запуск загрузки
  qDebug() << "Start download" << currentReference;
  QNetworkRequest request(currentReference);
  webManager.get(request); // В конце загрузки будет вызван слот onFileDownloadFinished()
}


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
      webManager.get(request); // В конце загрузки будет вызван слот onFileDownloadFinished()

      enableNextDownload=false;
    }
    else
    {
      // Иначе перенаправления нет, и значит в ответе содержится принятый файл

      if(saveMode==memory)
        memoryFiles[currentReferenceNum]=reply->readAll(); // Загруженные данные сохраняются

      if(saveMode==disk)
        qDebug() << "Save dowload file to disk: Development in process...";
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
