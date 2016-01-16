#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "Downloader.h"

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
  downloadPercentCol=1; // Счет с нуля

  setupUI();
  setupSignals();
  assembly();
}


Downloader::~Downloader()
{
  delete table;
  delete cancelButton;
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
  QVBoxLayout *mainLayout=new QVBoxLayout(this);

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


void Downloader::setReferencesList(QStringList iReferencesList)
{
  referencesList=iReferencesList;
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
  exec();
}


bool Downloader::isSuccess()
{
  return isSuccessFlag;
}


QString Downloader::getErrorLog()
{
  return errorLog;
}
