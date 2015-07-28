#include "main.h"
#include "Record.h"

extern AppConfig mytetraConfig;
extern FixedParameters fixedParameters;
extern GlobalParameters globalParameters;


Record::Record()
{
  liteFlag=true; // По-умолчанию объект легкий
}


Record::~Record()
{

}


bool Record::isNull() const
{
  // Заполненная запись не может содержать пустые свойства
  if(fieldList.count()==0)
    return true;
  else
    return false;
}


bool Record::isLite() const
{
  return liteFlag;
}


void Record::switchToLite()
{
  pictureFiles.clear();
  attachFiles.clear();

  liteFlag=true;
}


void Record::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true || pictureFiles.count()>0 || attachFiles.count()>0)
  {
    critical_error("Unavailable switching record object to fat state. "+getIdAndNameAsString());
    return;
  }

  liteFlag=false;
}


QString Record::getIdAndNameAsString() const
{
  QString id, name;

  if(fieldList.contains("id"))
    id=fieldList["id"];

  if(fieldList.contains("name"))
    name=fieldList["name"];

  return "Record ID: "+id+" Name: "+name;
}



QString Record::getField(QString name) const
{
  // Если имя поля недопустимо
  if(fixedParameters.isRecordFieldAvailable(name)==false)
    critical_error("RecordTableData::getField() : get unavailable field "+name);

  QString result="";

  // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
  // то расшифровка невозможна
  if(fixedParameters.recordFieldCryptedList().contains(name))
    if(fieldList.contains("crypt"))
      if(fieldList["crypt"]=="1")
        if(globalParameters.getCryptKey().length()==0)
          return QString();


  // Если поле с таким названием есть
  if(fieldList.contains(name))
  {
    // Нужно определить, зашифровано поле или нет

    bool isCrypt=false;

    // Если имя поля принадлежит списку полей, которые могут шифроваться
    // и в наборе полей есть поле crypt
    // и поле crypt установлено в 1
    // и запрашиваемое поле не пустое (пустые данные невозможно расшифровать)
    if(fixedParameters.recordFieldCryptedList().contains(name))
      if(fieldList.contains("crypt"))
        if(fieldList["crypt"]=="1")
          if(fieldList[name].length()>0)
            isCrypt=true;

    // Если поле не подлежит шифрованию
    if(isCrypt==false)
      result=fieldList[name]; // Возвращается значение поля
    else
    {
      // Поле расшифровывается
      result=decryptString(globalParameters.getCryptKey(), fieldList[name]);
    }
  }

  // qDebug() << "RecordTableData::get_field : pos" << pos <<"name"<<name<<"value"<<result;

  return result;
}


void Record::setField(QString name, QString value)
{
  // Если имя поля недопустимо
  if(fixedParameters.isRecordFieldAvailable(name)==false)
    critical_error("In RecordTableData::setField() unavailable field name "+name+" try set to "+value);

  bool isCrypt=false;

  // Если имя поля принадлежит списку полей, которые могут шифроваться
  // и в наборе полей есть поле crypt
  // и поле crypt установлено в 1
  // и поле не пустое (пустые данные ненужно шифровать)
  if(fixedParameters.recordFieldCryptedList().contains(name))
    if(fieldList.contains("crypt"))
      if(fieldList["crypt"]=="1")
        if(value.length()>0)
        {
          if(globalParameters.getCryptKey().length()>0)
            isCrypt=true;
          else
            critical_error("In RecordTableData::set_field() can not set data to crypt field "+name+". Password not setted");
        }


  // Если нужно шифровать, поле шифруется
  if(isCrypt==true)
    value=encryptString(globalParameters.getCryptKey(), value);

  // Устанавливается значение поля
  fieldList.insert(name, value);

  // qDebug() << "RecordTableData::set_field : pos" << pos <<"name"<<name<<"value"<<value;
}


// Получение значений всех полей
// Поля, которые могут быть у записи, но не заданы, не передаются
// Поля, которые зашифрованы, расшифровываются
QMap<QString, QString> Record::getFieldList() const
{
  // Список имен инфополей
  QStringList fieldNames=fixedParameters.recordFieldAvailableList();

  QMap<QString, QString> resultFieldList;

  // Проверяется, используется ли шифрование
  bool isCrypt=false;

  if(fieldList.contains("crypt"))
    if(fieldList["crypt"]=="1")
      isCrypt=true;


  // Перебираются названия полей
  for(int i=0;i<fieldNames.size();++i)
  {
    QString currName=fieldNames.at(i);

    // Если поле с таким именем существует
    if(fieldList.contains( currName ))
    {
      QString result="";

      if(isCrypt==false)
        result=fieldList[currName]; // Напрямую значение поля
      else
      {
        // Присутствует шифрование

        // Если поле не подлежит шифрованию (не все поля в зашифрованной ветке шифруются. Например, не шифруется ID записи)
        if(fixedParameters.recordFieldCryptedList().contains(currName)==false)
          result=fieldList[currName]; // Напрямую значение поля
        else
          if(globalParameters.getCryptKey().length()>0 &&
             fixedParameters.recordFieldCryptedList().contains(currName))
            result=decryptString(globalParameters.getCryptKey(), fieldList[currName]); // Расшифровывается значение поля
      }

      resultFieldList[currName]=result;
    }
  }

  qDebug() << "Record::getFieldList() : "<<resultFieldList;

  return resultFieldList;
}


QMap<QString, QString> Record::getAttachList() const
{

}


void Record::setAttachList(QMap<QString, QString> list)
{

}


// Тяжелые свойства устанавливаются и берутся через геттеры и сеттеры
QString Record::getText() const
{
  // У легкого объекта невозможно запросить текст, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    critical_error("Cant get text from lite record object"+getIdAndNameAsString());

  return text;
}


void Record::setText(QString iText)
{
  // Легкому объекту невозможно установить текст, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    critical_error("Cant set text for lite record object"+getIdAndNameAsString());

  text=iText;
}


QMap<QString, QByteArray> Record::getPictureFiles() const
{
  // У легкого объекта невозможно запросить картики, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    critical_error("Cant get picture files from lite record object"+getIdAndNameAsString());

  return pictureFiles;
}


// todo: Переделать на копирование по ссылке
void Record::setPictureFiles(QMap<QString, QByteArray> iPictureFiles)
{
  // Легкому объекту невозможно установить картики, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    critical_error("Cant set picture files for lite record object"+getIdAndNameAsString());

  pictureFiles=iPictureFiles;
}


QMap<QString, QByteArray> Record::getAttachFiles() const
{
  // У легкого объекта невозможно запросить приаттаченные файлы, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    critical_error("Cant get attach files from lite record object"+getIdAndNameAsString());

  return pictureFiles;
}


// todo: Переделать на копирование по ссылке
void Record::setAttachFiles(QMap<QString, QByteArray> iAttachFiles)
{
  // Легкому объекту невозможно установить картики, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    critical_error("Cant set attach files for lite record object"+getIdAndNameAsString());

  attachFiles=iAttachFiles;
}


// Запись "тяжелых" атрибутов (текста, картинок, приаттаченных файлов) на диск
void Record::flushFatAttributes()
{
 // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
 // то зашифровать текст невозможно
 if(getField("crypt", pos)=="1" &&
    globalParameters.getCryptKey().length()==0)
  critical_error("RecordTableData::set_text() : Try save text for crypt record while password not setted.");

 // Заполняются имена директории и полей
 // Директория при проверке создается если ее небыло
 QString nameDirFull;
 QString nameFileFull;
 if(checkAndFillFileDir(pos, nameDirFull, nameFileFull)==false)
  critical_error("RecordTableData::set_text() as String : For record "+QString::number(pos)+" can not set field \"dir\" or \"file\"");

 // Если шифровать ненужно
 if(getField("crypt", pos)=="" || getField("crypt", pos)=="0")
  {
   // Текст сохраняется в файл
   QFile wfile(nameFileFull);

   if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
    critical_error("Cant open text file "+nameFileFull+" for write.");

   QTextStream out(&wfile);
   out.setCodec("UTF-8");
   out << text;
  }
 else if(getField("crypt", pos)=="1")
  {
   // Текст шифруется
   QByteArray encryptData=encryptStringToByteArray(globalParameters.getCryptKey(), text);

   // В файл сохраняются зашифрованные данные
   QFile wfile(nameFileFull);

   if(!wfile.open(QIODevice::WriteOnly))
    critical_error("Cant open binary file "+nameFileFull+" for write.");

   wfile.write(encryptData);
  }
 else
  critical_error("RecordTableData::set_text() : Unavailable crypt field value \""+getField("crypt", pos)+"\"");

 // Если есть какие-то файлы, сопровождащие запись,
 // они вставляются в конечную директорию
 if(iPictureFiles.size()>0)
  save_files_to_directory(nameDirFull, iPictureFiles);
}
