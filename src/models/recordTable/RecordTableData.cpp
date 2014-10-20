#include <QDomElement>
#include <QMap>
#include <QString>
#include <QDir>
#include <QMessageBox>

#include "main.h"
#include "RecordTableData.h"

#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/GlobalParameters.h"
#include "models/tree/TreeItem.h"
#include "libraries/WalkHistory.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"

#include "libraries/wyedit/Editor.h"

extern AppConfig mytetraconfig;
extern GlobalParameters globalParameters;
extern WalkHistory walkHistory;


// Это набор данных данные конечной таблицы, с которыми удобно работать

// Конструктор
RecordTableData::RecordTableData(QObject *pobj)
{
 Q_UNUSED(pobj);

 treeItem=NULL;

 workPos=-1;

 return;
}


// Деструктор
RecordTableData::~RecordTableData()
{
 empty();
 return;
}


// Получение значения указанного поля для указанного имени поля
// Имя поля - все возможные имена полей, кроме text
QString RecordTableData::getField(QString name, int pos) const
{
 // Если индекс недопустимый
 if(pos<0 || pos>=fieldsTable.size())
  {
   QString i;
   i.setNum(pos);
   critical_error("RecordTableData::get_field() : get unavailable record index "+i);
  }

 // Если имя поля недопустимо
 if(isFieldNameAvailable(name)==false)
  critical_error("RecordTableData::get_field() : get unavailable field "+name);
 
 QMap<QString, QString> lineTmp;
 lineTmp=fieldsTable.at(pos);

 QString result="";


 // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
 // то расшифровка невозможна
 if(fieldNameForCryptList().contains(name))
  if(lineTmp.contains("crypt"))
   if(lineTmp["crypt"]=="1")
    if(globalParameters.getCryptKey().length()==0)
  return QString();


 // Если поле с таким названием есть
 if(lineTmp.contains(name))
  {
   // Нужно определить, зашифровано поле или нет

   bool isCrypt=false;

   // Если имя поля принадлежит списку полей, которые могут шифроваться
   // и в наборе полей есть поле crypt
   // и поле crypt установлено в 1
   // и запрашиваемое поле не пустое (пустые данные невозможно расшифровать)
   if(fieldNameForCryptList().contains(name))
    if(lineTmp.contains("crypt"))
     if(lineTmp["crypt"]=="1")
      if(lineTmp[name].length()>0)
       isCrypt=true;

   // Если поле не подлежит шифрованию
   if(isCrypt==false)
    result=lineTmp[name]; // Возвращается значение поля
   else
    {
     // Поле расшифровывается
     result=decryptString(globalParameters.getCryptKey(), lineTmp[name]);
    }
  }

 // qDebug() << "RecordTableData::get_field : pos" << pos <<"name"<<name<<"value"<<result;

 return result;
}


// Установка значения указанного поля для указанного элемента
void RecordTableData::setField(QString name, QString value, int pos)
{
 // Если индекс недопустимый
 if(pos<0 || pos>=fieldsTable.size())
  {
   QString i;
   i.setNum(pos);
   critical_error("In RecordTableData::set_field() unavailable record index "+i+" in table while field "+name+" try set to "+value);
  }


 // Если имя поля недопустимо
 if(isFieldNameAvailable(name)==false)
  critical_error("In RecordTableData::set_field() unavailable field name "+name+" try set to "+value);


 bool isCrypt=false;

 // Если имя поля принадлежит списку полей, которые могут шифроваться
 // и в наборе полей есть поле crypt
 // и поле crypt установлено в 1
 // и поле не пустое (пустые данные ненужно шифровать)
 if(fieldNameForCryptList().contains(name))
  if((fieldsTable[pos]).contains("crypt"))
   if((fieldsTable[pos])["crypt"]=="1")
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
 (fieldsTable[pos]).insert(name, value);

 // qDebug() << "RecordTableData::set_field : pos" << pos <<"name"<<name<<"value"<<value;
}


// Получение значения текста указанной записи
// Если возникнет проблема, что файла с текстом записи нет, будет создан пустой файл
QString RecordTableData::getText(int pos)
{
 // Если индекс недопустимый, возвращается пустая строка
 if(pos<0 || pos>=size()) 
  return QString();

 // Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
 // то расшифровка невозможна
 if(getField("crypt", pos)=="1" &&
    globalParameters.getCryptKey().length()==0)
  return QString();


 // Выясняются значения инфополей записи
 QMap<QString, QString> lineTmp;
 lineTmp=getFields(pos); // Раньше было index.row()

 // Выясняется полное имя файла с текстом записи
 QString fileName;
 fileName=mytetraconfig.get_tetradir()+"/base/"+lineTmp["dir"]+"/"+lineTmp["file"];

 QFile f(fileName);

 checkAndCreateTextFile(pos, fileName);

 // Открывается файл
 if(!f.open(QIODevice::ReadOnly))
  critical_error("File "+fileName+" not readable. Check permission.");

 // Если незашифровано
 if(getField("crypt", pos)=="" || getField("crypt", pos)=="0")
  {
   qDebug() << "RecordTableData::get_text() : return data direct";
   return QString::fromUtf8( f.readAll() );
  }
 else
  {
   qDebug() << "RecordTableData::get_text() : return data after decrypt";
   return decryptStringFromByteArray(globalParameters.getCryptKey(), f.readAll()); // Если зашифровано
  }
}


// В функцию должно передаваться полное имя файла
void RecordTableData::checkAndCreateTextFile(int pos, QString fillFileName)
{
 QFile f(fillFileName);

 // Если нужный файл не существует
 if(!f.exists())
  {
   // Выводится уведомление что будет создан пустой текст записи
   QMessageBox msgBox;
   msgBox.setWindowTitle(tr("Warning!"));
   msgBox.setText( tr("Database consistency was broken.\n File %1 not found.\n MyTetra will try to create a blank entry for the corrections.").arg(fillFileName) );
   msgBox.setIcon(QMessageBox::Information);
   msgBox.exec();

   // Создается пустой текст записи
   QString text="";
   QMap<QString, QByteArray> files;
   setRecordData(pos, text, files);
  }
}


/*
// Получение значения текста указанной записи
QByteArray RecordTableData::get_text_as_byte_array(int pos) const
{
 // Если индекс недопустимый, возвращается пустая строка
 if(pos<0 || pos>=fieldsTable.size()) return QByteArray();

 return get_text_smart(pos);
}


QByteArray RecordTableData::get_text_smart(int pos) const
{
 // Выясняются значения инфополей записи
 QMap<QString, QString> lineTmp;
 lineTmp=fieldsTable.at(pos); // Раньше было index.row()

 // Выясняется путь к файлу с текстом записи
 QString fileName;
 fileName=mytetraconfig.get_tetradir()+"/base/"+lineTmp["dir"]+"/"+lineTmp["file"];

 QFile f(fileName);

  // Если нужный файл не существует
 if(!f.exists())
  critical_error("File "+fileName+" not found");

 // Открывается файл
 if(!f.open(QIODevice::ReadOnly))
  critical_error("File "+fileName+" not readable. Check permission.");

 return f.readAll();
}
*/


// Установка текста указанной записи из QString
// pos - номер записи
// text - текст записи
// files - список файлов (по умолчанию пуст)
void RecordTableData::setRecordData(int pos,
                               const QString &text,
                               const QMap<QString, QByteArray> &files)
{
 // Если индекс недопустимый
 if(pos<0 || pos>=size())
  return;

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
 if(files.size()>0)
  save_files_to_directory(nameDirFull, files);
}



// Функция, которая заменяет стандартную функцию редактора по считыванию
// редактируемого текста
// Ее вызывает редактор, передавая указатель на себя
// и ссылку на переменную loadText, которую надо заполнить
void RecordTableData::editorLoadCallback(QObject *editor,
                                         QString &loadText)
{
 // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

 // Ссылка на объект редактора
 Editor *currEditor=qobject_cast<Editor *>(editor);

 // Нужно ли дешифровать данные при чтении
 bool workWithCrypt=false;
 if(currEditor->getMiscField("crypt")=="1")
  {
   // Если не установлено ключа шифрации
   if(globalParameters.getCryptKey().length()==0)
    {
     loadText="";
     return;
    }

   workWithCrypt=true;
  }

 QString fileName=currEditor->get_work_directory()+"/"+currEditor->get_file_name();

 QFile f(fileName);

  // Если нужный файл не существует
 if(!f.exists())
  critical_error("File "+fileName+" not found");

 // Открывается файл
 if(!f.open(QIODevice::ReadOnly))
  critical_error("File "+fileName+" not readable. Check permission.");

 // Если незашифровано
 if(workWithCrypt==false)
  loadText=QString::fromUtf8( f.readAll() );
 else
  loadText=decryptStringFromByteArray(globalParameters.getCryptKey(), f.readAll()); // Если зашифровано
}


// Функция, которая заменяет стандартную функцию редактора по записыванию
// редактируемого текста
// Ее вызывает редактор, передавая указатель на себя
// и текст который надо записать в переменной saveText
void RecordTableData::editorSaveCallback(QObject *editor,
                                           QString saveText)
{
 // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

 // Ссылка на объект редактора
 Editor *currEditor=qobject_cast<Editor *>(editor);

 // Нужно ли шифровать записываемый текст
 bool workWithCrypt=false;
 if(currEditor->getMiscField("crypt")=="1")
  {
   // Если не установлено ключа шифрации
   if(globalParameters.getCryptKey().length()==0)
    return;

   workWithCrypt=true;
  }

 QString fileName=currEditor->get_work_directory()+"/"+currEditor->get_file_name();

 // Если шифровать ненужно
 if(workWithCrypt==false)
  {
   // Текст сохраняется в файл
   QFile wfile(fileName);

   if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
    critical_error("RecordTableData::editor_save_callback() : Cant open text file "+fileName+" for write.");

   QTextStream out(&wfile);
   out.setCodec("UTF-8");
   out << saveText;
  }
 else
  {
   // Текст шифруется
   QByteArray encryptData=encryptStringToByteArray(globalParameters.getCryptKey(), saveText);

   // В файл сохраняются зашифрованные данные
   QFile wfile(fileName);

   if(!wfile.open(QIODevice::WriteOnly))
    critical_error("RecordTableData::editor_save_callback() : Cant open binary file "+fileName+" for write.");

   wfile.write(encryptData);
  }


 // Вызывается сохранение картинок
 // В данной реализации картинки сохраняются незашифрованными
 currEditor->save_textarea_images(Editor::SAVE_IMAGES_REMOVE_UNUSED);
}


/*
// Установка текста указанной записи из QString
// pos - номер записи
// text - текст записи
// files - список файлов (по умолчанию пуст)
void RecordTableData::set_text(int pos,
                               const QString &text,
                               const QMap<QString, QByteArray> &files)
{
 QByteArray unusedByteArray;

 set_text_smart(pos,
                0,
                text,
                unusedByteArray,
                files);
}


// Установка текста указанной записи из QByteArray
// pos - номер записи
// text - текст записи
// files - список файлов (по умолчанию пуст)
void RecordTableData::set_text(int pos,
                               const QByteArray &text,
                               const QMap<QString, QByteArray> &files)
{
 QString unusedString;

 set_text_smart(pos,
                1,
                unusedString,
                text,
                files);
}


// Установка текста указанной записи из QString или QByteArray
// pos - номер записи
// mode - 0-брать данные из QString, 1-из QByteArray
// text - текст записи
// files - список файлов (по умолчанию пуст)
void RecordTableData::set_text_smart(int pos,
                                     int mode,
                                     const QString &textString,
                                     const QByteArray &textByteArray,
                                     const QMap<QString, QByteArray> &files)
{
 // Если индекс недопустимый
 if(pos<0 || pos>=fieldsTable.size()) return;

 // Заполняются имена директории и полей
 // Директория при проверке создается если ее небыло
 QString nameDirFull;
 QString nameFileFull;
 if(checkAndFillFileDir(pos, nameDirFull, nameFileFull)==false)
  critical_error("RecordTableData::set_text() as String : For record "+QString::number(pos)+" can not set field \"dir\" or \"file\"");

 // Текст записи сохраняется в файл
 QFile wfile(nameFileFull);
 if(mode==0)
  {
   if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
    critical_error("Cant open text file "+nameFileFull+" for write.");
   QTextStream out(&wfile);
   out.setCodec("UTF-8");
   out << textString;
  }
 else if(mode==1)
  {
   if(!wfile.open(QIODevice::WriteOnly))
    critical_error("Cant open binary file "+nameFileFull+" for write.");
   wfile.write(textByteArray);
  }

 // Если есть какие-то файлы, сопровождащие запись,
 // они вставляются в конечную директорию
 if(files.size()>0)
  save_files_to_directory(nameDirFull, files);
}
*/


// Функция проверяет наличие полей dir и file (они используются для текста)
// проверяет их правильность и заполняет полные имена директории и файла
bool RecordTableData::checkAndFillFileDir(int pos, QString &nameDirFull, QString &nameFileFull)
{
  // Если у записи не установлены поля dir и file
 if(getFields(pos).contains("dir")==false ||
    getFields(pos).contains("file")==false)
  {
   nameDirFull="";
   nameFileFull="";
   return false;
  }

 // Выясняются имена директории и файла
 QString nameDir=(getFields(pos))["dir"];
 QString nameFile=(getFields(pos))["file"];

 // Полные имена директории и файла
 nameDirFull=mytetraconfig.get_tetradir()+"/base/"+nameDir;
 nameFileFull=nameDirFull+"/"+nameFile;

 // Проверяется наличие директории, куда будет вставляться файл с текстом записи
 QDir recordDir(nameDirFull);
 if(!recordDir.exists())
  {
   // Создается новая директория в директории base
   QDir directory(mytetraconfig.get_tetradir()+"/base");
   directory.mkdir(nameDir);
  }

 return true;
}


// Получение значений всех инфополей
// Поля, которые могут быть у записи, но не заданы, не передаются
// Поля, которые зашифрованы, расшифровываются
QMap<QString, QString> RecordTableData::getFields(int pos) const
{
 // Если индекс недопустимый, возвращается пустой список полей
 if(pos<0 || pos>=fieldsTable.size()) return QMap<QString, QString>();

 // Список имен инфополей
 QStringList fieldNames=fieldNameAvailableList();
 
 // В linetmp копируется запись (только инфополя) с нужным номером
 QMap<QString, QString> lineTmp;
 lineTmp=fieldsTable.at(pos);

 // qDebug() << "RecordTableData::get_fields() : pos"<<pos<<"lineTmp:"<<lineTmp;

 QMap<QString, QString> tmpRecordFields;


 // Проверяется, используется ли шифрование
 bool isCrypt=false;

 if(lineTmp.contains("crypt"))
  if(lineTmp["crypt"]=="1")
   isCrypt=true;


 // Перебираются названия полей
 for(int i=0;i<fieldNames.size();++i)
  {
   QString currName=fieldNames.at(i);

   // Если поле с таким именем существует
   if(lineTmp.contains( currName ))
    {
     QString result="";

     if(isCrypt==false)
      result=lineTmp[currName]; // Напрямую значение поля
     else
      {
       // Присутствует шифрование

       // Если поле не подлежит шифрованию
       if(fieldNameForCryptList().contains(currName)==false)
        result=lineTmp[currName]; // Напрямую значение поля
       else
        if(globalParameters.getCryptKey().length()>0 &&
           fieldNameForCryptList().contains(currName))
        result=decryptString(globalParameters.getCryptKey(), lineTmp[currName]); // Расшифровывается значение поля
      }

     tmpRecordFields[currName]=result;
    }
  }

  // else
  //  tmpRecordFields[fieldNames.at(i)]="";

 qDebug() << "RecordTableData::get_fields() : pos"<<pos<<"Data:"<<tmpRecordFields;

 return tmpRecordFields;
}


// Получение полного образа записи
QMap<QString, QString> RecordTableData::getRecordExemplar(int pos)
{
 // Если индекс недопустимый, возвращается пустой список данных
 if(pos<0 || pos>=size())
  return QMap<QString, QString>();

 QMap<QString, QString> record;
 
 record=getFields(pos);  // Все инфополя
 record["text"]=getText(pos); // Текст записи

 return record;
}


// Инициализация таблицы данных на основе переданного DOM-элемента
void RecordTableData::init(TreeItem *item, QDomElement domModel)
{
  // Создание таблицы
  if(!domModel.isNull())
   {
    QDomElement *dml=&domModel;
    setupDataFromDom(dml);
   }

  // Запоминается ссылка на ветку, которой принадлежит данная таблица
  treeItem=item;
}


// Разбор DOM модели и преобразование ее в таблицу
void RecordTableData::setupDataFromDom(QDomElement *domModel)
{
  // QDomElement n = dommodel.documentElement();
  // QDomElement n = dommodel;

  // qDebug() << "In recordtabledata setup_data_from_dom() start";
 
  // Если принятый элемент не является таблицей
  if(domModel->tagName()!="recordtable") return;
 
  // Определяется указатель на первый элемент с записью
  QDomElement currentRec=domModel->firstChildElement();
  
  while(!currentRec.isNull())
   {
    if(currentRec.tagName()=="record")
     {
      // Получение списка всех атрибутов текущего элемента
      QDomNamedNodeMap attList;
      attList=currentRec.attributes();

      // Создается объект одной записи
      QMap<QString, QString> tmpline;

      // Перебор атрибутов в списке и добавление их в объект одной записи
      int i;
      for(i=0; i<attList.count(); i++)
       {
        QDomAttr attcurr=attList.item(i).toAttr();

        QString name=attcurr.name();
        QString value=attcurr.value();

        tmpline[name]=value;

        // Распечатка считанных данных в консоль
        // qDebug() << "Read record attr " << name << value;
       }

      // Данная запись добавляется в таблицу конечных записей
      fieldsTable << tmpline;
     }

    currentRec=currentRec.nextSiblingElement();
   }

  return;
}


// Преобразование таблицы конечных записей в Dom документ
QDomDocument RecordTableData::exportDataToDom(void)
{
 // Если у ветки нет таблицы конечных записей, возвращается пустой документ
 if(fieldsTable.size()==0)return QDomDocument();
 
 QDomDocument doc;

 QDomElement recordTableDomData = doc.createElement("recordtable");
 doc.appendChild(recordTableDomData);

 QStringList fieldsNamesAvailable=fieldNameAvailableList();

 // Пробегаются все записи в таблице
 for(int i=0; i<fieldsTable.size(); i++)
 {
  QDomElement elem = doc.createElement("record");

  QMap<QString, QString> lineTmp;
  lineTmp=fieldsTable.at(i);

  // Перебираются допустимые имена полей
  for(int j=0; j<fieldsNamesAvailable.size(); ++j)
   {
    QString currentFieldName=fieldsNamesAvailable.at(j);

    // Устанавливается значение поля как атрибут DOM-узла
    if(lineTmp.contains(currentFieldName))
     elem.setAttribute(currentFieldName, lineTmp[currentFieldName]);
   }

  // К элементу recordtabledata прикрепляются конечные записи
  doc.firstChild().appendChild(elem);
 }

 // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

 return doc;
}


// Добавление новой записи
// Метод только добавляет во внутреннее представление новые данные,
// сохраняет текст файла и обновляет данные на экране.
// Сохранения дерева XML-данных на диск в этом методе нет.
// Допустимые режимы:
// ADD_NEW_RECORD_TO_END - в конец списка, pos игнорируется
// ADD_NEW_RECORD_BEFORE - перед указанной позицией, pos - номер позиции
// ADD_NEW_RECORD_AFTER - после указанной позиции, pos - номер позиции
int RecordTableData::insertNewRecord(int mode,
                                     int pos,
                                     QMap<QString, QString> fields,
                                     QString text,
                                     QMap<QString, QByteArray> files)
{
  qDebug() << "RecordTableData::insert_new_record() : Insert new record to branch " << treeItem->getAllFields();

  // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
  bool isCrypt=false;
  if(treeItem!=NULL)
   if(treeItem->getField("crypt")=="1")
    {
     if(globalParameters.getCryptKey().length()>0)
      isCrypt=true;
     else
      critical_error("RecordTableData::insert_new_record() : Can not insert data to crypt branch. Password not setted.");
    }


  // В список переданных полей добавляются вычислимые в данном месте поля

  // Наличие шифрации
  if(isCrypt) fields["crypt"]="1";
  else fields["crypt"]="0";

  // Выясняется, есть ли в дереве запись с указанным ID
  // Если есть, то генерируются новые ID для записи и новая директория хранения
  // Если нет, то это значит что запись была вырезана, но хранится в буфере,
  // и ее желательно вставить с прежним ID и прежнии именем директории
  KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("KnowTreeView")->model());
  if(fields["id"].length()==0 ||
     dataModel->isRecordIdExists( fields["id"] ) )
   {
    // Создается новая запись (ID был пустой) или
    // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)

    // Директория хранения записи и файл
    fields["dir"]=get_unical_id();
    fields["file"]="text.html";

    // Уникальный идентификатор XML записи
    QString id=get_unical_id();
    fields["id"]=id;
   }


  // Время создания данной записи
  QDateTime ctime_dt=QDateTime::currentDateTime();
  QString ctime=ctime_dt.toString("yyyyMMddhhmmss");
  fields["ctime"]=ctime;
  

  // Добавляются инфополя объекта
  int insertPos=0;
  QMap<QString, QString> emptyRecord;

  // Вначале добавляется пустая запись
  if(mode==ADD_NEW_RECORD_TO_END) // В конец списка
   {
    fieldsTable << emptyRecord;
    insertPos=fieldsTable.size()-1;
   }
  else if(mode==ADD_NEW_RECORD_BEFORE) // Перед указанной позицией
   {
    fieldsTable.insert(pos, emptyRecord);
    insertPos=pos;
   }
  else if(mode==ADD_NEW_RECORD_AFTER) // После указанной позиции
   {
    fieldsTable.insert(pos+1, emptyRecord);
    insertPos=pos+1;
   }

  // Запись заполняется данными
  
  // Вначале на всякий случай устанавливается значение поля наличия шифрования
  setField("crypt", fields["crypt"], insertPos);

  // Устанавливается весь набор полей
  QMapIterator<QString, QString> i(fields);
  while(i.hasNext())
   {
    i.next();

    qDebug() << "RecordTableData::insert_new_record() : Set field " << i.key() << " value " << i.value();
    setField(i.key(), i.value(), insertPos);
   }


  // Добавляется текст и файлы объекта
  setRecordData(insertPos, text, files);

  qDebug() << "RecordTableData::insert_new_record() : New record pos" << QString::number(insertPos);

  // Возвращается номера строки, на которую должна быть установлена засветка
  // после выхода из данного метода
  return insertPos;
}


// Замена в указанной записи переданных полей на новые значения
void RecordTableData::editRecordFields(int pos,
                                       QMap<QString, QString> editFields)
{
 qDebug() << "In recordtabledata method edit_record()";

 // QMap<QString, QString> tmpline=getMergeFields(pos, editFields);
 // fieldsTable.replace(pos, tmpline);

 QMapIterator<QString, QString> i(editFields);
 while(i.hasNext())
  {
   i.next();
   setField(i.key(), i.value(), pos);
  }

 // changePersistentIndex(QModelIndex(), QModelIndex());
}


// Удаление записи с указанным индексом
void RecordTableData::deleteRecord(int i)
{
 qDebug() << "Try delete record num " << i << " table count " << fieldsTable.size();

 // Нельзя удалять с недопустимым индексом
 if(i>=fieldsTable.size())return;

 // Удаление директории и файлов внутри, с сохранением в резервной директории
 QString dirForDelete=mytetraconfig.get_tetradir()+"/base/"+getField("dir",i);
 qDebug() << "Remove dir " << dirForDelete;
 remove_directory_to_trash( dirForDelete );


 // Удаление позиции курсора из истории
 QString id=getField("id", i);
 if(id.length()>0)
  walkHistory.removeHistoryData(id);

 // Начинается удаление записи
 // beginRemoveRows(QModelIndex(),i,i);

 // Удаляется элемент
 fieldsTable.removeAt(i); // Было takeAt
 qDebug() << "Delete record succesfull";

 // Удаление записи закончено
 // endRemoveRows();
}


// Удаление всех элементов таблицы конечных записей
void RecordTableData::deleteAllRecords(void)
{
 for(int i=0;i<size();i++)
  deleteRecord(i);
}


// Метод мягкого удаления данных
// Данные очищаются только у объекта
// а физически данные на диске не затрагиваются
void RecordTableData::empty(void)
{
 fieldsTable.clear();
 treeItem=NULL;
}


// Количество записей в таблице данных
int RecordTableData::size(void) const
{
 return fieldsTable.size();
}


// Перемещение записи вверх на одну строку
void RecordTableData::moveUp(int pos)
{
 if(pos>0)
  {
   // Данные перемещаются
   fieldsTable.move(pos,pos-1);

   // Обновляется экран
   // QModelIndex from=index(pos-1);
   // QModelIndex to=index(pos);
   // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
  }
}


// Перемещение записи вниз на одну строку
void RecordTableData::moveDn(int pos)
{
 if(pos<fieldsTable.count())
  {
   // Данные перемещаются
   fieldsTable.move(pos,pos+1);

   // Обновляется экран
   // QModelIndex from=index(pos);
   // QModelIndex to=index(pos+1);
   // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
  }
}


// Переключение таблицы в зашифрованное состояние
void RecordTableData::switchToEncrypt(void)
{
 // Перебор записей
 for(int i=0; i<size(); i++)
  {
   // Если запись уже зашифрована, ее шифровать ненужно
   if(getField("crypt", i)=="1")
    continue;

   // ---------------------
   // Шифрация полей записи
   // ---------------------


   // Поля записей, незашифрованные
   QMap<QString, QString> recordFields=getFields(i);

   // Устанавливается поле что запись зашифрована
   setField("crypt", "1", i);

   // Выбираются поля, разрешенные для шифрования
   foreach(QString fieldName, fieldNameForCryptList())
   {
    // Если в полях записей присутствует очередное разрешенное имя поля
    // И это поле непустое
    // Поле шифруется
    if(recordFields.contains(fieldName))
     if(recordFields[fieldName].length()>0)
      {
       // Устанавливаются значения, при установке произойдет шифрация
       setField(fieldName, recordFields[fieldName], i);

       /*
       set_field(fieldName,
                 encryptString(globalParameters.getCryptKey(), recordFields[fieldName]),
                 i);
       */
      }
   }
   

   // -------------------------------
   // Шифрация файла с текстом записи
   // -------------------------------
   // set_text_internal(i, encryptStringToByteArray(globalParameters.getCryptKey(), get_text(i)) );
   QString nameDirFull;
   QString nameFileFull;
   if(checkAndFillFileDir(i, nameDirFull, nameFileFull)==false)
    critical_error("RecordTableData::switchToEncrypt() : For record "+QString::number(i)+" can not set field \"dir\" or \"file\"");
   encryptFile(globalParameters.getCryptKey(), nameFileFull);
  }
}


// Переключение таблицы в расшифрованное состояние
void RecordTableData::switchToDecrypt(void)
{
 // Перебор записей
 for(int i=0; i<size(); i++)
  {
   // Если запись не зашифрована, ее не нужно расшифровывать
   if(getField("crypt", i)=="" || getField("crypt", i)=="0")
    continue;

   // ------------------------
   // Расшифровка полей записи
   // ------------------------

   // Поля записей, расшифрованные
   QMap<QString, QString> recordFields=getFields(i);

   // Затем устанавливается флаг что шифрации нет
   setField("crypt", "0", i);

   // Все поля, подлежащие шифрованию, заполняются расшифрованными значениями
   foreach(QString fieldName, fieldNameForCryptList())
   {
    // Если в полях записей присутствует очередное разрешенное имя поля
    // И это поле непустое
    // Поле расшифровывается
    if(recordFields.contains(fieldName))
     if(recordFields[fieldName].length()>0)
      {
       // Устанавливаются значения расшифрованными данными
       setField(fieldName, recordFields[fieldName], i);

       /*
       set_field(fieldName,
                 decryptString(globalParameters.getCryptKey(), recordFields[fieldName]),
                 i);
       */
      }
   }

   // ----------------------------------
   // Расшифровка файла с текстом записи
   // ----------------------------------
   // set_text_internal(i, decryptStringFromByteArray(globalParameters.getCryptKey(), get_text_as_byte_array(i)) );
   QString nameDirFull;
   QString nameFileFull;
   if(checkAndFillFileDir(i, nameDirFull, nameFileFull)==false)
    critical_error("RecordTableData::switchToDecrypt() : For record "+QString::number(i)+" can not set field \"dir\" or \"file\"");
   decryptFile(globalParameters.getCryptKey(), nameFileFull);

   // Устанавливается флаг что запись не зашифрована
   setField("crypt", "0", i);
  }
}


bool RecordTableData::isFieldNameAvailable(QString name) const
{
 if(fieldNameAvailableList().contains(name))
  return true;
 else
  return false;
}


QStringList RecordTableData::fieldNameAvailableList(void) const
{
 QStringList names;

 names << "id";
 names << "name";
 names << "author";
 names << "url";
 names << "tags";
 names << "ctime";
 names << "dir";
 names << "file";
 names << "crypt";

 return names;
}


QStringList RecordTableData::fieldNameForCryptList(void) const
{
 QStringList names;

 names << "name";
 names << "author";
 names << "url";
 names << "tags";

 return names;
}


// Метод, возвращающий набор полей и их значений, полученный путем слияния
// данных указанной записи и переданного набора полей
QMap<QString, QString> RecordTableData::getMergeFields(int pos, QMap<QString, QString> fields)
{
 QMap<QString, QString> resultFields;

 // Если номер позиции превышает количество записей в таблице
 if(pos>=size())
  critical_error("In RecordTableData::getMergeFields bad record pos "+QString::number(pos));

 // Список допустимых имен инфополей
 QStringList fieldNames=fieldNameAvailableList();

 // Перебираются допустимые имена полей
 for(int i=0; i<fieldNames.size(); ++i)
  {
   QString currentName=fieldNames.at(i);

   // Если поле с таким именем существует в переданном списке
   if(fields.contains( currentName ))
    resultFields[currentName]=fields[currentName]; // Запоминается значение
   else
    {
     // Иначе в переданном списке нет поля с текущим допустимым именем

     // Проверяется, есть ли уже поле с таким именем в таблице сущаствующих полей
     if((getFields(pos)).contains(currentName))
      resultFields[currentName]=getField(currentName, pos); // Запоминается значение
    }
  }

 return resultFields;
}


// Получение ссылки на объект ветки, которой принадлежит таблица
TreeItem *RecordTableData::getItem(void)
{
 return treeItem;
}


int RecordTableData::getWorkPos(void)
{
 return workPos;
}


void RecordTableData::setWorkPos(int pos)
{
 workPos=pos;
}
