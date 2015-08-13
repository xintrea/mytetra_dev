#include <QDomElement>
#include <QMap>
#include <QString>
#include <QDir>
#include <QMessageBox>

#include "main.h"
#include "Record.h"
#include "RecordTableData.h"

#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"
#include "models/tree/TreeItem.h"
#include "libraries/WalkHistory.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"

#include "libraries/wyedit/Editor.h"

extern AppConfig mytetraConfig;
extern FixedParameters fixedParameters;
extern GlobalParameters globalParameters;
extern WalkHistory walkHistory;


// Это набор данных конечной таблицы, с которыми удобно работать

// Конструктор
RecordTableData::RecordTableData(QObject *pobj) : QObject(pobj)
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
// Имя поля - все возможные имена полей, кроме text (такого поля теперь вообще нет, текст запрашивается как отдельное свойство)
QString RecordTableData::getField(QString name, int pos) const
{
  // Если индекс недопустимый
  if(pos<0 || pos>=tableData.size())
  {
    QString i;
    i.setNum(pos);
    critical_error("RecordTableData::getField() : get unavailable record index "+i);
  }

  return tableData.at(pos).getField(name);
}


// Установка значения указанного поля для указанного элемента
void RecordTableData::setField(QString name, QString value, int pos)
{
  // Если индекс недопустимый
  if(pos<0 || pos>=tableData.size())
  {
    QString i;
    i.setNum(pos);
    critical_error("In RecordTableData::setField() unavailable record index "+i+" in table while field "+name+" try set to "+value);
  }

 tableData[pos].setField(name, value);
}


// Получение значения текста указанной записи
// Если возникнет проблема, что файла с текстом записи нет, будет создан пустой файл
QString RecordTableData::getText(int pos)
{
 // Если индекс недопустимый, возвращается пустая строка
 if(pos<0 || pos>=size()) 
   return QString();

 return tableData[pos].getTextDirect();
}


// Функция, которая заменяет стандартную функцию редактора по считыванию
// редактируемого текста
// Ее вызывает редактор, передавая указатель на себя
// и ссылку на переменную loadText, которую надо заполнить
// Внимание! Метод не содержит работы с данными записи. Подумать, где его разместить
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

 // Файл, с которым работает редактор
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
// Внимание! Метод не содержит работы с данными записи. Подумать, где его разместить
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


// Получение списка приаттаченных файлов указанного элемента
// Todo: сделать работу с шифрованием
QMap<QString, QString> RecordTableData::getAttachList(int pos) const
{
  return tableData.at(pos).getAttachList();
}


// Получение легкого образа записи
// Поля, которые зашифрованы, расшифровываются
Record RecordTableData::getRecordLite(int pos)
{
 // Если индекс недопустимый, возвращается пустая запись
 if(pos<0 || pos>=size())
   return Record();

 // Хранимая в дереве запись не может быть "тяжелой"
 if(!tableData.at(pos).isLite())
   critical_error("In RecordTableData::getRecordLite() try get fat record");

 // Todo: доделать с расшифровкой
 return tableData.at(pos);
}


// Получение полного образа записи
Record RecordTableData::getRecordFat(int pos)
{
 // Копия записи из дерева
 Record resultRecord=getRecordLite(pos);

 // Превращение копии записи на режим с хранением полного содержимого
 resultRecord.switchToFat();

 // Добавление текста записи
 resultRecord.setText( getText(pos) );

 // Добавление бинарных образов файлов картинок и приаттаченных файлов
 QString directory=mytetraConfig.get_tetradir()+"/base/"+resultRecord.getField("dir");
 resultRecord.setPictureFiles( get_files_from_directory(directory, "*.png") );

 // Todo: проверить расшифровку
 return resultRecord;
}


Record *RecordTableData::getRecord(int pos)
{
  // Если индекс недопустимый, возвращается пустая запись
  if(pos<0 || pos>=size())
    return NULL;

  return &(tableData[pos]);
}


// Инициализация таблицы данных на основе переданного DOM-элемента
void RecordTableData::init(TreeItem *item, QDomElement iDomElement)
{
  // Создание таблицы
  if(!iDomElement.isNull())
   {
    QDomElement *domElement=&iDomElement;
    setupDataFromDom(domElement);
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
  if(domModel->tagName()!="recordtable")
    return;
 
  // Определяется указатель на первый элемент с записью
  QDomElement currentRec=domModel->firstChildElement();
  
  while(!currentRec.isNull())
   {
    if(currentRec.tagName()=="record")
     {
      // Структура, куда будет помещена текущая запись
      Record currentRecordData;
      currentRecordData.setupDataFromDom(currentRec);

      // Текущая запись добавляется в таблицу конечных записей
      tableData << currentRecordData;
     }

    currentRec=currentRec.nextSiblingElement();
   } // Закрылся цикл перебора тегов <record ...>

  return;
}


// Преобразование таблицы конечных записей в Dom документ
QDomDocument RecordTableData::exportDataToDom(void)
{
  // Если у ветки нет таблицы конечных записей, возвращается пустой документ
  if(tableData.size()==0)
    return QDomDocument();

  QDomDocument doc;

  QDomElement recordTableDomData = doc.createElement("recordtable");
  doc.appendChild(recordTableDomData);

  QStringList fieldsNamesAvailable=fixedParameters.recordFieldAvailableList();

  // Пробегаются все записи в таблице
  for(int i=0; i<tableData.size(); i++)
  {
    QDomElement elem = doc.createElement("record");

    // QMap<QString, QString> lineTmp;
    // lineTmp=tableData.at(i).getFieldList();
    // QMap<QString, QString> lineTmp;
    // lineTmp=tableData.at(i).getFieldListDirect();

    // Перебираются допустимые имена полей
    for(int j=0; j<fieldsNamesAvailable.size(); ++j)
    {
      QString currentFieldName=fieldsNamesAvailable.at(j);

      // Устанавливается значение поля как атрибут DOM-узла
      if(tableData.at(i).isFieldExists(currentFieldName))
        elem.setAttribute(currentFieldName, tableData.at(i).getFieldSource(currentFieldName));
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
// Метод принимает "тяжелый" объект записи
// Объект для вставки всегда приходит незашифрованный
// todo: добавить сохранение таблицы приаттаченных файлов и содержимого файлов
int RecordTableData::insertNewRecord(int mode,
                                     int pos,
                                     Record record)
{
  qDebug() << "RecordTableData::insert_new_record() : Insert new record to branch " << treeItem->getAllFields();

  // Мотод должен принять полновесный объект записи
  if(record.isLite()==true)
    critical_error("RecordTableData::insertNewRecord() can't insert lite record");

  // Выясняется, есть ли в дереве запись с указанным ID
  // Если есть, то генерируются новые ID для записи и новая директория хранения
  // Если нет, то это значит что запись была вырезана, но хранится в буфере,
  // и ее желательно вставить с прежним ID и прежним именем директории
  KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());
  if(record.getField("id").length()==0 ||
     dataModel->isRecordIdExists( record.getField("id") ) )
   {
    // Создается новая запись (ID был пустой) или
    // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)

    // Директория хранения записи и файл
    record.setField("dir", get_unical_id());
    record.setField("file", "text.html");

    // Уникальный идентификатор XML записи
    QString id=get_unical_id();
    record.setField("id", id);
   }


  // В список переданных полей добавляются вычислимые в данном месте поля

  // Время создания данной записи
  QDateTime ctime_dt=QDateTime::currentDateTime();
  QString ctime=ctime_dt.toString("yyyyMMddhhmmss");
  record.setField("ctime", ctime);


  // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
  bool isCrypt=false;
  if(treeItem!=NULL)
   if(treeItem->getField("crypt")=="1")
    {
     if(globalParameters.getCryptKey().length()>0)
      isCrypt=true;
     else
      critical_error("RecordTableData::insertNewRecord() : Can not insert data to crypt branch. Password not setted.");
    }

  // Запись полновесных данных с учетом шифрации
  if(isCrypt && record.getField("crypt")!="1") // В зашифрованную ветку незашифрованную запись
    record.switchToEncryptAndSaveFat();
  else if (!isCrypt && record.getField("crypt")=="1") // В незашифрованную ветку зашифрованную запись
    record.switchToDecryptAndSaveFat();
  else
    record.pushFatAttributes();

  // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
  record.switchToLite();

  // Запись добавляется в таблицу конечных записей
  int insertPos=-1;
  if(mode==ADD_NEW_RECORD_TO_END) // В конец списка
   {
    tableData << record;
    insertPos=tableData.size()-1;
   }
  else if(mode==ADD_NEW_RECORD_BEFORE) // Перед указанной позицией
   {
    tableData.insert(pos, record);
    insertPos=pos;
   }
  else if(mode==ADD_NEW_RECORD_AFTER) // После указанной позиции
   {
    tableData.insert(pos+1, record);
    insertPos=pos+1;
   }

  qDebug() << "RecordTableData::insert_new_record() : New record pos" << QString::number(insertPos);

  // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода
  return insertPos;
}


// Замена в указанной записи переданных полей на новые значения
void RecordTableData::editRecordFields(int pos,
                                       QMap<QString, QString> editFields)
{
 qDebug() << "In recordtabledata method edit_record()";

 QMapIterator<QString, QString> i(editFields);
 while(i.hasNext())
  {
   i.next();
   setField(i.key(), i.value(), pos);
  }

 // changePersistentIndex(QModelIndex(), QModelIndex());
}


// Удаление записи с указанным индексом
// todo: добавить удаление приаттаченных файлов и очистку таблицы приаттаченных файлов
void RecordTableData::deleteRecord(int i)
{
 qDebug() << "Try delete record num " << i << " table count " << tableData.size();

 // Нельзя удалять с недопустимым индексом
 if(i>=tableData.size())
   return;

 // Удаление директории и файлов внутри, с сохранением в резервной директории
 QString dirForDelete=mytetraConfig.get_tetradir()+"/base/"+getField("dir",i);
 qDebug() << "Remove dir " << dirForDelete;
 remove_directory_to_trash( dirForDelete );


 // Удаление позиции курсора из истории
 QString id=getField("id", i);
 if(id.length()>0)
  walkHistory.removeHistoryData(id);

 // Начинается удаление записи
 // beginRemoveRows(QModelIndex(),i,i);

 // Удаляется элемент
 tableData.removeAt(i); // Было takeAt
 qDebug() << "Delete record succesfull";

 // Удаление записи закончено
 // endRemoveRows();
}


void RecordTableData::deleteRecordById(QString id)
{
  for(int i=0; i<size(); i++)
    if(getField("id", i)==id)
      deleteRecord(i); // Так как id уникальный, удаляться будет только одна запись
}


// Удаление всех элементов таблицы конечных записей
void RecordTableData::deleteAllRecords(void)
{
  int tableSize=size(); // Запоминается размер таблицы, так как он при удалении меняется

  for(int i=0; i<tableSize; i++)
    deleteRecord(0); // Удаляется самая первая запись много раз
}


// Метод мягкого удаления данных
// Данные очищаются только у объекта
// а физически данные на диске не затрагиваются
void RecordTableData::empty(void)
{
 tableData.clear();
 treeItem=NULL;
}


bool RecordTableData::isRecordExists(QString id)
{
  for(int i=0;i<size();i++)
    if(getField("id", i)==id)
      return true;

  return false;
}


int RecordTableData::getPosById(QString id)
{
  for(int i=0;i<size();i++)
    if(getField("id", i)==id)
      return i;

  return -1;
}


// Количество записей в таблице данных
int RecordTableData::size(void) const
{
 return tableData.size();
}


// Перемещение записи вверх на одну строку
void RecordTableData::moveUp(int pos)
{
 if(pos>0)
  {
   // Данные перемещаются
   tableData.move(pos,pos-1);

   // Обновляется экран
   // QModelIndex from=index(pos-1);
   // QModelIndex to=index(pos);
   // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
  }
}


// Перемещение записи вниз на одну строку
void RecordTableData::moveDn(int pos)
{
 if(pos<tableData.count())
  {
   // Данные перемещаются
   tableData.move(pos,pos+1);

   // Обновляется экран
   // QModelIndex from=index(pos);
   // QModelIndex to=index(pos+1);
   // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
  }
}


// Переключение таблицы в зашифрованное состояние
// todo: Добавить шифрацию имени приаттаченных файлов и содержимого файлов
void RecordTableData::switchToEncrypt(void)
{
  // Перебор записей
  for(int i=0; i<size(); i++)
  {
    // Если запись уже зашифрована, ее шифровать ненужно
    if(getField("crypt", i)=="1")
      continue;

    // Шифрация записи
    tableData[i].switchToEncryptAndSaveLite(); // В таблице конечных записей хранятся легкие записи
  }
}


// Переключение таблицы в расшифрованное состояние
// todo: добавить расшифрацию имени приаттаченных файлов и содержимого файлов
void RecordTableData::switchToDecrypt(void)
{
 // Перебор записей
 for(int i=0; i<size(); i++)
  {
   // Если запись не зашифрована, ее не нужно расшифровывать
   if(getField("crypt", i)=="" || getField("crypt", i)=="0")
    continue;

   // Расшифровка записи
   tableData[i].switchToDecryptAndSaveLite(); // В таблице конечных записей хранятся легкие записи
  }
}


// Метод, возвращающий набор полей и их значений, полученный путем слияния
// данных указанной записи и переданного набора полей
// Todo: метод не используется, подумать, а не убрать ли его
QMap<QString, QString> RecordTableData::getMergeFields(int pos, QMap<QString, QString> fields)
{
 QMap<QString, QString> resultFields;

 // Если номер позиции превышает количество записей в таблице
 if(pos>=size())
  critical_error("In RecordTableData::getMergeFields bad record pos "+QString::number(pos));

 // Список допустимых имен инфополей
 QStringList fieldNames=fixedParameters.recordFieldAvailableList();

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
     if(tableData.at(pos).isFieldExists(currentName))
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
