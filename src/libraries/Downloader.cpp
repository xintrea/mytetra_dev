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
  // Создается таблица скачиваемых файлов
  table=new QTableWidget(0, colsName.count(), this);

  // Задаются заголовки таблицы
  table->setHorizontalHeaderLabels(colsName);


  // Создается кнопка отмены загрузки
  cancelButton=new QPushButton(this);
  cancelButton->setText(tr("Cancel"));
}


void Downloader::setupSignals()
{
  connect(&webManager, SIGNAL (finished(QNetworkReply*)),
          this,        SLOT   (onFileDownloadFinished(QNetworkReply*)) );

  connect(cancelButton, SIGNAL (clicked()),
          this,         SIGNAL (reject()));
}


void Downloader::assembly()
{
  QVBoxLayout *mainLayout=new QVBoxLayout();

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
    table->setCellWidget(i, downloadPercentCol, progressBar);
  }
}


QMap<QString, QByteArray> Downloader::getMemoryFiles() const
{
  return QMap<QString, QByteArray>();
}


QStringList Downloader::getDiskFilesList() const
{
  return QStringList();
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


// Запуск загрузки с первой ссылки
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
  if(saveMode==memory)
  {
    memoryFiles[currentReferenceNum]=reply->readAll(); // Загруженные данные сохраняются
  }

  if(saveMode==disk)
  {
    qDebug() << "Development in process...";
  }

  reply->deleteLater();

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
