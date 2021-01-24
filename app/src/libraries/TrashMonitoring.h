#ifndef _TRASHMONITORING_H_
#define	_TRASHMONITORING_H_

#include <QWidget>
#include <QDir>


class TrashMonitoring
{

public:
 TrashMonitoring(void);
 virtual ~TrashMonitoring(void);

 void init(QString trashPath);
 
 // Функция, вызываемая после фактического добавления файла в корзину
 // принимает имя файла без пути к директории
 void addFile(QString filename);
 
 void update(void);

private:
 
 void removeOldesFile(void);
 
 QString path;
 QDir dir;
 unsigned int dirSize;
  
 struct FileData
 {
  QString fileName;
  unsigned int fileTime;
  unsigned int fileSize;
 };

 QList<FileData> filesTable;

};

#endif	/* _TRASHMONITORING_H_ */

