#include <QWidget>
#include <QDateTime>
#include <QFileInfo>
#include <QMessageBox>

#include "TrashMonitoring.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/helpers/DebugHelper.h"

extern AppConfig mytetraConfig;
extern QObject *pMainWindow;


TrashMonitoring::TrashMonitoring(void)
{
 
}


TrashMonitoring::~TrashMonitoring(void)
{

}


void TrashMonitoring::init(const QString trashPath)
{
    // Инит объекта директории с указанным путем
    m_dir.setPath(trashPath);
    if(!m_dir.exists())
        criticalError("Can not open trash directory "+trashPath);

    m_path=trashPath; // Имя директории запоминается

    // Размер директории
    m_dirSize=0;

    // Получение списка информации о файлах
    QFileInfoList fileInfoList=m_dir.entryInfoList(QDir::Files, QDir::Time);

    // Перебор всех файлов в полученном списке
    for(int i=0;i<fileInfoList.size();i++)
    {
        QString      fileName=fileInfoList.at(i).fileName();

        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        unsigned int fileTime=fileInfoList.at(i).birthTime().toTime_t();
        #else
        unsigned int fileTime=fileInfoList.at(i).created().toTime_t();
        #endif

        unsigned int fileSize=static_cast<unsigned int>( fileInfoList.at(i).size() );

        // Директории с именами "." и ".." обрабатывать не нужно
        if(fileName=="." || fileName=="..")
            continue;

        // Информация о файле добавляется в таблицу
        this->appendFileToList(fileName, fileTime, fileSize, AppendFileMode::Tail);
    }

    // qDebug() << "In init trash " << filesTable.size() << "files";
}


// Функция должна вызываться после фактического добавления файла в корзину
// принимает имя файла без пути к директории
void TrashMonitoring::addFile(const QString fileName)
{
 // Выясняется время создания файла берется текущее, особой точности не нужно
 unsigned int fileTime=(QDateTime::currentDateTime()).toTime_t();

 // Выясняется размер файла
 QFile currentFile(m_path+"/"+fileName);
 unsigned int fileSize=currentFile.size();

 // Информация о файле добавляется в таблицу
 this->appendFileToList(fileName, fileTime, fileSize, AppendFileMode::Head);
 
 update();
}


void TrashMonitoring::update(void)
{
    // Исключается наиболее старый файл пока выполняется
    // условие что количество файлов слишком велико или
    // суммарный размер файлов превышает предельно допустимый размер корзины
    while(m_filesList.size() > mytetraConfig.get_trashmaxfilecount() ||
          m_dirSize > mytetraConfig.get_trashsize()*1000000)
    {
        if(m_filesList.size()==1)
        {
            break; // Оставляется последний файл, какого бы размера он не был
        }
        else
        {
            removeOldesFile();
        }
    }
}


void TrashMonitoring::appendFileToList(const QString fileName,
                                       const unsigned int fileTime,
                                       const unsigned int fileSize,
                                       const AppendFileMode appendFileMode)
{
    // Если по каким-то причинам добавляется файл, который уже есть в списке,
    // то список не изменяется, чтобы не было попытки повторного удаления
    // файла, который уже был удален
    if ( m_availableFilesName.contains(fileName))
    {
        qDebug() << "Double append file to trash monitoring: " << fileName;
        return;
    }

    FileData currentFileData;
    currentFileData.fileName=fileName;
    currentFileData.fileTime=fileTime;
    currentFileData.fileSize=fileSize;

    // Увеличивается подсчитываемый размер директории
    m_dirSize=m_dirSize+fileSize;

    // Список имен обслуживаемых файлов пополняется
    m_availableFilesName << fileName;

    // Пополняется списк файлов
    switch (appendFileMode)
    {
        case AppendFileMode::Head:
            m_filesList.insert(0, currentFileData);
            break;

        case AppendFileMode::Tail:
            m_filesList << currentFileData;
            break;

        default:
            qDebug() << "Incorrect append file mode to tresh monitoring: " << appendFileMode;
            break;
    }
}


void TrashMonitoring::removeOldesFile(void)
{
  QString fileName=m_path+"/"+m_filesList.last().fileName;
  
  qDebug() << "Remove file " << fileName << " from trash";
  
  if(QFile::remove(fileName)) // Файл физически удаляется
  {
    // Расчетный размер директории уменьшается на размер файла
    m_dirSize=m_dirSize-m_filesList.last().fileSize;
    
    // Файл удаляется из списка
    m_filesList.removeLast();

    // Имя файла удаляется из контролирующего списка
    m_availableFilesName.remove(fileName);
  }
  else
  {
    // Не удаление файла в корзине - это не повод останавливать работу программы,
    // Просто выводится сообщение что нужно о проблеме оповестить разработчиков
    QMessageBox::information(qobject_cast<QWidget *>(pMainWindow),
        QObject::tr("Important information"),
        QObject::tr("In trash monitoring can not delete file:\n\n"
                    "%1\n\n"
                    "Please report about this problem to the developers.").arg(fileName),
        QMessageBox::Ok);
  }
}

