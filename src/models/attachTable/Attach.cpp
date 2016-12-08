#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

#include "main.h"

#include "Attach.h"
#include "AttachTableData.h"
#include "models/recordTable/Record.h"
#include "libraries/crypt/CryptService.h"
#include "libraries/GlobalParameters.h"
#include "libraries/DiskHelper.h"

extern GlobalParameters globalParameters;


// Конструктор прикрепляемого файла
Attach::Attach(AttachTableData *iParentTable)
{
  init(iParentTable);
}


// Конструктор прикрепляемого файла с указанием типа прикрепления
// в настоящий момент есть два типа прикрепления - просто файл "file" или линк на файл "link"
Attach::Attach(QString iType, AttachTableData *iParentTable)
{
  if( !typeAvailableList().contains(iType) )
    criticalError("Incorrect attach type in Attach constructor: "+iType);

  setField("type", iType);

  init(iParentTable);
}


Attach::~Attach()
{
  fileContent.clear();
}


void Attach::init(AttachTableData *iParentTable)
{
  liteFlag=true; // По-умолчанию легкий объект
  parentTable=iParentTable;
  fileContent.clear();
}


void Attach::setParentTable(AttachTableData *iParentTable)
{
  parentTable=iParentTable;
}


// Допустимые имена полей
QStringList Attach::fieldAvailableList(void)
{
  return QStringList() << "id" << "fileName" << "link" << "type" << "crypt";
}


// Имена полей, которые шифруются
QStringList Attach::fieldCryptedList(void)
{
  return QStringList() << "fileName" << "link";
}


// Допустимые типы аттачей
QStringList Attach::typeAvailableList(void)
{
  return QStringList() << "file" << "link";
}


// На вход метода подается тег <file>
void Attach::setupDataFromDom(QDomElement iDomElement)
{
  QStringList fieldsName=fieldAvailableList();
  foreach( QString fieldName, fieldsName ) // Перебираются имена полей (XML-тегов)
    fields[fieldName]=iDomElement.attribute(fieldName); // Напрямую устанавливаются значения из XML файла
}


QDomElement Attach::exportDataToDom(QDomDocument *doc) const
{
  QDomElement elem=doc->createElement("file");

  QStringList fieldsName=fieldAvailableList();
  foreach( QString fieldName, fieldsName ) // Перебираются имена полей (XML-тегов)
    if( fields[fieldName].size()>0 )
      elem.setAttribute(fieldName, fields[fieldName]);

  return elem;
}


void Attach::exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const
{
  xmlWriter->writeStartElement("file");

  QStringList fieldsName=fieldAvailableList();
  foreach( QString fieldName, fieldsName ) // Перебираются имена полей (XML-тегов)
    if( fields[fieldName].size()>0 )
      xmlWriter->writeAttribute(fieldName, fields[fieldName]);

  xmlWriter->writeEndElement(); // Закрылся file
}


bool Attach::isEmpty() const
{
  // Заполненный аттач не может содержать пустой id
  if(fields.contains("id")==false || getField("id").length()==0)
    return true;
  else
    return false;
}


bool Attach::isLite() const
{
  return liteFlag;
}


void Attach::switchToLite()
{
  // Переключение возможно только из полновесного состояния
  if(liteFlag==true)
    criticalError("Can't switch attach to lite state. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  fileContent.clear();

  liteFlag=true;
}


void Attach::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true)
    criticalError("Unavailable switching attach object to fat state. Attach Id: "+getField("id")+" File name: "+getField("fileName"));

  liteFlag=false;
}


// Получение значения поля
// Метод возвращает расшифрованные данные, если запись была зашифрована
QString Attach::getField(QString name) const
{
  // Если имя поля недопустимо
  if(fieldAvailableList().contains(name)==false)
    criticalError("Attach::getField() : get unavailable field "+name);


  // ------------------------------------------
  // Проверки и действия для разных типов полей
  // ------------------------------------------

  // Если запрашивается линк на файл
  if(name=="link")
    if(fields["type"]!="link") // И тип аттача не является линком
      criticalError("Attach::getField() : Can't get link from non-link attach.");


  // -----------------------
  // Получение значения поля
  // -----------------------

  // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
  // то расшифровка невозможна
  if(fieldCryptedList().contains(name))
    if(fields.contains("crypt"))
      if(fields["crypt"]=="1")
        if(globalParameters.getCryptKey().length()==0)
          return QString();

  bool isCrypt=false;

  // Если имя поля принадлежит списку полей, которые могут шифроваться
  // и в наборе полей есть поле crypt
  // и поле crypt установлено в 1
  // и запрашиваемое поле не пустое (пустые данные невозможно расшифровать)
  if(fieldCryptedList().contains(name))
    if(fields.contains("crypt"))
      if(fields["crypt"]=="1")
        if(fields[name].length()>0)
          isCrypt=true;

  // Если поле не подлежит шифрованию
  if(isCrypt==false)
    return fields[name]; // Возвращается значение поля
  else
    return CryptService::decryptString(globalParameters.getCryptKey(), fields[name]); // Поле расшифровывается
}


// Установка значения поля
// Метод принимает незашифрованные данные, и шфирует их если запись является зашифрованой
void Attach::setField(QString name, QString value)
{
  // Если имя поля недопустимо
  if(fieldAvailableList().contains(name)==false)
    criticalError("Attach::setField() : set unavailable field "+name);


  // ------------------------------------------
  // Проверки и действия для разных типов полей
  // ------------------------------------------

  // Поле с типом аттача
  if(name=="type")
    if( !typeAvailableList().contains(value) )
      criticalError("Attach::setField() : Incorrect attach type : "+value);

  // Поле с именем файла
  if(name=="fileName")
    if(getField("type")=="link") // Если устанавливается имя файла для линка
      if(getField("fileName").length()>0 && value.length()>0) // Если имя уже было задано (при создании аттача), и новое имя не пустое
      {
        // Имя файла для линка менять нельзя
        showMessageBox(QObject::tr("Unable to rename a file which attached as a link."));
        return;
      }

  // Поле со ссылкой на файл (содержит путь к файлу, на который указывает линк)
  if(name=="link")
  {
    QFile tempFile(value);

    // Если файла, на который ссылается линк, не существует
    if(!tempFile.exists())
    {
      showMessageBox(QObject::tr("Bad link. File not found."));
      return;
    }
  }


  // -----------------------
  // Установка значения поля
  // -----------------------

  bool isCrypt=false;

  // Если имя поля принадлежит списку полей, которые могут шифроваться
  // и в наборе полей есть поле crypt
  // и поле crypt установлено в 1
  // и поле не пустое (пустые данные ненужно шифровать)
  if(fieldCryptedList().contains(name))
    if(fields.contains("crypt"))
      if(fields["crypt"]=="1")
        if(value.length()>0)
        {
          if(globalParameters.getCryptKey().length()>0)
            isCrypt=true;
          else
            criticalError("In Attach::setField() can not set data to crypt field "+name+". Password not setted");
        }

  // Если нужно шифровать, значение поля шифруется
  if(isCrypt==true)
    value=CryptService::encryptString(globalParameters.getCryptKey(), value);

  // Устанавливается значение поля
  fields.insert(name, value);
}


// Защищенный метод - Установка значения поля напрямую
// Используеся при шифрации-дешифрации данных аттача
// todo: подумать, может быть отказаться от этого метода
void Attach::setFieldSource(QString name, QString value)
{
  // Если имя поля недопустимо
  if(fieldAvailableList().contains(name)==false)
    criticalError("Attach::setField() : set unavailable field "+name);

  // Устанавливается значение поля
  fields.insert(name, value);
}


void Attach::pushFatDataToDisk()
{
  if(getField("type")!="file")
    criticalError("Can't push fat data for non-file attach.");

  if(liteFlag==true)
    criticalError("Can't push fat data for lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  QString innerFileName=getInnerFileName();
  QString innerDirName=parentTable->record->getFullDirName();

  QMap<QString, QByteArray> fileList;
  fileList[innerFileName]=fileContent;
  DiskHelper::saveFilesToDirectory(innerDirName, fileList);
}


// То же самое что и pushFatDataToDisk, только в нужную директорию
void Attach::pushFatDataToDirectory(QString dirName)
{
  if(getField("type")!="file")
    criticalError("Can't save to directory "+dirName+" non-file attach.");

  if(liteFlag==true)
    criticalError("Can't save to directory lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  QMap<QString, QByteArray> fileList;
  fileList[ getInnerFileName() ]=fileContent;
  DiskHelper::saveFilesToDirectory(dirName, fileList);
}


// Втаскивание в объект содержимого файла с диска
void Attach::popFatDataFromDisk()
{
  // Втаскивание возможно только в полновесном состоянии
  if(liteFlag==true)
    criticalError("Can't' pop data for lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  fileContent.clear();

  QString innerFileName=getInnerFileName();
  QString innerDirName=parentTable->record->getFullDirName();

  fileContent.append( (DiskHelper::getFilesFromDirectory(innerDirName, innerFileName)).value(innerFileName) );
}


// fullFileName - имя файла с полным путем, который нужно скопировать в каталог записи
bool Attach::copyFileToBase(QString iFileName)
{
  qDebug() << "Before check file. iFileName: "+iFileName;
  qDebug() << "Inner file name: "+getFullInnerFileName();

  // Проверка наличия исходного файла
  QFile file(iFileName);

  qDebug() << "Check file finish";

  if(file.exists()==false)
  {
    showMessageBox(QObject::tr("Can't open file %1. File not exists.").arg(iFileName));
    return false;
  }

  qDebug() << "Before real copy file.";

  bool result=file.copy( getFullInnerFileName() );

  qDebug() << "After real copy file.";

  if(result==false)
    showMessageBox(QObject::tr("Can't copy file %1. May be directory %2 not writable, or target file %3 already exists.").arg(iFileName).arg(getFullInnerDirName()).arg(getFullInnerFileName()));

  return result;
}


// Удаление файла с диска
void Attach::removeFile()
{
  if(getField("type")!="file")
    return;

  // Проверка наличия файла
  QFile file( getFullInnerFileName() );

  if(file.exists()==false)
  {
    showMessageBox(QObject::tr("Unable to delete the file %1 from disk: file not found.").arg( getFullInnerFileName() ));
    return;
  }

  file.setPermissions(QFile::ReadOther | QFile::WriteOther);
  file.remove();
}


QString Attach::constructFileName(const QString type, const QString id, const QString fileName)
{
  // Для файла
  if(type=="file")
  {
    // Выясняется расширение по видимому имени файла
    QFileInfo fileInfo( fileName );
    QString suffix=fileInfo.suffix();

    return id+"."+suffix;
  }

  // Для линка просто возвращается имя файла, куда указывает линк
  if(type=="link")
    return fileName;

  criticalError("Bad attach type in getInnerFileName():"+type);

  return "";
}


// Внутрисистемное имя файла (без пути)
// Внутрисистемное имя складывается из идентификатора аттача и расширения, взятого от имени файла
QString Attach::getInnerFileName() const
{
  return constructFileName(getField("type"), getField("id"), getField("fileName"));
}


// Внутрисистемное имя файла с путем
QString Attach::getFullInnerFileName() const
{
  if(getField("type")=="file") // Для файла
  {
    QString resultFileName=getFullInnerDirName()+"/"+getInnerFileName();
    return resultFileName;
  }

  if(getField("type")=="link") // Для линка
    return getField("link");

  criticalError("Bad attach type in getFullInnerFileName():"+getField("type"));

  return "";
}


// Внутрисистемное имя файла с абсолютным путем
QString Attach::getAbsoluteInnerFileName() const
{
  QFileInfo fileInfo( getFullInnerFileName() );

  return fileInfo.absoluteFilePath();
}


// Внутрисистемный путь к файлу (полный)
QString Attach::getFullInnerDirName() const
{
  return parentTable->record->getFullDirName();
}


// Размер аттача в байтах
qint64 Attach::getFileSize() const
{
  QString tempFileName;

  if(fields["type"]=="file")
  {
    QString recordDir=parentTable->record->getFullDirName();
    tempFileName=recordDir+"/"+getInnerFileName();
  }

  if(fields["type"]=="link")
    tempFileName=getField("link");

  QFile tempFile(tempFileName);

  if(!tempFile.exists())
    return 0;
  else
    return tempFile.size();
}


// Шифрация аттача на диске и в памяти
void Attach::encrypt(unsigned int area)
{
  // В этом методе важна последовательность действий,
  // чтобы не получилась ситуации, когда часть данных зашифрована,
  // а другая пытается их использовать, а флаг шифрации еще не установлен

  // Если аттач уже зашифрован, значит есть какая-то ошибка в логике выше
  if(getField("crypt")=="1")
    criticalError("Attach::encrypt() : Cant encrypt already encrypted attach.");


  // Шифруется файл
  if(area & areaFile)
    if(getField("type")=="file")
      CryptService::encryptFile(globalParameters.getCryptKey(), getFullInnerFileName());

  // Шифруется содержимое файла в памяти, если таковое есть
  if(area & areaMemory)
    if(liteFlag==false && fileContent.length()>0)
      fileContent=CryptService::encryptByteArray(globalParameters.getCryptKey(), fileContent);


  // Шифруются поля, которые подлежат шифрованию
  foreach( QString fieldName, fieldCryptedList() )
  {
    // У аттача с типом file не должно быть обращений к полю link (оно не должно использоваться)
    if(getField("type")=="file" && fieldName=="link")
      continue;

    // Если поле с указанным именем существует
    if(getField(fieldName).length()>0)
      setFieldSource(fieldName, CryptService::encryptString( globalParameters.getCryptKey(), getField(fieldName)));
  }

  // Устанавливается флаг, что запись зашифрована
  setField("crypt", "1");
}


// Расшифровка аттача на диске и в памяти
void Attach::decrypt(unsigned int area)
{
  // Если аттач не зашифрован, и происходит расшифровка, значит есть какая-то ошибка в логике выше
  if(getField("crypt")!="1")
    criticalError("Attach::decrypt() : Cant decrypt unencrypted attach.");

  // Расшифровывается файл
  if(area & areaFile)
    if(getField("type")=="file")
      CryptService::decryptFile(globalParameters.getCryptKey(), getFullInnerFileName());

  // Расшифровывается содержимое файла в памяти, если таковое есть
  if(area & areaMemory)
    if(liteFlag==false && fileContent.length()>0)
      fileContent=CryptService::decryptByteArray(globalParameters.getCryptKey(), fileContent);

  // Расшифровываются поля, которые подлежат шифрованию
  foreach( QString fieldName, fieldCryptedList() )
  {
    // У аттача с типом file не должно быть обращений к полю link (оно не должно использоваться)
    if(getField("type")=="file" && fieldName=="link")
      continue;

    // Если поле с указанным именем существует и содержит данные, оно расшифровывается из исходных зашифрованных данных
    if(getField(fieldName).length()>0)
      setFieldSource(fieldName, CryptService::decryptString( globalParameters.getCryptKey(), fields[fieldName]));
  }

  // Устанавливается флаг, что запись не зашифрована
  setField("crypt", ""); // Отсутсвие значения предпочтительней, так как тогда в XML-данные не будет попадать атрибут crypt="0"
}


// Расшифровка переданного DOM-элемента
// Метод статический, он не изменяет сам объект Attach
void Attach::decryptDomElement(QDomElement &iDomElement)
{
  if(iDomElement.hasAttribute("crypt") && iDomElement.attribute("crypt")=="1")
    foreach( QString fieldName, fieldCryptedList() ) // Перебираются зашифрованные поля
      if(iDomElement.hasAttribute(fieldName) && iDomElement.attribute(fieldName).length()>0)
      {
        QString decryptAttribute=CryptService::decryptString( globalParameters.getCryptKey(), iDomElement.attribute(fieldName));

        iDomElement.setAttribute(fieldName, decryptAttribute);
      }

  iDomElement.setAttribute("crypt", "0");
}
