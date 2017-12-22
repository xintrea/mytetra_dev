#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QGridLayout>
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

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


FindScreen::FindScreen(QWidget *parent) : QWidget(parent)
{
  setupFindTextAndButton();
  assemblyFindTextAndButton();

  setupCloseButton();
  assemblyCloseButton();

  setupComboOption();
  assemblyComboOption();

  setupWhereFindLine();
  assemblyWhereFindLine();

  setupUI();
  assembly();

  setupSignals();
}


FindScreen::~FindScreen(void)
{

}


// Текст поиска и кнопка "Поиск"
void FindScreen::setupFindTextAndButton(void)
{
  // Поле текста для поиска
  findText=new QLineEdit();

  // Кнопка "Поиск"
  findStartButton=new QPushButton(this);
  findStartButton->setText(tr("Find"));
  findStartButton->setDefault(true);
  findStartButton->setEnabled(false);

  // Кнопка разворачивания инструментов
  toolsExpand=new QToolButton(this);
  toolsExpand->setIcon(QIcon(":/resource/pic/find_in_base_expand_tools.svg"));
  toolsExpand->setEnabled(true);
}


// Текст поиска и кнопка "Поиск"
void FindScreen::assemblyFindTextAndButton(void)
{
  toolsAreaFindTextAndButton=new QHBoxLayout();
  toolsAreaFindTextAndButton->addWidget(findText);
  toolsAreaFindTextAndButton->addWidget(findStartButton);
  toolsAreaFindTextAndButton->addWidget(toolsExpand);
}


// Набор опций поиска в виде выпадающих списков
void FindScreen::setupComboOption(void)
{
  // Выбор "Любое слово" - "Все слова"
  wordRegard=new MtComboBox();
  wordRegard->addItem(QIcon(":/resource/pic/find_in_base_any.svg"), tr("Any word"));
  wordRegard->addItem(QIcon(":/resource/pic/find_in_base_all.svg"), tr("All words"));
  wordRegard->setCurrentIndex(mytetraConfig.get_findscreen_wordregard());

  // Выбор "Только целые слова" - "Подстрока"
  howExtract=new MtComboBox();
  howExtract->addItem(QIcon(":/resource/pic/find_in_base_separate.svg"), tr("Whole words"));
  howExtract->addItem(QIcon(":/resource/pic/find_in_base_substring.svg"), tr("Substring"));
  howExtract->setCurrentIndex(mytetraConfig.get_findscreen_howextract());

  // Выбор "Во всей базе" - "В текущей ветке"
  treeSearchArea=new MtComboBox();
  treeSearchArea->addItem(QIcon(":/resource/pic/find_in_base_search_all.svg"), tr("Entire base")); // Вся база
  treeSearchArea->addItem(QIcon(":/resource/pic/find_in_base_search_branch.svg"), tr("Current tree item")); // Текущая ветка
  treeSearchArea->setCurrentIndex(mytetraConfig.getFindScreenTreeSearchArea());

  if(mytetraConfig.getInterfaceMode()=="mobile")
  {
    // wordRegard->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    // howExtract->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    // treeSearchArea->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    // wordRegard->showMinimized();
    // howExtract->showMinimized();
    // treeSearchArea->showMinimized();

    wordRegard->setMinimumContentsLength(1);
    wordRegard->setMaximumWidth(getCalculateIconSizePx()*2);
    wordRegard->setMinimumWidth(getCalculateIconSizePx()*2);
    wordRegard->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    howExtract->setMinimumContentsLength(1);
    howExtract->setMaximumWidth(getCalculateIconSizePx()*2);
    howExtract->setMinimumWidth(getCalculateIconSizePx()*2);
    howExtract->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    treeSearchArea->setMinimumContentsLength(1);
    treeSearchArea->setMaximumWidth(getCalculateIconSizePx()*2);
    treeSearchArea->setMinimumWidth(getCalculateIconSizePx()*2);
    treeSearchArea->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
  }
}


// Набор опций поиска в виде выпадающих списков
void FindScreen::assemblyComboOption(void)
{
  toolsAreaComboOption=new QHBoxLayout();
  toolsAreaComboOption->addWidget(wordRegard);
  toolsAreaComboOption->addWidget(howExtract);
  toolsAreaComboOption->addWidget(treeSearchArea);
  toolsAreaComboOption->addStretch();
}


void FindScreen::setupCloseButton(void)
{
  // Кнопка закрытия виджета
  closeButton=new QToolButton(this);
  closeButton->setVisible(true);
  closeButton->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton)); // SP_TitleBarCloseButton SP_DialogCloseButton

  if(mytetraConfig.getInterfaceMode()=="desktop")
  {
    int w=closeButton->geometry().width();
    int h=closeButton->geometry().height();
    int x=qMin(w,h)/2;
    closeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
    closeButton->setMinimumSize(x,x);
    closeButton->setMaximumSize(x,x);
    closeButton->resize(x,x);
  }
}


void FindScreen::assemblyCloseButton(void)
{
  // Вертикальная область с кнопкой закрытия и распоркой
  // чтобы кнопка была вверху
  toolsAreaCloseButton=new QVBoxLayout();
  toolsAreaCloseButton->setContentsMargins(0,0,0,0);
  toolsAreaCloseButton->addWidget(closeButton);
  toolsAreaCloseButton->addStretch();
}


void FindScreen::setupWhereFindLine(void)
{
  whereFindLabel=new QLabel(tr("Find in: "));

  findInName=new QCheckBox(tr("Title"));
  findInName->setChecked(mytetraConfig.get_findscreen_find_in_field("name"));

  findInAuthor=new QCheckBox(tr("Author(s)"));
  findInAuthor->setChecked(mytetraConfig.get_findscreen_find_in_field("author"));

  findInUrl=new QCheckBox(tr("Url"));
  findInUrl->setChecked(mytetraConfig.get_findscreen_find_in_field("url"));

  findInTags=new QCheckBox(tr("Tags"));
  findInTags->setChecked(mytetraConfig.get_findscreen_find_in_field("tags"));

  findInText=new QCheckBox(tr("Text"));
  findInText->setChecked(mytetraConfig.get_findscreen_find_in_field("text"));

  findInNameItem=new QCheckBox(tr("Name tree item"));
  findInNameItem->setChecked(mytetraConfig.get_findscreen_find_in_field("nameItem"));
}


void FindScreen::assemblyWhereFindLine(void)
{
  whereFindLine=new QHBoxLayout();

  if(mytetraConfig.getInterfaceMode()=="desktop")
    whereFindLine->addWidget(whereFindLabel);

  if(mytetraConfig.getInterfaceMode()=="mobile")
    whereFindLabel->hide();

  whereFindLine->addWidget(findInName);
  whereFindLine->addWidget(findInAuthor);
  whereFindLine->addWidget(findInUrl);
  whereFindLine->addWidget(findInTags);
  whereFindLine->addWidget(findInText);
  whereFindLine->addWidget(findInNameItem);

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

  // При нажатии кнопки разворачивания/сворачивания инструментов поиска
  connect(toolsExpand,SIGNAL(clicked()),
          this,SLOT(toolsExpandClicked()));

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

  connect(findInNameItem,SIGNAL(stateChanged(int)),
          this,SLOT(changedFindInNameItem(int)));
}


void FindScreen::setupUI(void)
{
  findTable=new FindTableWidget();

  progress=new QProgressDialog(this);

  // Корректировка регрессии в Qt 5.5 - 5.6.x QTBUG-47042 QTBUG-47049
  // QProgressDialog показывает сам себя через 4 секунды (точнее, спустя minimumDuration() сек.) после отработки конструктора
  // метод hide() не может скрыть такое окно
  progress->cancel();
}


void FindScreen::assembly(void)
{
  centralDesktopLayout=new QVBoxLayout();

  if(mytetraConfig.getInterfaceMode()=="desktop")
  {
    toolsLine=new QHBoxLayout();
    toolsLine->addLayout(toolsAreaFindTextAndButton);
    toolsLine->addLayout(toolsAreaComboOption);
    toolsLine->addLayout(toolsAreaCloseButton);

    centralDesktopLayout->addLayout(toolsLine);
  }

  if(mytetraConfig.getInterfaceMode()=="mobile")
  {
    toolsGrid=new QGridLayout();
    toolsGrid->addLayout(toolsAreaFindTextAndButton, 0, 0);
    toolsGrid->addLayout(toolsAreaCloseButton,       0, 1);
    toolsGrid->addLayout(toolsAreaComboOption,       1, 0);

    centralDesktopLayout->addLayout(toolsGrid);
  }

  centralDesktopLayout->addLayout(whereFindLine);
  centralDesktopLayout->addWidget(findTable, 10);
  centralDesktopLayout->setContentsMargins(0,0,0,0); // Границы убираются
  centralDesktopLayout->setSizeConstraint(QLayout::SetNoConstraint);

  // whereFindLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  // findTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


  this->setLayout(centralDesktopLayout);

  switchToolsExpand(mytetraConfig.getFindInBaseExpand());
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
  searchArea["nameItem"]  =findInNameItem->isChecked();

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
  searchWordList=textDelimiterDecompose(findText->text());

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

  // Очищается надпись поверх результата поиска
  findTable->setOverdrawMessage("");

  // Выясняется ссылка на модель дерева данных
  KnowTreeModel *searchModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());


  // Выясняется стартовый элемент в дереве, с которого будет начат поиск
  // Выясняется сколько всего конечных записей
  TreeItem *startItem=0;
  int totalRec=0;
  if(mytetraConfig.getFindScreenTreeSearchArea()==0) // Если нужен поиск во всем дереве
  {
    // Корневой элемент дерева
    startItem=searchModel->rootItem;

    // Количество элементов (веток) во всем дереве
    totalRec=searchModel->getAllRecordCount();
  }
  else if (mytetraConfig.getFindScreenTreeSearchArea()==1) // Если нужен поиск в текущей ветке
  {
    // Индекс текущей выбранной ветки
    QModelIndex currentItemIndex=find_object<TreeScreen>("treeScreen")->getCurrentItemIndex();

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
    messageBox.setText(tr("Starting position for a tree searching is not set."));
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
  isUnsearchCryptBranchPresent=false;

  //Вызывается рекурсивный поиск в дереве
  findRecurse( startItem );

  // После вставки всех данных подгоняется ширина колонок
  findTable->updateColumnsWidth();

  // Виджет линейки наполняемости скрывается
  progress->hide();


  // Если ничего небыло найдено
  if(findTable->getRowCount()==0)
  {
    // Если были непросмотренные ветки по причине того, что они зашифрованны, а пароль не введен
    if(isUnsearchCryptBranchPresent)
      findTable->setOverdrawMessage(tr("No search results.\nHint: encrypted items has been detected. For searching in encrypted items you have to enter the password.")); // Ничего не найдено. При поиске обнаружены зашифрованные ветки. Однако пароль не был введен, поэтому поиск в них не проводился

    // Иначе если поиск проводился не во всей базе
    else if(mytetraConfig.getFindScreenTreeSearchArea()!=0)
      findTable->setOverdrawMessage(tr("No search results.\nHint: search produced in current tree item.\nTry to search for entire database.")); // Ничего не найдено. Поиск производился внутри одной ветки. Попробуйте искать во всей базе.
  }

}


void FindScreen::findRecurse(TreeItem *curritem)
{
  // Если была нажата отмена поиска
  if(cancelFlag==1)return;

  // Если ветка зашифрована, и пароль не был введен
  if(curritem->getField("crypt")=="1" &&
     globalParameters.getCryptKey().length()==0)
  {
    isUnsearchCryptBranchPresent=true;
    return;
  }

  if(searchArea["nameItem"]==true)
  {
    // Поиск в имени ветви
    QString itemName = curritem->getField("name");
    bool findItem = findInTextProcess(itemName);
    if(findItem)
    {
      QString path = curritem->getPathAsNameWithDelimeter(" ");
      qDebug() << "Find branch succesfull " << path;
      // В таблицу результатов добавляется запись о найденой ветке
      findTable->addRow(path,
                        itemName,
                        "",
                        curritem->getPath(),
                        curritem->getField("id"));
    }
  }

  // Если в ветке присутсвует таблица конечных записей
  if(curritem->recordtableGetRowCount() > 0)
  {
    // Обработка таблицы конечных записей

    // Выясняется ссылка на таблицу конечных записей
    RecordTableData *searchRecordTable=curritem->recordtableGetTableData();

    // Перебираются записи таблицы
    for(unsigned int i=0; i<searchRecordTable->size(); i++)
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
        // ID записи в таблице конечных записей
        findTable->addRow(searchRecordTable->getField("name", i),
                          curritem->getField("name"),
                          searchRecordTable->getField("tags", i),
                          curritem->getPath(),
                          searchRecordTable->getField("id", i));
      }

    } // Закрылся цикл перебора записей в таблице конечных записей
  } // Закрылось условие что в ветке есть таблица конечных записей


  // Рекурсивная обработка каждой подчиненной ветки
  for(unsigned int i=0; i<curritem->childCount(); i++)
    findRecurse(curritem->child(i));

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
      if(textDelimiterDecompose(text).contains(searchWordList.at(i), Qt::CaseInsensitive))
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
  mytetraConfig.set_findscreen_wordregard(pos);
}


void FindScreen::changedHowExtract(int pos)
{
  mytetraConfig.set_findscreen_howextract(pos);
}


void FindScreen::changedTreeSearchArea(int pos)
{
  mytetraConfig.setFindScreenTreeSearchArea(pos);
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

void FindScreen::changedFindInNameItem(int state)
{
  changedFindInField("nameItem",state);
}


void FindScreen::changedFindInField(QString fieldname, int state)
{
  bool i;
  if(state==Qt::Checked) i=true;
  else i=false;
  
  mytetraConfig.set_findscreen_find_in_field(fieldname,i);
}
 

void FindScreen::widgetShow(void)
{
  mytetraConfig.set_findscreen_show(true);
  this->show();

  // При появлении виджета курсор должен сразу стоять на поле ввода
  findText->setFocus();
}


// Полное сокрытие виджета
void FindScreen::widgetHide(void)
{
  // Запоминается размер сплиттера перед скрытием виджета
  QSplitter *findSplitterRel=find_object<QSplitter>("findsplitter");
  mytetraConfig.set_findsplitter_size_list(findSplitterRel->sizes());

  // Виджет скрывается
  mytetraConfig.set_findscreen_show(false);
  this->close();
}


// Слот, срабатывающий при клике на кнопку expand
void FindScreen::toolsExpandClicked(void)
{
  // Если нужно сомкнуть инструменты
  if(findInName->isVisible())
  {
    switchToolsExpand(false);
    mytetraConfig.setFindInBaseExpand(false);
  }
  else
  {
    switchToolsExpand(true);
    mytetraConfig.setFindInBaseExpand(true);
  }
}


void FindScreen::switchToolsExpand(bool flag)
{
  // toolsAreaComboOption->setVisible(flag);
  // whereFindLine->setVisible(flag);

  // Выпадающие списки скрываются в мобильном интерфейсе, так как они на отдельной строке
  if(mytetraConfig.getInterfaceMode()=="mobile")
  {
    wordRegard->setVisible(flag);
    howExtract->setVisible(flag);
    treeSearchArea->setVisible(flag);
  }

  // Надпись Find in видна и управляется только в desktop режиме интерфейса
  if(mytetraConfig.getInterfaceMode()=="desktop")
    whereFindLabel->setVisible(flag);

  // Флаги поиска скрываются для любого интерфейса, так как они всегда находятся на отдельной строке
  findInName->setVisible(flag);
  findInAuthor->setVisible(flag);
  findInUrl->setVisible(flag);
  findInTags->setVisible(flag);
  findInText->setVisible(flag);
  findInNameItem->setVisible(flag);
}

// Устаревшая функция, простое обнаружение токенов для поиска
/*
QStringList FindScreen::textDelimiterDecompose(QString text)
{
 text.replace('"',' ');
 text.replace("'"," ");
 text.replace('.',' ');
 text.replace(',',' ');
 text.replace(';',' ');
 text.replace(':',' ');
 text.replace('-',' ');
 text.replace('?',' ');
 text.replace('!',' ');

 QStringList list = text.split(QRegExp("\\W+"), QString::SkipEmptyParts);

 return list;
}
*/


QStringList FindScreen::textDelimiterDecompose(QString text)
{
  const int len=text.length();

  QStringList list;
  QString buf;
  bool quoted=false;

  QString delimiter=" '.,;:-?!`";
  delimiter.append(QChar::Tabulation);
  delimiter.append(QChar::Nbsp);

  for(int i=0; i<len; i++)
  {
    // Если обнаружен разделитель
    if(delimiter.contains( text[i] ))
    {
      if( !quoted )
      {
        if(buf.length() > 0)
        {
          list.append(buf);
          buf = "";
        }
      }
      else
        buf += text[i];
    }
    else if(text[i] == '"')
      quoted = !quoted;
    else
      buf += text[i];
  }

  if(buf.length() > 0)
    list.append(buf);

  qDebug() << "Find split list:" << list;

  return list;
}
