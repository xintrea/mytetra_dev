#include <QWidget>
#include <QDateTime>
#include <QFileInfo>

#include "main.h"
#include "TrashMonitoring.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/helpers/DebugHelper.h"

extern AppConfig mytetraConfig;


TrashMonitoring::TrashMonitoring(void)
{
 
}


TrashMonitoring::~TrashMonitoring(void)
{

}


void TrashMonitoring::init(QString trashPath)
{
    // Инит объекта директории с указанным путем
    dir.setPath(trashPath);
    if(!dir.exists())
        criticalError("Can not open trash directory "+trashPath);

    path=trashPath; // Имя директории запоминается

    // Размер директории
    dirSize=0;

    // Получение списка информации о файлах
    QFileInfoList fileInfoList=dir.entryInfoList(QDir::Files, QDir::Time);

    // Перебор всех файлов в полученном списке
    for(int i=0;i<fileInfoList.size();i++)
    {
        QString      fileName=fileInfoList.at(i).fileName();
        // Директории с именами "." и ".." обрабатывать не нужно
        if(fileName=="." || fileName=="..")
            continue;

        unsigned int fileSize=fileInfoList.at(i).size();

        // Увеличивается подсчитываемый размер директории
        dirSize=dirSize+fileSize;

        // Информация о файле добавляется в таблицу
        FileData currentFileData = {fileName, fileSize};
        filesTable << currentFileData;
    }

    // qDebug() << "In init trash " << filesTable.size() << "files";
}


// Функция должна вызываться после фактического добавления файла в корзину
// принимает имя файла без пути к директории
void TrashMonitoring::addFile(QString filename)
{
 // Выясняется размер файла
 QFile currentFile(path+"/"+filename);
 unsigned int fileSize=currentFile.size();

 // Увеличивается подсчитываемый размер директории
 dirSize=dirSize+fileSize;

 // Информация о файле добавляется в таблицу
 FileData currentfiledata;
 currentfiledata.fileName=filename;
 currentfiledata.fileSize=fileSize;
 filesTable.insert(0,currentfiledata);
 
 update();
}


void TrashMonitoring::update(void)
{
 // Исключается наиболее старый файл пока выполняется 
 // условие что количество файлов слишком велико или
 // суммарный размер файлов превышает предельно допустимый размер корзины
 while(filesTable.size() > mytetraConfig.get_trashmaxfilecount() ||
       dirSize > mytetraConfig.get_trashsize()*1000000)
   if(filesTable.size()==1) // Оставляется последний файл, какого бы размера он не был
     break;
   else
    removeOldesFile();
}


void TrashMonitoring::removeOldesFile(void)
{
  QString fileName=path+"/"+filesTable.last().fileName;
  
  qDebug() << "Remove file " << fileName << " from trash";
  
  if(QFile::remove(fileName)) // Файл физически удаляется
   {
    // Расчетный размер директории уменьшается на размер файла
    dirSize=dirSize-filesTable.last().fileSize;
    
    // Файл удаляется из списка
    filesTable.removeLast();
   } 
  else
   {
    criticalError("In trash monitoring can not delete file "+fileName);
    exit(0);
   }
}

