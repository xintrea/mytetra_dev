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


// Получение значения текста напрямую из файла, без заполнения свойства text
// Так как заполнение свойства не происходит, объект может бы легким. Проверки на легкость не требуется
QString Record::getTextDirect() const
{
  // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
  // то расшифровка невозможна
  if(fieldList.value("crypt")=="1" &&
     globalParameters.getCryptKey().length()==0)
    return QString();

  // Выясняется полное имя файла с текстом записи
  QString fileName=getFullTextFileName();

  checkAndCreateTextFile();

  QFile f(fileName);

  // Открывается файл
  if(!f.open(QIODevice::ReadOnly))
    critical_error("File "+fileName+" not readable. Check permission.");

  // Если незашифровано
  if(fieldList.value("crypt").length()==0 ||fieldList.value("crypt")=="0")
  {
    qDebug() << "Record::getTextDirect() : return direct data";
    return QString::fromUtf8( f.readAll() );
  }
  else
  {
    qDebug() << "Record::getTextDirect( : return direct data after decrypt";
    return decryptStringFromByteArray(globalParameters.getCryptKey(), f.readAll()); // Если зашифровано
  }

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
  // У легкого объекта невозможно запросить картинки, если так происходит - это ошибка вызывающей логики
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


// Приватная функция, шифрует только поля
void Record::switchToEncryptFields(void)
{
  if(fieldList.value("crypt")=="1")
    return;

  // Устанавливается поле (флаг) что запись зашифрована
  fieldList["crypt"]="1";

  // Для шифрации просто пересохраняются поля.
  // В момент, когда поле сохраняется, оно будет сохранено зашифрованным так как у записи установлен флаг шифрования

  // Выбираются поля, разрешенные для шифрования
  foreach(QString fieldName, fixedParameters.recordFieldCryptedList())
  {
    // Если в полях записей присутствует очередное разрешенное имя поля
    // И это поле непустое
    // Поле шифруется
    if(fieldList.contains(fieldName))
      if(fieldList[fieldName].length()>0)
      {
        // Устанавливаются значения, при установке произойдет шифрация
        setField(fieldName, fieldList.value(fieldName) );

        /*
        set_field(fieldName,
              encryptString(globalParameters.getCryptKey(), recordFields[fieldName]),
              i);
        */
      }
  }
}


// Шифрование записи с легкими данными
void Record::switchToEncryptAndSaveLite(void)
{
  // Метод обрабатывает только легкий объект
  if(liteFlag==false)
    critical_error("Cant call switchToEncryptLite() for non lite record object "+getIdAndNameAsString());

  // Нельзя шифровать уже зашифрованную запись
  if(fieldList.value("crypt")=="1")
    critical_error("Cant call switchToEncryptLite() for crypt record object "+getIdAndNameAsString());

  // Зашифровываются поля записи
  switchToEncryptFields();

  // В легком объекте данные не из полей находятся в файлах

  // Шифрация файла с текстом записи
  QString dirName;
  QString fileName;
  checkAndFillFileDir(dirName, fileName);
  encryptFile(globalParameters.getCryptKey(), fileName);

  // Шифрование приаттаченных файлов
  foreach(QString fileId, attachFiles.keys())
  {
    QString fileName=getFullFileName(fileId+".bin");
    encryptFile(globalParameters.getCryptKey(), fileName);
  }
}


// Шифрование записи с полновесными данными
void Record::switchToEncryptAndSaveFat(void)
{
  // Метод обрабатывает только тяжелый объект
  if(liteFlag==true)
    critical_error("Cant call switchToEncryptFat() for non fat record object "+getIdAndNameAsString());

  // Нельзя шифровать уже зашифрованную запись
  if(fieldList.value("crypt")=="1")
    critical_error("Cant call switchToEncryptAndSaveFat() for crypt record object "+getIdAndNameAsString());

  // Зашифровываются поля записи
  switchToEncryptFields();

  // Тяжелые данные записываются в хранилище, при записи будет произведена шифрация
  pushFatAttributes();
}


// Запись "тяжелых" атрибутов (текста, картинок, приаттаченных файлов) на диск
// Исходные данные должны быт нешифрованы. Они будут зашифровано по необходимости
void Record::pushFatAttributes()
{
  // Легкий объект невозможно сбросить на диск, потому что он не содержит данных, сбрасываемых в файлы
  if(liteFlag==true)
    critical_error("Cant push lite record object"+getIdAndNameAsString());

  // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
  // то зашифровать текст невозможно
  if(fieldList.value("crypt")=="1" &&
     globalParameters.getCryptKey().length()==0)
    critical_error("Record::pushFatAttributes() : Try save text for crypt record while password not setted.");

  // Заполняются имена директории и полей
  // Директория при проверке создается если ее небыло
  QString dirName;
  QString fileName;
  checkAndFillFileDir(dirName, fileName);

  // Запись файла с текстом записи, при необходимости текст в вызываемой функции будет зашифрован
  saveText(text);

  // Если есть файлы картинок, они вставляются в конечную директорию (картинки не шифруются)
  if(pictureFiles.size()>0)
    save_files_to_directory(dirName, pictureFiles);

  // Если есть приаттаченные файлы, они вставляются в конечную директорию
  // todo: сделать шифрование приаттаченных файлов, если запись зашифрована
  if(attachFiles.size()>0)
    save_files_to_directory(dirName, attachFiles);
}


void Record::saveText(QString iText)
{
  QString fileName=getFullTextFileName();

  // Если шифровать ненужно
  if(fieldList.value("crypt").length()==0 || fieldList.value("crypt")=="0")
  {
    // Текст сохраняется в файл
    QFile wfile(fileName);

    if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
      critical_error("Cant open text file "+fileName=+" for write.");

    QTextStream out(&wfile);
    out.setCodec("UTF-8");
    out << iText;
  }
  else if(fieldList.value("crypt")=="1")
  {
    // Текст шифруется
    QByteArray encryptData=encryptStringToByteArray(globalParameters.getCryptKey(), iText);

    // В файл сохраняются зашифрованные данные
    QFile wfile(fileName=);

    if(!wfile.open(QIODevice::WriteOnly))
      critical_error("Cant open binary file "+fileName+" for write.");

    wfile.write(encryptData);
  }
  else
    critical_error("Record::saveText() : Unavailable crypt field value \""+fieldList.value("crypt")+"\"");
}


// Полное имя директории записи
QString Record::getFullDirName() const
{
 if(fieldList.contains("dir")==false)
   critical_error("Record::getFullDirName() : Not present dir field");

 return mytetraConfig.get_tetradir()+"/base/"+fieldList.value("dir");
}


// Полное имя файла с текстом записи
QString Record::getFullTextFileName() const
{
  if(fieldList.contains("file")==false)
    critical_error("Record::getFullDirName() : Not present file field");

  return getFullDirName()+"/"+fieldList.value("file");
}


// Полное имя произвольного файла в каталоге записи
QString Record::getFullFileName(QString fileName) const
{
  return getFullDirName()+"/"+fileName;
}


// Функция проверяет наличие полей dir и file (они используются для текста записи)
// проверяет их правильность и заполняет в переданных параметрах полные имена директории и файла
void Record::checkAndFillFileDir(QString &iDirName, QString &iFileName)
{
 // Полные имена директории и файла
 iDirName=getFullDirName();
 iFileName=getFullTextFileName();

 QDir recordDir(iDirName);

 if(!recordDir.isReadable)
   critical_error("Record::checkAndFillFileDir() : Directory "+dirName+" is not readable");

 if(!recordDir.isWritable)
   critical_error("Record::checkAndFillFileDir() : Directory "+dirName+" is not writable");

 // Проверяется наличие директории, куда будет вставляться файл с текстом записи
 if(!recordDir.exists())
  {
   // Создается новая директория в директории base
   QDir directory(mytetraConfig.get_tetradir()+"/base");
   result=directory.mkdir(dirName);

   if(!result)
     critical_error("Record::checkAndFillFileDir() : Can't create directory "+dirName);
  }
}


// В функцию должно передаваться полное имя файла
void Record::checkAndCreateTextFile()
{
 QString fileName=getFullTextFileName();

 QFile f(fileName);

 // Если нужный файл не существует
 if(!f.exists())
  {
   // Выводится уведомление что будет создан пустой текст записи
   QMessageBox msgBox;
   msgBox.setWindowTitle(tr("Warning!"));
   msgBox.setText( tr("Database consistency was broken.\n File %1 not found.\n MyTetra will try to create a blank entry for the corrections.").arg(fileName) );
   msgBox.setIcon(QMessageBox::Information);
   msgBox.exec();

   // Создается пустой текст записи
   saveText("");
  }
}
