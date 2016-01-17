#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QProgressBar>

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


QMap<QString, QByteArray> Downloader::getMemoryFiles()
{
  return QMap<QString, QByteArray>();
}


QStringList Downloader::getDiskFilesList()
{
  return QStringList();
}


void Downloader::run()
{
  if(referencesList.count()>0)
    exec();
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
