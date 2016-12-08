#include <QObject>
#include <QMessageBox>
#include <QDomElement>

#include "main.h"
#include "Record.h"

#include "models/appConfig/AppConfig.h"
#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"
#include "models/attachTable/AttachTableData.h"
#include "libraries/crypt/CryptService.h"
#include "libraries/DiskHelper.h"

extern AppConfig mytetraConfig;
extern FixedParameters fixedParameters;
extern GlobalParameters globalParameters;


Record::Record() : attachTableData(this)
{
  liteFlag=true; // По-умолчанию объект легкий
}


// Конструктор копирования
Record::Record(const Record &obj)
{
  // Скопировать нужно каждый кусочек класса, сами они не копируются
  liteFlag=obj.liteFlag;
  fieldList=obj.fieldList;
  text=obj.text;
  pictureFiles=obj.pictureFiles;
  attachTableData=obj.attachTableData;

  // Обратный указатель во включенном объекте должен указывать на новый экземпляр
  attachTableData.setRecord(this);
  attachTableData.updateAttachTableBackLink();
}


Record::~Record()
{

}


// На вход этой функции подается элемент <record>
void Record::setupDataFromDom(QDomElement iDomElement)
{
  // Получение списка всех атрибутов текущего элемента
  QDomNamedNodeMap attList;
  attList=iDomElement.attributes();

  // Перебор атрибутов в списке и добавление их в запись
  int i;
  for(i=0; i<attList.count(); i++)
  {
    QDomAttr attcurr=attList.item(i).toAttr();

    QString name=attcurr.name();
    QString value=attcurr.value();

    this->setNaturalFieldSource(name, value);

    // Распечатка считанных данных в консоль
    // qDebug() << "Read record attr " << name << value;
  }

  // Инициализируется таблица прикрепляемых файлов
  attachTableData.clear(); // Подумать, возможно эта команда не нужна
  attachTableData.setRecord(this);

  // Проверка, есть ли у переданного DOM-элемента таблица файлов для заполнения
  if(!iDomElement.firstChildElement("files").isNull())
    attachTableData.setupDataFromDom( iDomElement.firstChildElement("files") ); // Заполнение таблицы приаттаченных файлов
}


QDomElement Record::exportDataToDom(QDomDocument *doc) const
{
  QDomElement elem=doc->createElement("record");

  // Перебираются допустимые имена полей, доступных для сохранения
  QStringList availableFieldList=fixedParameters.recordNaturalFieldAvailableList;
  int availableFieldListSize=availableFieldList.size();
  for(int j=0; j<availableFieldListSize; ++j)
  {
    QString currentFieldName=availableFieldList.at(j);

    // Устанавливается значение поля как атрибут DOM-узла
    if(isNaturalFieldExists(currentFieldName))
      elem.setAttribute(currentFieldName, getNaturalFieldSource(currentFieldName));
  }

  // К элементу записи прикрепляется элемент таблицы приаттаченных файлов, если таковые есть
  if(attachTableData.size()>0)
    elem.appendChild( attachTableData.exportDataToDom(doc) );

  return elem;
}


void Record::exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const
{
  xmlWriter->writeStartElement("record");

  // Перебираются допустимые имена полей, доступных для сохранения
  QStringList availableFieldList=fixedParameters.recordNaturalFieldAvailableList;
  int availableFieldListSize=availableFieldList.size();
  for(int j=0; j<availableFieldListSize; ++j)
  {
    QString currentFieldName=availableFieldList.at(j);

    // Устанавливается значение поля как атрибут записи
    if(isNaturalFieldExists(currentFieldName))
      xmlWriter->writeAttribute(currentFieldName, getNaturalFieldSource(currentFieldName));
  }

  // К элементу записи прикрепляется элемент таблицы приаттаченных файлов, если таковые есть
  if(attachTableData.size()>0)
    attachTableData.exportDataToStreamWriter( xmlWriter );

  xmlWriter->writeEndElement(); // Закрылся record
}


bool Record::isEmpty() const
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
  // Переключение возможно только из полновесного состояния
  if(liteFlag==true)
    criticalError("Record::switchToLite() : Record "+getIdAndNameAsString()+" already lite");

  text.clear();
  pictureFiles.clear();

  attachTableData.switchToLite();

  liteFlag=true;
}


// Метод используется только при следующих действиях:
// - добавление новой записи
// - помещение записи в буфер обмена
void Record::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true || text.length()>0 || pictureFiles.count()>0)
    criticalError("Unavailable switching record object to fat state. "+getIdAndNameAsString());

  attachTableData.switchToFat();

  liteFlag=false;
}


// Метод используется только при выводе сообщений об ошибке
QString Record::getIdAndNameAsString() const
{
  QString id, name;

  if(fieldList.contains("id"))
    id=fieldList["id"];

  if(fieldList.contains("name"))
    name=fieldList["name"];

  return "Record ID: "+id+" Name: "+name;
}


// Получение значения поля
// Метод возвращает расшифрованные данные, если запись была зашифрована
QString Record::getField(QString name) const
{
  // Если имя поля недопустимо
  if(fixedParameters.isRecordFieldAvailable(name)==false)
    criticalError("RecordTableData::getField() : get unavailable field "+name);

  // Для настоящего поля
  if(fixedParameters.isRecordFieldNatural(name))
    return getNaturalField(name);

  // Для вычислимого поля
  if(fixedParameters.isRecordFieldCalculable(name))
    return getCalculableField(name);

  return "";
}


// Защищенная функция
// Проверка допустимости имени происходит в вызывающем коде
QString Record::getNaturalField(QString name) const
{
  QString result="";

  // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
  // то расшифровка невозможна
  if(fixedParameters.recordFieldCryptedList.contains(name))
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
    if(fixedParameters.recordFieldCryptedList.contains(name))
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
      result=CryptService::decryptString(globalParameters.getCryptKey(), fieldList[name]);
    }
  }

  // qDebug() << "RecordTableData::get_field : pos" << pos <<"name"<<name<<"value"<<result;

  return result;
}


// Защищенная функция
// Проверка допустимости имени происходит в вызывающем коде (необходимо для оптимизации)
QString Record::getCalculableField(QString name) const
{
  // Наличие аттачей
  if(name=="hasAttach")
  {
    if(this->attachTableData.size()>0)
      return "1";
    else
      return "0";
  }

  // Количество аттачей
  if(name=="attachCount")
    return QString::number( this->attachTableData.size() );

  return "";
}


void Record::setField(QString name, QString value)
{
  // Если имя поля недопустимо (установить значение можно только для натурального поля)
  if(fixedParameters.isRecordFieldNatural(name)==false)
    criticalError("In RecordTableData::setField() unavailable field name "+name+" try set to "+value);

  bool isCrypt=false;

  // Если имя поля принадлежит списку полей, которые могут шифроваться
  // и в наборе полей есть поле crypt
  // и поле crypt установлено в 1
  // и поле не пустое (пустые данные ненужно шифровать)
  if(fixedParameters.recordFieldCryptedList.contains(name))
    if(fieldList.contains("crypt"))
      if(fieldList["crypt"]=="1")
        if(value.length()>0)
        {
          if(globalParameters.getCryptKey().length()>0)
            isCrypt=true;
          else
            criticalError("In RecordTableData::setField() can not set data to crypt field "+name+". Password not setted");
        }


  // Если нужно шифровать, поле шифруется
  if(isCrypt==true)
    value=CryptService::encryptString(globalParameters.getCryptKey(), value);

  // Устанавливается значение поля
  fieldList.insert(name, value);

  // qDebug() << "RecordTableData::set_field : pos" << pos <<"name"<<name<<"value"<<value;
}


bool Record::isNaturalFieldExists(QString name) const
{
  return fieldList.contains(name);
}


QString Record::getNaturalFieldSource(QString name) const
{
  // Если имя поля недопустимо
  if(fixedParameters.isRecordFieldNatural(name)==false)
    criticalError("RecordTableData::getNaturalFieldSource() : get unavailable field "+name);

  // Если поле с таким названием есть
  if(fieldList.contains(name))
    return fieldList[name]; // Возвращается значение поля
  else
    return QString();
}


void Record::setNaturalFieldSource(QString name, QString value)
{
  // Если имя поля недопустимо
  if(fixedParameters.isRecordFieldNatural(name)==false)
    criticalError("In RecordTableData::setNaturalFieldSource() unavailable field name "+name+" try set to "+value);

  // Устанавливается значение поля
  fieldList.insert(name, value);
}


// Получение значений всех натуральных (невычислимых) полей
// Поля, которые могут быть у записи, но не заданы, не передаются
// Поля, которые зашифрованы, расшифровываются
QMap<QString, QString> Record::getNaturalFieldList() const
{
  // Список имен инфополей
  QStringList fieldNames=fixedParameters.recordNaturalFieldAvailableList;

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
        if(fixedParameters.recordFieldCryptedList.contains(currName)==false)
          result=fieldList[currName]; // Напрямую значение поля
        else
          if(globalParameters.getCryptKey().length()>0 &&
             fixedParameters.recordFieldCryptedList.contains(currName))
            result=CryptService::decryptString(globalParameters.getCryptKey(), fieldList[currName]); // Расшифровывается значение поля
      }

      resultFieldList[currName]=result;
    }
  }

  qDebug() << "Record::getFieldList() : " << resultFieldList;

  return resultFieldList;
}


AttachTableData Record::getAttachTable() const
{
  if(this->isLite()!=attachTableData.isLite())
    criticalError("getAttachTable(): Unsyncro lite state for record: "+getIdAndNameAsString());

  return attachTableData;
}


AttachTableData *Record::getAttachTablePointer()
{
  if(this->isLite()!=attachTableData.isLite())
    criticalError("getAttachTable(): Unsyncro lite state for record: "+getIdAndNameAsString());

  return &attachTableData;
}


void Record::setAttachTable(AttachTableData iAttachTable)
{
  if(this->isLite()!=attachTableData.isLite())
    criticalError("setAttachTable(): Unsyncro lite state for record: "+getIdAndNameAsString());

  attachTableData=iAttachTable;
}


// Получение текста записи из памяти
// Если запись зашифрована, возвращаемый текст будет расшифрован
QString Record::getText() const
{
  // У легкого объекта невозможно запросить текст из памяти, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    criticalError("Cant get text from lite record object"+getIdAndNameAsString());

  // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
  // то расшифровка невозможна
  if(fieldList.value("crypt")=="1" &&
     globalParameters.getCryptKey().length()==0)
    return "";

  // Если шифровать ненужно
  if(fieldList.value("crypt").length()==0 || fieldList.value("crypt")=="0")
    return QString(text); // Текст просто преобразуется из QByteArray
  else if(fieldList.value("crypt")=="1") // Если нужно шифровать
    return CryptService::decryptStringFromByteArray(globalParameters.getCryptKey(), text);
  else
    criticalError("Record::getText() : Unavailable crypt field value \""+fieldList.value("crypt")+"\"");

  return "";
}


// Получение значения текста напрямую из файла, без заполнения свойства text
QString Record::getTextDirect()
{
  // У тяжелого объекта невозможно получить текст записи из файла (у тяжелого объекта текст записи хранится в памяти)
  if(liteFlag==false)
    criticalError("Cant run Record::getTextDirect() for non lite record "+getIdAndNameAsString());

  // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
  // то расшифровка невозможна
  if(fieldList.value("crypt")=="1" &&
     globalParameters.getCryptKey().length()==0)
    return "";

  // Выясняется полное имя файла с текстом записи
  QString fileName=getFullTextFileName();

  checkAndCreateTextFile();

  QFile f(fileName);

  // Открывается файл
  if(!f.open(QIODevice::ReadOnly))
    criticalError("File "+fileName+" not readable. Check permission.");

  // Если незашифровано
  if(fieldList.value("crypt").length()==0 ||fieldList.value("crypt")=="0")
  {
    qDebug() << "Record::getTextDirect() : return direct data";
    return QString::fromUtf8( f.readAll() );
  }
  else
  {
    qDebug() << "Record::getTextDirect() : return direct data after decrypt";
    return CryptService::decryptStringFromByteArray(globalParameters.getCryptKey(), f.readAll()); // Если зашифровано
  }

  return "";
}


// Установка текста записи как свойства объекта
// Принимает незашифрованные данные, сохраняет их в памяти, при записи шифрует если запись зашифрована
void Record::setText(QString iText)
{
  // Легкому объекту невозможно установить текст, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    criticalError("Cant set text for lite record object"+getIdAndNameAsString());

  // Если шифровать ненужно
  if(fieldList.value("crypt").length()==0 || fieldList.value("crypt")=="0")
    text=iText.toUtf8(); // Текст просто запоминается в виде QByteArray
  else if(fieldList.value("crypt")=="1") // Если нужно шифровать
    text=CryptService::encryptStringToByteArray(globalParameters.getCryptKey(), iText);
  else
    criticalError("Record::setText() : Unavailable crypt field value \""+fieldList.value("crypt")+"\"");
}


// Сохранение текста записи на диск без установки текста записи как свойства
// Принимает незашифрованные данные, сохраняет в файл, при записи шифрует если запись зашифрована
void Record::saveTextDirect(QString iText)
{
  QString fileName=getFullTextFileName();

  // Если шифровать ненужно
  if(fieldList.value("crypt").length()==0 || fieldList.value("crypt")=="0")
  {
    // Текст сохраняется в файл
    QFile wfile(fileName);

    if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
      criticalError("Cant open text file "+fileName+" for write.");

    QTextStream out(&wfile);
    out.setCodec("UTF-8");
    out << iText;
  }
  else if(fieldList.value("crypt")=="1")
  {
    // Текст шифруется
    QByteArray encryptData=CryptService::encryptStringToByteArray(globalParameters.getCryptKey(), iText);

    // В файл сохраняются зашифрованные данные
    QFile wfile(fileName);

    if(!wfile.open(QIODevice::WriteOnly))
      criticalError("Cant open binary file "+fileName+" for write.");

    wfile.write(encryptData);
  }
  else
    criticalError("Record::saveTextDirect() : Unavailable crypt field value \""+fieldList.value("crypt")+"\"");
}


// Запись текста записи, хранимого в памяти, на диск
void Record::saveText()
{
  QString fileName=getFullTextFileName();

  // В файл сохраняются зашифрованные данные
  QFile wfile(fileName);

  if(!wfile.open(QIODevice::WriteOnly))
    criticalError("Record::saveText() : Cant open binary file "+fileName+" for write.");

  // Сохраняется QByteArray с текстом записи (в QByteArray могут быть как зашифрованные, так и не зашифрованные данные)
  wfile.write(text);
}


QMap<QString, QByteArray> Record::getPictureFiles() const
{
  // У легкого объекта невозможно запросить картинки, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    criticalError("Cant get picture files from lite record object"+getIdAndNameAsString());

  return pictureFiles;
}


// todo: Переделать на копирование по ссылке
void Record::setPictureFiles(QMap<QString, QByteArray> iPictureFiles)
{
  // Легкому объекту невозможно установить картики, если так происходит - это ошибка вызывающей логики
  if(liteFlag==true)
    criticalError("Cant set picture files for lite record object"+getIdAndNameAsString());

  pictureFiles=iPictureFiles;
}


// Приватная функция, шифрует только поля
void Record::switchToEncryptFields(void)
{
  if(fieldList.value("crypt")=="1")
    return;

  // Устанавливается поле (флаг) что запись зашифрована
  fieldList["crypt"]="1";

  // Для шифрации просто переустанавливаются поля.
  // В момент, когда поле переустанавливается, оно получит зашифрованное значение так как у записи установлен флаг шифрования

  // Выбираются поля, разрешенные для шифрования
  foreach(QString fieldName, fixedParameters.recordFieldCryptedList)
  {
    // Если в полях записей присутствует очередное разрешенное имя поля
    // И это поле непустое
    // Поле шифруется
    if(fieldList.contains(fieldName))
      if(fieldList[fieldName].length()>0)
        setField(fieldName, fieldList.value(fieldName) ); // Устанавливаются значения, при установке произойдет шифрация
  }
}


// Приватная функция, расшифровывает только поля
void Record::switchToDecryptFields(void)
{
  // Нельзя расшифровать незашифрованную запись
  if(fieldList.value("crypt")!="1")
    return;

  // Выбираются поля, разрешенные для шифрования
  foreach(QString fieldName, fixedParameters.recordFieldCryptedList)
  {
    // Если в полях записей присутствует очередное разрешенное имя поля
    // И это поле непустое
    // Поле расшифровывается
    if(fieldList.contains(fieldName))
      if(fieldList[fieldName].length()>0)
        setNaturalFieldSource(fieldName, getField(fieldName) );
  }

  // Устанавливается поле (флаг) что запись не зашифрована
  fieldList["crypt"]="0";
}


// Шифрование записи с легкими данными
void Record::switchToEncryptAndSaveLite(void)
{
  // Метод обрабатывает только легкий объект
  if(liteFlag==false)
    criticalError("Cant call switchToEncryptAndSaveLite() for non lite record object "+getIdAndNameAsString());

  // Нельзя шифровать уже зашифрованную запись
  if(fieldList.value("crypt")=="1")
    criticalError("Cant call switchToEncryptAndSaveLite() for crypt record object "+getIdAndNameAsString());

  // В легком объекте данные не из полей находятся в файлах

  // Шифрация файла с текстом записи на диске
  QString dirName;
  QString fileName;
  checkAndFillFileDir(dirName, fileName);
  CryptService::encryptFile(globalParameters.getCryptKey(), fileName);

  // Шифрование приаттаченных файлов на диске
  attachTableData.encrypt(Attach::areaFile);

  // Зашифровываются поля записи (здесь же устанавливается флаг crypt)
  switchToEncryptFields();
}


// Шифрование записи с полновесными данными
void Record::switchToEncryptAndSaveFat(void)
{
  // Метод обрабатывает только тяжелый объект
  if(liteFlag==true)
    criticalError("Cant call switchToEncryptFat() for non fat record object "+getIdAndNameAsString());

  // Нельзя шифровать уже зашифрованную запись
  if(fieldList.value("crypt")=="1")
    criticalError("Cant call switchToEncryptAndSaveFat() for crypt record object "+getIdAndNameAsString());

  // Зашифровывается текст записи в памяти
  text=CryptService::encryptByteArray(globalParameters.getCryptKey(), text);

  // Зашифровываются аттачи в памяти
  attachTableData.encrypt(Attach::areaMemory);

  // Зашифровываются поля записи (здесь же устанавливается флаг crypt)
  switchToEncryptFields();

  // Тяжелые данные записываются в хранилище
  pushFatAttributes();
}


// Расшифровка записи с легкими данными
void Record::switchToDecryptAndSaveLite(void)
{
  // Метод обрабатывает только легкий объект
  if(liteFlag==false)
    criticalError("Cant call switchToDecryptAndSaveLite() for non lite record object "+getIdAndNameAsString());

  // Нельзя расшифровать не зашифрованную запись
  if(fieldList.value("crypt")!="1")
    criticalError("Cant call switchToDecryptAndSaveLite() for non crypt record object "+getIdAndNameAsString());

  // Расшифровка файла с текстом записи на диске
  QString dirName;
  QString fileName;
  checkAndFillFileDir(dirName, fileName);
  CryptService::decryptFile(globalParameters.getCryptKey(), fileName);

  // Расшифровка приаттаченных файлов на диске
  attachTableData.decrypt(Attach::areaFile);

  // Расшифровка полей (здесь же происходит установка флага crypt в конечное значение)
  switchToDecryptFields(); // Ранее было fieldList=getNaturalFieldList(); (имеено так, так как getNaturalFieldList() возвращает расшифрованные данные)

  // Устанавливается флаг что шифрации нет
  // fieldList["crypt"]="0"; // Похоже, что команда не нужна, так как в switchToDecryptFields() флаг уже установлен
}


void Record::switchToDecryptAndSaveFat(void)
{
  // Метод обрабатывает только тяжелый объект
  if(liteFlag==true)
    criticalError("Cant call switchToDecryptAndSaveFat() for non fat record object "+getIdAndNameAsString());

  // Нельзя расшифровать не зашифрованную запись
  if(fieldList.value("crypt")!="1")
    criticalError("Cant call switchToDecryptAndSaveFat() for non crypt record object "+getIdAndNameAsString());

  // Расшифровывается текст записи в памяти
  text=CryptService::decryptByteArray(globalParameters.getCryptKey(), text);

  // Расшифровываются аттачи в памяти
  attachTableData.decrypt(Attach::areaMemory);

  // Расшифровываются поля записи (здесь же происходит установка флага crypt в конечное значение)
  switchToDecryptFields();

  // Тяжелые данные записываются в хранилище
  pushFatAttributes();
}


// Запись "тяжелых" атрибутов (текста, картинок и приаттаченных файлов) на диск
// Запись происходит в явном виде, то, что хранится в Fat-атрибутах, без шифрации-дешифрации
void Record::pushFatAttributes()
{
  // Легкий объект невозможно сбросить на диск, потому что он не содержит данных, сбрасываемых в файлы
  if(liteFlag==true)
    criticalError("Cant push lite record object"+getIdAndNameAsString());

  // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
  // то зашифровать текст невозможно
  if(fieldList.value("crypt")=="1" &&
     globalParameters.getCryptKey().length()==0)
    criticalError("Record::pushFatAttributes() : Try save text for crypt record while password not setted.");

  // Заполняются имена директории и полей
  // Директория при проверке создается если ее небыло
  QString dirName;
  QString fileName;
  checkAndFillFileDir(dirName, fileName);

  // Запись файла с текстом записи
  saveText();

  // Если есть файлы картинок, они вставляются в конечную директорию
  if(pictureFiles.size()>0)
    DiskHelper::saveFilesToDirectory(dirName, pictureFiles);

  // Если есть приаттаченные файлы, они вставляются в конечную директорию
  if(attachTableData.size()>0)
    attachTableData.saveAttachFilesToDirectory(dirName);
}


// Статический метод, заменяет в переданном файле записи внутренние ссылки вида "mytetra:" согласно таблице перекодировки
// Используется при импорте, когда идентификаторы записей могут поменяться, следовательно должны поменяться и ссылки
void Record::replaceInternalReferenceByTranslateTable(QString recordFileName, QMap<QString, QString> idRecordTranslate)
{
  //Считывание из файла
  QString lines;
  QFile file(recordFileName);

  if (!file.open(QIODevice::ReadOnly))
    return;

  QTextStream stream(&file);
  while(!stream.atEnd())
    lines.append(stream.readLine());

  file.close();


  // Подготавливается регулярное выражение
  QRegExp rx("href=\"mytetra:\\/\\/note/(.*)\"");
  rx.setMinimal(true); // Регулярка делается ленивой (ленивые квантификаторы в строке регвыра в Qt не поддерживаются)

  // Заполняется список идентификаторов, встречающихся во внутренних ссылках
  QStringList internalIdList;
  int pos = 0;
  while ((pos = rx.indexIn(lines, pos))!=-1)
  {
    internalIdList << rx.cap(1);
    pos += rx.matchedLength();
  }


  // Перебираются найденные внутри mytetra-ссылок идентификаторы
  bool isIdReplace=false;
  foreach(QString currentId, internalIdList)
    if(idRecordTranslate.contains(currentId)) // Если идентификатор надо заменить
    {
      lines.replace("href=\"mytetra://note/"+currentId+"\"",
                    "href=\"mytetra://note/"+idRecordTranslate[currentId]+"\"");
      isIdReplace=true;
    }


  // Запись в файл, если были заменены идентификаторы во внутренних ссылках
  if(isIdReplace)
  {
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QTextStream writeStream(&file);
      writeStream << lines;
    }

    file.close();
  }
}


// Полное имя директории записи
QString Record::getFullDirName() const
{
 if(fieldList.contains("dir")==false)
   criticalError("Record::getFullDirName() : Not present dir field");

 return mytetraConfig.get_tetradir()+"/base/"+fieldList.value("dir");
}


// Короткое имя директории записи
QString Record::getShortDirName() const
{
 if(fieldList.contains("dir")==false)
   criticalError("Record::getShortDirName() : Not present dir field");

 return fieldList.value("dir");
}


// Полное имя файла с текстом записи
QString Record::getFullTextFileName() const
{
  if(fieldList.contains("file")==false)
    criticalError("Record::getFullDirName() : Not present file field");

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

 // ПРоверки на чтение и запись в этом месте бессмысленны, так как директории просто может не существовать
 // if(!recordDir.isReadable())
 //   critical_error("Record::checkAndFillFileDir() : Directory "+iDirName+" is not readable");

 // Оказывается, что у QDir нет возможности проверить доступность на запись в директорию
 // if(!recordDir.isWritable())
 //  critical_error("Record::checkAndFillFileDir() : Directory "+iDirName+" is not writable");

 // Проверяется наличие директории, куда будет вставляться файл с текстом записи
 if(!recordDir.exists())
  {
   // Создается новая директория в директории base
   QDir directory(mytetraConfig.get_tetradir()+"/base");
   bool result=directory.mkdir( getShortDirName() );

   if(!result)
     criticalError("Record::checkAndFillFileDir() : Can't create directory '"+getShortDirName()+"'");
  }
}


// В функцию должно передаваться полное имя файла
void Record::checkAndCreateTextFile()
{
  QString fileName=getFullTextFileName();

  QFile f(fileName);
  QFileInfo fileInfo(f);

  // Если директория с файлом не существует
  if(!fileInfo.absoluteDir().exists())
  {
    QString messageText=QObject::tr("The database is not consistent.\nThe directory %1 is not found.\nMyTetra will try to create a blank entry to fix it.").arg(fileInfo.absoluteDir().absolutePath());

    qWarning() << messageText;

    // Выводится уведомление что будет создана пустая директория записи
    QMessageBox msgBox;
    msgBox.setWindowTitle(QObject::tr("Warning!"));
    msgBox.setText( messageText );
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();

    // Создается пустая директория записи
    QDir tempDir("/");
    tempDir.mkpath( fileInfo.absoluteDir().absolutePath() );
  }

  // Если файл записи не существует
  if(!f.exists())
  {
    QString messageText=QObject::tr("The database is not consistent.\nThe file %1 is not found.\nMyTetra will try to create a blank entry to fix it.").arg(fileName);

    qWarning() << messageText;

    // Выводится уведомление что будет создан пустой текст записи
    QMessageBox msgBox;
    msgBox.setWindowTitle(QObject::tr("Warning!"));
    msgBox.setText( messageText );
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();

    // Создается пустой текст записи
    saveTextDirect( QString() );
  }
}
