#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QSplitter>
#include <QWidget>
#include <QProgressDialog>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QBoxLayout>
#include <QTextDocument>
#include <QLineEdit>
#include <QByteArray>

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "FindScreen.h"
#include "FindTableWidget.h"
#include "models/tree/KnowTreeModel.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/TreeItem.h"
#include "views/record/MetaEditor.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"
#include "libraries/MtComboBox.h"
#include "views/tree/KnowTreeView.h"

extern AppConfig mytetraconfig;
extern GlobalParameters globalParameters;


FindScreen::FindScreen(QWidget *parent) : QWidget(parent)
{
 setupToolsLine();
 assemblyToolsLine();

 setupWhereFindLine();
 assemblyWhereFindLine();

 setupUI();
 assembly();
 
 setupSignals();
}


FindScreen::~FindScreen(void)
{

}

void FindScreen::setupToolsLine(void)
{
 findText=new QLineEdit();
  
 findStartButton=new QPushButton(this);
 findStartButton->setText(tr("Find"));
 findStartButton->setDefault(true);
 findStartButton->setEnabled(false);
 
 wordRegard=new MtComboBox();
 wordRegard->addItem(QIcon(":/resource/pic/find_in_base_any.svg"), tr("Any word"));
 wordRegard->addItem(QIcon(":/resource/pic/find_in_base_all.svg"), tr("All words"));
 wordRegard->setCurrentIndex(mytetraconfig.get_findscreen_wordregard());
 
 howExtract=new MtComboBox();
 howExtract->addItem(QIcon(":/resource/pic/find_in_base_separate.svg"), tr("Whole words"));
 howExtract->addItem(QIcon(":/resource/pic/find_in_base_substring.svg"), tr("Substring"));
 howExtract->setCurrentIndex(mytetraconfig.get_findscreen_howextract());

 treeSearchArea=new MtComboBox();
 treeSearchArea->addItem(QIcon(":/resource/pic/find_in_base_treesearcharea_all.svg"), tr("Entire base")); // Вся база
 treeSearchArea->addItem(QIcon(":/resource/pic/find_in_base_treesearcharea_branch.svg"), tr("In current branch")); // Текущая ветка
 treeSearchArea->setCurrentIndex(mytetraconfig.getFindScreenTreeSearchArea());
 
 closeButton=new QToolButton(this);
 closeButton->setVisible(true);
 int w=closeButton->geometry().width();
 int h=closeButton->geometry().height();
 int x=imin(w,h)/2;
 closeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
 closeButton->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
 closeButton->setMinimumSize(x,x);
 closeButton->setMaximumSize(x,x);
 closeButton->resize(x,x);

}


// Строка формирование поискового запроса с кнопочкой закрытия
void FindScreen::assemblyToolsLine(void)
{
 // Горизонтальный набор инструментов запроса
 // с распоркой чтобы область кнопки была справа
 toolsLine=new QHBoxLayout();
 toolsLine->addWidget(findText);
 toolsLine->addWidget(findStartButton);
 toolsLine->addWidget(wordRegard);
 toolsLine->addWidget(howExtract);
 toolsLine->addWidget(treeSearchArea);
 toolsLine->addStretch();
 
 // Вертикальная область с кнопкой закрытия и распоркой 
 // чтобы кнопка была вверху
 placeUpCloseButton=new QVBoxLayout();
 placeUpCloseButton->setContentsMargins(0,0,0,0);
 placeUpCloseButton->addWidget(closeButton);
 placeUpCloseButton->addStretch();
 toolsLine->addLayout(placeUpCloseButton);
 
 toolsLine->setContentsMargins(3,2,2,0); // Устанавливаются границы
}


void FindScreen::setupWhereFindLine(void)
{
 whereFindLabel=new QLabel(tr("Find in: "));

 findInName=new QCheckBox(tr("Title"));
 findInName->setChecked(mytetraconfig.get_findscreen_find_in_field("name"));

 findInAuthor=new QCheckBox(tr("Author(s)"));
 findInAuthor->setChecked(mytetraconfig.get_findscreen_find_in_field("author"));

 findInUrl=new QCheckBox(tr("Url"));
 findInUrl->setChecked(mytetraconfig.get_findscreen_find_in_field("url"));

 findInTags=new QCheckBox(tr("Tags"));
 findInTags->setChecked(mytetraconfig.get_findscreen_find_in_field("tags"));

 findInText=new QCheckBox(tr("Text"));
 findInText->setChecked(mytetraconfig.get_findscreen_find_in_field("text"));
}


void FindScreen::assemblyWhereFindLine(void)
{
 whereFindLine=new QHBoxLayout();
 
 whereFindLine->addWidget(whereFindLabel);
 whereFindLine->addWidget(findInName);
 whereFindLine->addWidget(findInAuthor);
 whereFindLine->addWidget(findInUrl);
 whereFindLine->addWidget(findInTags);
 whereFindLine->addWidget(findInText);
 
 whereFindLine->addStretch();
 
 whereFindLine->setContentsMargins(3,0,0,0); // Устанавливаются границы
}


void FindScreen::setupSignals(void)
{
 // При каждом изменении текста в строке запроса
 connect(findText, SIGNAL(textChanged(const QString&)),
         this, SLOT(enableFindButton(const QString&)));

 // При каждом изменении текста извне может вырабатыватся этот сигнал
 // Он вырабатывается в слоте setFindText()
 connect(this, SIGNAL(textChangedFromAnother(const QString&)),
         this, SLOT(enableFindButton(const QString&)));


 // При нажатии Enter в строке запроса
 connect(findText, SIGNAL(returnPressed()),
         this, SLOT(findClicked()));
 
 // При нажатии кнопки Find
 connect(findStartButton,SIGNAL(clicked()),
         this,SLOT(findClicked()));

 // После установки текста извне, вырабатывается этот сигнал
 connect(this, SIGNAL(findClickedAfterAnotherTextChanged()),
         this, SLOT(findClicked()));


 // При нажатии кнопки закрытия
 connect(closeButton,SIGNAL(clicked()),
         this,SLOT(widgetHide()));
 
 // Сигналы для запоминания состояния интерфейса
 connect(wordRegard,SIGNAL(currentIndexChanged(int)),
         this,SLOT(changedWordRegard(int)));

 connect(howExtract,SIGNAL(currentIndexChanged(int)),
         this,SLOT(changedHowExtract(int)));
 
 connect(treeSearchArea,SIGNAL(currentIndexChanged(int)),
         this,SLOT(changedTreeSearchArea(int)));

 connect(findInName,SIGNAL(stateChanged(int)),
         this,SLOT(changedFindInName(int)));

 connect(findInAuthor,SIGNAL(stateChanged(int)),
         this,SLOT(changedFindInAuthor(int)));

 connect(findInUrl,SIGNAL(stateChanged(int)),
         this,SLOT(changedFindInUrl(int)));

 connect(findInTags,SIGNAL(stateChanged(int)),
         this,SLOT(changedFindInTags(int)));

 connect(findInText,SIGNAL(stateChanged(int)),
         this,SLOT(changedFindInText(int)));
}


void FindScreen::setupUI(void)
{
 findTable=new FindTableWidget();

 progress=new QProgressDialog(this);
 progress->hide();
}


void FindScreen::assembly(void)
{
 centralLayout=new QVBoxLayout();
 centralLayout->addLayout(toolsLine);
 centralLayout->addLayout(whereFindLine);
 centralLayout->addWidget(findTable);
 centralLayout->setContentsMargins(0,0,0,0); // Границы убираются
 centralLayout->setSizeConstraint(QLayout::SetNoConstraint);
 
 this->setLayout(centralLayout);
}


void FindScreen::enableFindButton(const QString &text)
{
 findStartButton->setEnabled(!text.isEmpty());
}


// Слот, с помощью которого другие виджеты могут устанавливать
// текст для поиска
void FindScreen::setFindText(QString text)
{
 findText->setText(text);

 emit textChangedFromAnother(text);
 emit findClickedAfterAnotherTextChanged();
}


// Слот, срабатывающий при нажатии на кнопку начала поиска
void FindScreen::findClicked(void)
{
 // Поля, где нужно искать (Заголовок, текст, теги...)
 searchArea["name"]  =findInName->isChecked();
 searchArea["author"]=findInAuthor->isChecked();
 searchArea["url"]   =findInUrl->isChecked();
 searchArea["tags"]  =findInTags->isChecked();
 searchArea["text"]  =findInText->isChecked();
 
 // Проверяется, установлено ли хоть одно поле для поиска
 int findEnableFlag=0;
 foreach (bool value, searchArea)
  if(value==true)findEnableFlag=1;

 // Если не отмечены поля для поиска
 if(findEnableFlag==0)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Cannot start find process"));
   messageBox.setText(tr("Verify that you selected fields for search for starting find process."));
   messageBox.addButton(tr("OK"),QMessageBox::AcceptRole);
   messageBox.exec();
   return; 
  }

 // Выясняется список слов, которые нужно искать
 searchWordList=text_delimiter_decompose(findText->text());

 if(searchWordList.size()==0)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Can not start find process"));
   messageBox.setText(tr("The search request is too short. Enter at least one word."));
   messageBox.addButton(tr("OK"),QMessageBox::AcceptRole);
   messageBox.exec();
   return; 
  }

 findStart();
}


void FindScreen::findStart(void)
{
 // Сохраняется текущая редактируемая запись, чтобы и в ней
 // были найдены введенные перед нажатием Find данные, если они есть
 find_object<MainWindow>("mainwindow")->saveTextarea();

 // Очищается таблица результата поиска
 findTable->clearAll();
 
 // Выясняется ссылка на модель дерева данных
 KnowTreeModel *searchModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("KnowTreeView")->model());
 
 
 // Выясняется стартовый элемент в дереве, с которого будет начат поиск
 // Выясняется сколько всего конечных записей
 TreeItem *startItem=0;
 int totalRec=0;
 if(mytetraconfig.getFindScreenTreeSearchArea()==0) // Если нужен поиск во всем дереве
 {
  // Корневой элемент дерева
  startItem=searchModel->rootItem;
  
  // Количество элементов (веток) во всем дереве
  totalRec=searchModel->getAllRecordCount();
 }
 else if (mytetraconfig.getFindScreenTreeSearchArea()==1) // Если нужен поиск в текущей ветке
 {
  // Индекс текущей выбранной ветки
  QModelIndex currentItemIndex=find_object<TreeScreen>("TreeScreen")->getCurrentItemIndex();

  // Текущая ветка
  startItem=searchModel->getItem(currentItemIndex);

  // Количество элементов (веток) в текущей ветке и всех подветках
  totalRec=searchModel->getRecordCountForItem(startItem);
 }

 qDebug() << "Start finding in " << totalRec << " records";
 
 
 // Если стартовый элемент не был установлен
 if(startItem==0)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Cannot start find process"));
   messageBox.setText(tr("Cant set start search element in tree."));
   messageBox.addButton(tr("OK"),QMessageBox::AcceptRole);
   messageBox.exec();
   return; 
  }
 
 
 // Показывается виджет линейки наполняемости
 progress->reset();
 progress->setLabelText(tr("Search..."));
 progress->setRange(0,totalRec);
 progress->setModal(true);
 progress->setMinimumDuration(0);
 progress->show();
 
 // Обнуляется счетчик обработанных конечных записей
 totalProgressCounter=0;
 cancelFlag=0;
 
 //Вызывается рекурсивный поиск в дереве
 findRecurse( startItem ); 
 
 // После вставки всех данных подгоняется ширина колонок
 findTable->updateColumnsWidth();

 // Виджет линейки наполняемости скрывается
 progress->hide();
}


void FindScreen::findRecurse(TreeItem *curritem)
{
 // Если была нажата отмена поиска
 if(cancelFlag==1)return;

 // Если ветка зашифрована, и пароль не был введен
 if(curritem->getField("crypt")=="1" &&
    globalParameters.getCryptKey().length()==0)
  return;
 
 // Если в ветке присутсвует таблица конечных записей
 if(curritem->recordtableGetRowCount() > 0)
  {
   // Обработка таблицы конечных записей
  
   // Выясняется ссылка на таблицу конечных записей
   RecordTableData *searchRecordTable=curritem->recordtableGetTableData();

   // Перебираются записи таблицы
   for(int i=0;i<searchRecordTable->size();i++)
    {
     // Обновляется линейка наполняемости
     progress->setValue(++totalProgressCounter);
     qApp->processEvents();
     if(progress->wasCanceled())
      {
       cancelFlag=1;
       return;
      }
    
     // Результаты поиска в полях
     QMap<QString, bool> iteration_search_result; 

     iteration_search_result["name"]  =false;
     iteration_search_result["author"]=false;
     iteration_search_result["url"]   =false;
     iteration_search_result["tags"]  =false;
     iteration_search_result["text"]  =false;

     // Текст в котором будет проводиться поиск
     QString inspectText;

     // Цикл поиска в отмеченных пользователем полях
     QMapIterator<QString, bool> j(iteration_search_result);
     while(j.hasNext()) 
      {
       j.next();
       QString key=j.key();

       // Если в данном поле нужно проводить поиск
       if(searchArea[key]==true)
        {
         if(key!="text")
          {
           // Поиск в обычном поле
           inspectText=searchRecordTable->getField(key,i);
           iteration_search_result[key]=findInTextProcess(inspectText);
          }
         else
          {
           // Поиск в тексте записи 
           inspectText=searchRecordTable->getText(i);
           QTextDocument textdoc;
           textdoc.setHtml(inspectText);
           iteration_search_result[key]=findInTextProcess(textdoc.toPlainText());
          }
        } 
      } // Закрылся цикл поиска в полях 
     
    
     // Проверяется, есть ли поле, в котором поиск был успешен
     int findFlag=0;
     foreach (bool value, iteration_search_result)
      if(value==true)findFlag=1;
     
     // Если запись найдена
     if(findFlag==1)
      {
       qDebug() << "Find succesfull in " << searchRecordTable->getField("name",i);
    
       // В таблицу результатов поиска добавляются данные
       // Имя записи
       // Имя ветки
       // Теги
       // Путь к ветке
       // Номер записи в таблице конечных записей
       findTable->addRow(searchRecordTable->getField("name",i),
                          curritem->getField("name"),
                          searchRecordTable->getField("tags",i),
                          curritem->getPath(),
                          i);
      }
     
    } // Закрылся цикл перебора записей в таблице конечных записей
  } // Закрылось условие что в ветке есть таблица конечных записей
 

 // Рекурсивная обработка каждой подчиненной ветки
 for(int i=0;i<curritem->childCount();i++) findRecurse(curritem->child(i));

}


// Поиск в переданном тексте
// Учитываются состояния переключателей wordregard и howextract
bool FindScreen::findInTextProcess(const QString& text)
{
 int findWordCount=0;
 int findFlag=0;
 
 // Перебираются искомые слова
 for(int i=0; i< searchWordList.size(); ++i)
  {
   findFlag=0;

   // Если надо найти совпадение целого слова
   if(howExtract->currentIndex()==0)
    {
     // Текст разбивается на слова с очисткой от лишних знаков 
     // и проверяется, есть ли в полученном списке текущее слово
     if(text_delimiter_decompose(text).contains(searchWordList.at(i), Qt::CaseInsensitive))
      findFlag=1;
    }
   else
    { 
     // Если надо найти слово как подстроку
     if(text.contains(searchWordList.at(i), Qt::CaseInsensitive))
      findFlag=1;
    }
   
   // Если слово было найдено, количество найденных слов увеличивается
   if(findFlag==1)findWordCount++;
   
   // Если ищется хотя бы одно совпадение
   if(findFlag==1 && wordRegard->currentIndex()==0)
    return true; // То при первом же совпадении цикл прекращается
  }    

 // Искалось хотябы одно совпадение, но небыло найдено
 if(wordRegard->currentIndex()==0) return false;
 else
  {
   // Иначе требовалось найти все слова в запросе
   if( findWordCount==searchWordList.size() )
    return true;
   else 
    return false;
  }  
 
}


void FindScreen::changedWordRegard(int pos)
{
 mytetraconfig.set_findscreen_wordregard(pos);
}


void FindScreen::changedHowExtract(int pos)
{
 mytetraconfig.set_findscreen_howextract(pos);
}


void FindScreen::changedTreeSearchArea(int pos)
{
 mytetraconfig.setFindScreenTreeSearchArea(pos);
}


void FindScreen::changedFindInName(int state)
{
 changedFindInField("name",state);
}


void FindScreen::changedFindInAuthor(int state)
{
 changedFindInField("author",state);
}


void FindScreen::changedFindInUrl(int state)
{
 changedFindInField("url",state);
}


void FindScreen::changedFindInTags(int state)
{
 changedFindInField("tags",state);
}


void FindScreen::changedFindInText(int state)
{
 changedFindInField("text",state);
}


void FindScreen::changedFindInField(QString fieldname, int state)
{
 bool i;
 if(state==Qt::Checked) i=true;
 else i=false;
  
 mytetraconfig.set_findscreen_find_in_field(fieldname,i);
}
 

void FindScreen::widgetShow(void)
{
 mytetraconfig.set_findscreen_show(true);
 this->show();

 // При появлении виджета курсор должен сразу стоять на поле ввода
 findText->setFocus();
}


void FindScreen::widgetHide(void)
{
 // Запоминается размер сплиттера перед скрытием виджета
 QSplitter *findSplitterRel=find_object<QSplitter>("findsplitter");
 mytetraconfig.set_findsplitter_size_list(findSplitterRel->sizes());
 
 // Виджет скрывается
 mytetraconfig.set_findscreen_show(false);
 this->close();
}
