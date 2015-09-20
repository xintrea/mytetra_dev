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
    critical_error("Incorrect attach type in Attach constructor: "+iType);

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


// Допустимые имена полей
QStringList Attach::fieldAvailableList(void) const
{
  return QStringList() << "id" << "fileName" << "link" << "type" << "crypt";
}


// Имена полей, которые шифруются
QStringList Attach::fieldCryptedList(void) const
{
  return QStringList() << "fileName" << "link";
}


// Допустимые типы аттачей
QStringList Attach::typeAvailableList(void) const
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
    critical_error("Can't switch attach to lite state. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  fileContent.clear();

  liteFlag=true;
}


void Attach::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true)
    critical_error("Unavailable switching attach object to fat state. Attach Id: "+getField("id")+" File name: "+getField("fileName"));

  liteFlag=false;
}


// Получение значения поля
// Метод возвращает расшифрованные данные, если запись была зашифрована
QString Attach::getField(QString name) const
{
  // Если имя поля недопустимо
  if(fieldAvailableList().contains(name)==false)
    critical_error("Attach::getField() : get unavailable field "+name);


  // ------------------------------------------
  // Проверки и действия для разных типов полей
  // ------------------------------------------

  // Если запрашивается линк на файл
  if(name=="link")
    if(fields["type"]!="link") // И тип аттача не является линком
      critical_error("Attach::getField() : Can't get link from non-link attach.");


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
    critical_error("Attach::setField() : set unavailable field "+name);


  // ------------------------------------------
  // Проверки и действия для разных типов полей
  // ------------------------------------------

  // Поле с типом аттача
  if(name=="type")
    if( !typeAvailableList().contains(value) )
      critical_error("Attach::setField() : Incorrect attach type : "+value);

  // Поле с именем файла
  if(name=="fileName")
    if(getField("type")=="link") // Если устанавливается имя файла для линка
      if(getField("fileName").length()>0 && value.length()>0) // Если имя уже было задано (при создании аттача), и новое имя не пустое
      {
        // Имя файла для линка менять нельзя
        showMessageBox(QObject::tr("Can't modify file name for link type attach."));
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
            critical_error("In Attach::setField() can not set data to crypt field "+name+". Password not setted");
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
    critical_error("Attach::setField() : set unavailable field "+name);

  // Устанавливается значение поля
  fields.insert(name, value);
}


void Attach::pushFatDataToDisk()
{
  if(getField("type")!="file")
    critical_error("Can't push fat data for non-file attach.");

  if(liteFlag==true)
    critical_error("Can't push fat data for lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  QString innerFileName=getInnerFileName();
  QString innerDirName=parentTable->record->getFullDirName();

  QMap<QString, QByteArray> fileList;
  fileList[innerFileName]=fileContent;
  save_files_to_directory(innerDirName, fileList);
}


// То же самое что и pushFatDataToDisk, только в нужную директорию
void Attach::pushFatDataToDirectory(QString dirName)
{
  if(getField("type")!="file")
    critical_error("Can't save to directory "+dirName+" non-file attach.");

  if(liteFlag==true)
    critical_error("Can't save to directory lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  QMap<QString, QByteArray> fileList;
  fileList[ getInnerFileName() ]=fileContent;
  save_files_to_directory(dirName, fileList);
}


// Втаскивание в объект содержимого файла с диска
void Attach::popFatDataFromDisk()
{
  // Втаскивание возможно только в полновесном состоянии
  if(liteFlag==true)
    critical_error("Can't' pop data for lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  fileContent.clear();

  QString innerFileName=getInnerFileName();
  QString innerDirName=parentTable->record->getFullDirName();

  fileContent.append( (get_files_from_directory(innerDirName, innerFileName)).value(innerFileName) );
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
    showMessageBox(QObject::tr("Can't delete file %1 on disk. File not exists.").arg( getFullInnerFileName() ));
    return;
  }

  file.setPermissions(QFile::ReadOther | QFile::WriteOther);
  file.remove();
}


// Внутрисистемное имя файла (без пути)
// Внутрисистемное имя складывается из идентификатора аттача и расширения, взятого от имени файла
QString Attach::getInnerFileName() const
{
  if(getField("type")=="file") // Для файла
  {
    // Выясняется расширение по видимому имени файла
    QFileInfo fileInfo( getField("fileName") );
    QString suffix=fileInfo.suffix();

    QString innerFileName=getField("id")+"."+suffix;

    return innerFileName;
  }

  if(getField("type")=="link") // Для линка просто возвращается имя файла, куда указывает линк
    return getField("fileName");

  critical_error("Bad attach type in getInnerFileName():"+getField("type"));

  return "";
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

  critical_error("Bad attach type in getFullInnerFileName():"+getField("type"));

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


void Attach::encrypt()
{
  // В этом методе важна последовательность действий,
  // чтобы не получилась ситуации, когда часть данных зашифрована,
  // а другая пытается их использовать, а флаг шифрации еще не установлен

  // Если аттач уже зашифрован, значит есть какая-то ошибка в логике выше
  if(getField("crypt")=="1")
    critical_error("Attach::encrypt() : Cant encrypt already encrypted attach.");


  // Шифруется файл
  if(getField("type")=="file")
    CryptService::encryptFile(globalParameters.getCryptKey(), getFullInnerFileName());

  // Шифруется содержимое файла в памяти, если таковое есть
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


void Attach::decrypt()
{
  // Если аттач не зашифрован, и происходит расшифровка, значит есть какая-то ошибка в логике выше
  if(getField("crypt")!="1")
    critical_error("Attach::decrypt() : Cant decrypt unencrypted attach.");

  // Расшифровывается файл
  if(getField("type")=="file")
    CryptService::decryptFile(globalParameters.getCryptKey(), getFullInnerFileName());

  // Расшифровывается содержимое файла в памяти, если таковое есть
  if(liteFlag==false && fileContent.length()>0)
    fileContent=CryptService::decryptByteArray(globalParameters.getCryptKey(), fileContent);

  // Расшифровываются поля, которые подлежат шифрованию
  foreach( QString fieldName, fieldCryptedList() )
  {
    // У аттача с типом file не должно быть обращений к полю link (оно не должно использоваться)
    if(getField("type")=="file" && fieldName=="link")
      continue;

    // Если поле с указанным именем существует
    if(getField(fieldName).length()>0)
      setFieldSource(fieldName, CryptService::decryptString( globalParameters.getCryptKey(), getField(fieldName)));
  }

  // Устанавливается флаг, что запись не зашифрована
  setField("crypt", ""); // Отсутсвие значения предпочтительней, так как тогда в XML-данные не будет попадать атрибут crypt="0"
}
