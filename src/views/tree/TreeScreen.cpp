#include <QAbstractItemView>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QString>
#include <QMap>
#include <QAction>
#include <QItemSelectionModel>

#include "main.h"
#include "TreeScreen.h"
#include "KnowTreeView.h"

#include "models/recordTable/RecordTableData.h"
#include "views/recordTable/RecordTableView.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "models/tree/TreeItem.h"
#include "models/tree/KnowTreeModel.h"
#include "libraries/ClipboardBranch.h"
#include "views/record/MetaEditor.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/Password.h"
#include "libraries/WindowSwitcher.h"


extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


TreeScreen::TreeScreen(QWidget *parent) : QWidget(parent)
{
 setupActions();
 setupUI();
 setupModels();
 setupSignals();
 assembly();
}


TreeScreen::~TreeScreen()
{

}


void TreeScreen::setupActions(void)
{
 QAction *ac;

 // Разворачивание всех подветок
 // a->setShortcut(tr("Ctrl+X"));
 ac=new QAction(tr("Expand all sub items"), this);
 ac->setStatusTip(tr("Expand all sub items"));
 ac->setIcon(QIcon(":/resource/pic/expand_all_subbranch.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(expand_all_subbranch()));
 actionList["expandAllSubbranch"]=ac;

 // Сворачивание всех подветок
 ac = new QAction(tr("Collapse all sub items"), this);
 ac->setStatusTip(tr("Collapse all sub items"));
 ac->setIcon(QIcon(":/resource/pic/collapse_all_subbranch.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(collapse_all_subbranch()));
 actionList["collapseAllSubbranch"]=ac;

 // Перемещение ветки вверх
 ac = new QAction(tr("Move item up"), this);
 ac->setStatusTip(tr("Move item up"));
 ac->setIcon(QIcon(":/resource/pic/move_up.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(move_up_branch()));
 actionList["moveUpBranch"]=ac;

 // Перемещение ветки вниз
 ac = new QAction(tr("Move item down"), this);
 ac->setStatusTip(tr("Move item down"));
 ac->setIcon(QIcon(":/resource/pic/move_dn.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(move_dn_branch()));
 actionList["moveDnBranch"]=ac;

 // Вставка новой подветки
 ac = new QAction(tr("Insert a new sub item"), this);
 ac->setStatusTip(tr("Insert a new sub item into selected"));
 ac->setIcon(QIcon(":/resource/pic/add_subbranch.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(ins_subbranch()));
 actionList["insSubbranch"]=ac;

 // Вставка новой ветки
 ac = new QAction(tr("Insert a new sibling item"), this);
 ac->setStatusTip(tr("Insert a new sibling item after selected"));
 ac->setIcon(QIcon(":/resource/pic/add_branch.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(ins_branch()));
 actionList["insBranch"]=ac;

 // Редактирование ветки
 ac = new QAction(tr("Edit item name"), this);
 ac->setStatusTip(tr("Edit name of selected item"));
 ac->setIcon(QIcon(":/resource/pic/note_edit.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(edit_branch()));
 actionList["editBranch"]=ac;

 // Удаление ветки
 ac = new QAction(tr("Delete item"), this);
 ac->setStatusTip(tr("Delete selected item and all sub items"));
 ac->setIcon(QIcon(":/resource/pic/note_delete.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(del_branch()));
 actionList["delBranch"]=ac;

 // Удаление ветки с сохранением копии в буфер обмена
 ac = new QAction(tr("Cut item"), this);
 ac->setStatusTip(tr("Cut item including sub items"));
 ac->setIcon(QIcon(":/resource/pic/branch_cut.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(cut_branch()));
 actionList["cutBranch"]=ac;

 // Копирование ветки в буфер обмена
 ac = new QAction(tr("Copy item"), this);
 ac->setStatusTip(tr("Copy item including sub items"));
 ac->setIcon(QIcon(":/resource/pic/branch_copy.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(copy_branch()));
 actionList["copyBranch"]=ac;

 // Вставка ветки из буфера обмена
 ac = new QAction(tr("Paste item"), this);
 ac->setStatusTip(tr("Paste sibling item after selected"));
 ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(paste_branch()));
 actionList["pasteBranch"]=ac;

 // Вставка ветки из буфера обмена в виде подветки
 ac = new QAction(tr("Paste as sub item"), this);
 ac->setStatusTip(tr("Paste item as sub item for selected"));
 ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(paste_subbranch()));
 actionList["pasteSubbranch"]=ac;

 // Шифрование ветки
 ac = new QAction(tr("Encrypt item"), this);
 ac->setStatusTip(tr("Encrypt item and all subitem"));
 // actionEncryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(encrypt_branch()));
 actionList["encryptBranch"]=ac;

 // Расшифровка ветки (снятие пароля)
 ac = new QAction(tr("Decrypt item"), this);
 ac->setStatusTip(tr("Decrypt item and all subitem"));
 // actionDecryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
 connect(ac, SIGNAL(triggered()), this, SLOT(decrypt_branch()));
 actionList["decryptBranch"]=ac;
}


void TreeScreen::setupUI(void)
{
 toolsLine=new QToolBar(this);

 QSize tool_bar_icon_size(16,16);
 toolsLine->setIconSize(tool_bar_icon_size);

 insert_action_as_button(toolsLine, actionList["insSubbranch"]);
 insert_action_as_button(toolsLine, actionList["insBranch"]);
 insert_action_as_button(toolsLine, actionList["editBranch"]);
 insert_action_as_button(toolsLine, actionList["delBranch"]);

 toolsLine->addSeparator();

 insert_action_as_button(toolsLine, actionList["expandAllSubbranch"]);
 insert_action_as_button(toolsLine, actionList["collapseAllSubbranch"]);

 toolsLine->addSeparator();

 insert_action_as_button(toolsLine, actionList["moveUpBranch"]);
 insert_action_as_button(toolsLine, actionList["moveDnBranch"]);

 knowTreeView=new KnowTreeView(this);
 knowTreeView->setObjectName("KnowTreeView");
 knowTreeView->setMinimumSize(150,250);
 knowTreeView->setWordWrap(true);

 // Временно сделан одинарный режим выбора пунктов
 // todo: Множественный режим надо выставить тогда, когда
 // станет ясно, как удалять несколько произвольных веток так, чтобы
 // в процессе удаления QModelIndex нижестоящих еще не удаленных
 // веток не менялся
 // knowTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
 knowTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

 knowTreeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
 
 // Нужно установить правила показа контекстного самодельного меню
 // чтобы оно могло вызываться
 knowTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

 // Представление не должно позволять редактировать элементы обычным путем
 knowTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void TreeScreen::setupModels(void)
{
 // Создание и первичная настройка модели
 knowTreeModel = new KnowTreeModel(this);

 // Установка заголовка
 // QStringList headers;
 // headers << tr("Info groups");
 // knowTreeModel->setHeaders(headers);

 // Загрузка данных
 knowTreeModel->initFromXML( mytetraConfig.get_tetradir()+"/mytetra.xml" );

 // Модель подключется к виду
 knowTreeView->setModel(knowTreeModel);
}


// Открытие контекстного меню в дереве разделов
void TreeScreen::on_customContextMenuRequested(const QPoint &pos)
{
  qDebug() << "In TreeScreen::on_customContextMenuRequested";

  // Конструирование меню
  QMenu menu(this);
  menu.addAction(actionList["insSubbranch"]);
  menu.addAction(actionList["insBranch"]);
  menu.addAction(actionList["editBranch"]);
  menu.addAction(actionList["delBranch"]);
  menu.addSeparator();
  menu.addAction(actionList["expandAllSubbranch"]);
  menu.addAction(actionList["collapseAllSubbranch"]);
  menu.addSeparator();
  menu.addAction(actionList["moveUpBranch"]);
  menu.addAction(actionList["moveDnBranch"]);
  menu.addSeparator();
  menu.addAction(actionList["cutBranch"]);
  menu.addAction(actionList["copyBranch"]);
  menu.addAction(actionList["pasteBranch"]);
  menu.addAction(actionList["pasteSubbranch"]);
  menu.addSeparator();
  menu.addAction(actionList["encryptBranch"]);
  menu.addAction(actionList["decryptBranch"]);

  // Получение индекса выделенной ветки
  QModelIndex index=getCurrentItemIndex();

  // Выясняется, зашифрована ли ветка или нет
  QString cryptFlag=knowTreeModel->getItem(index)->getField("crypt");

  // Выясняется, зашифрована ли родительская ветка
  QString parentCryptFlag=knowTreeModel->getItem(index)->parent()->getField("crypt");

  // Если ветка не зашифрована
  // Или ветка зашифрована, но пароль успешно введен
  if(cryptFlag!="1" ||
     (cryptFlag=="1" && globalParameters.getCryptKey().length()>0))
   {

    // Если в буфере есть ветки, соответсвующие пункты становятся активными
    bool isBranch=false;
    const QMimeData *mimeData=QApplication::clipboard()->mimeData();
     if(mimeData!=NULL)
      if(mimeData->hasFormat("mytetra/branch"))
        isBranch=true;

     if( isBranch )
     {
      actionList["pasteBranch"]->setEnabled(true);
      actionList["pasteSubbranch"]->setEnabled(true);
     }
    else
     {
      actionList["pasteBranch"]->setEnabled(false);
      actionList["pasteSubbranch"]->setEnabled(false);
     }


    // ----------------------------
    // Подсветка пунктов шифрования
    // ----------------------------

    // Если ветка незашифрована
    if(cryptFlag!="1")
     {
      // Шифровать можно
      // Дешифровать нельзя
      actionList["encryptBranch"]->setEnabled(true);
      actionList["decryptBranch"]->setEnabled(false);
     }
    else
     {
      // Ветка зашифрована

      // Шифровать нельзя
      actionList["encryptBranch"]->setEnabled(false);

      // Дешифровать можно только если верхнележащая ветка незашифрована
      if(parentCryptFlag!="1")
       actionList["decryptBranch"]->setEnabled(true);
      else
       actionList["decryptBranch"]->setEnabled(false);
     }
   }

  
  // Включение отображения меню на экране
  // menu.exec(event->globalPos());
  menu.exec(knowTreeView->viewport()->mapToGlobal(pos));
}


void TreeScreen::setupSignals(void)
{
 // Соединение сигнал-слот чтобы показать контекстное меню по правому клику на ветке
 connect(knowTreeView, SIGNAL(customContextMenuRequested(const QPoint &)),
         this,SLOT(on_customContextMenuRequested(const QPoint &)));

 // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
 connect(knowTreeView, SIGNAL(tapAndHoldGestureFinished(const QPoint &)),
         this, SLOT(on_customContextMenuRequested(const QPoint &)));

 // Соединение сигнал-слот что ветка выбрана мышкой или стрелками на клавиатуре
 connect(knowTreeView->selectionModel(), SIGNAL(currentRowChanged (const QModelIndex&, const QModelIndex&)),
         this, SLOT(on_knowtree_clicked(const QModelIndex&)));
 
 // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
 // connect(knowTreeView, SIGNAL(doubleClicked(const QModelIndex &)),
 //         actionList["editBranch"], SLOT(trigger(void)));

 // Сигнал что ветка выбрана мышкой
 // connect(knowTreeView,SIGNAL(pressed(const QModelIndex &)),
 //         this,SLOT(on_knowTreeView_clicked(const QModelIndex &)));
 // connect(knowTreeView, SIGNAL(clicked(const QModelIndex &)),
 //         this, SLOT(on_knowTreeView_clicked(const QModelIndex &)));

}


void TreeScreen::assembly(void)
{
 treeScreenLayout=new QVBoxLayout();
 treeScreenLayout->setObjectName("treescreen_QVBoxLayout");

 treeScreenLayout->addWidget(toolsLine);
 treeScreenLayout->addWidget(knowTreeView);

 setLayout(treeScreenLayout);

 // Границы убираются, так как данный объект будет использоваться как виджет
 QLayout *lt;
 lt=layout();
 lt->setContentsMargins(0,2,0,0);
}


void TreeScreen::expand_all_subbranch(void)
{
 // Получение индексов выделенных строк
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();

 for(int i = 0; i < selectitems.size(); ++i) 
  expand_or_collapse_recurse(selectitems.at(i), true);
}


void TreeScreen::collapse_all_subbranch(void)
{
 // Получение индексов выделенных строк
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();

 for(int i = 0; i < selectitems.size(); ++i) 
  expand_or_collapse_recurse(selectitems.at(i), false);
}


void TreeScreen::expand_or_collapse_recurse(QModelIndex index, bool mode)
{
 knowTreeView->setExpanded(index, mode);

 int i=0;
 while( (index.child(i,0)).isValid() )  
  {
   expand_or_collapse_recurse(index.child(i,0), mode);
   i++;
  } 

}


void TreeScreen::move_up_branch(void)
{
 moveUpDnBranch(1);
}


void TreeScreen::move_dn_branch(void)
{
 moveUpDnBranch(-1);
}


void TreeScreen::moveUpDnBranch(int direction)
{
 // Если ветку нельзя перемещать
 if(!moveCheckEnable()) return;
 
 // Получение индекса выделенной строки
 QModelIndex index=getCurrentItemIndex();
 
 // Ветка перемещается
 QModelIndex index_after_move;
 if(direction==1) index_after_move=knowTreeModel->moveUpBranch(index);
 else             index_after_move=knowTreeModel->moveDnBranch(index);
  
 // Установка курсора на позицию, куда была перенесена ветка
 if(index_after_move.isValid())
  {
   knowTreeView->selectionModel()->setCurrentIndex(index_after_move,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
   knowTreeView->selectionModel()->select(index_after_move,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
  } 
 
 // Сохранение дерева веток
 find_object<TreeScreen>("TreeScreen")->saveKnowTree();
}


bool TreeScreen::moveCheckEnable(void)
{
 // Получение списка индексов QModelIndex выделенных элементов
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();
 
 // Если выбрано более одной ветки
 if(selectitems.size()>1)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Unavailable action"));
   messageBox.setText(tr("You've selected ")+QString::number(selectitems.size())+tr(" items.\nPlease select single item for moving."));
   messageBox.addButton(tr("OK"),QMessageBox::AcceptRole);
   messageBox.exec();
   return false; 
  }
 else
  return true;
}


void TreeScreen::ins_branch(void)
{
 qDebug() << "In ins_branch()";

 insBranchSmart(true);
}


void TreeScreen::ins_subbranch(void)
{
 qDebug() << "In ins_subbranch()";

 insBranchSmart(false);
}


// Вспомогательная функция, используется при добавлении ветки
// Если is_branch=true, то добавляется ветка на тот же уровень
// Если is_branch=false, то добавляется подветка
void TreeScreen::insBranchSmart(bool is_branch)
{
 // Получение списка индексов QModelIndex выделенных элементов
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();

 // Если выбрано более одной ветки
 if(selectitems.size()>1)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Unavailable action"));
   messageBox.setText(tr("You've selected ")+QString::number(selectitems.size())+tr(" items.\nPlease select single item for enabling insert operation."));
   messageBox.addButton(tr("OK"),QMessageBox::AcceptRole);
   messageBox.exec();
   return;
  }

 // Создается окно ввода данных
 bool ok;
 QString title, text;
 if(is_branch) {
  title=tr("Create new item");
  text=tr("Item name:");
 }
 else {
  title=tr("Create new sub item");
  text=tr("Sub item name:");
 }
 QString name = QInputDialog::getText(this, title, text, QLineEdit::Normal, "", &ok);
 if (!( ok && !name.isEmpty() )) return; // Если была нажата отмена

  // Получение индекса выделенной строки
 QModelIndex index=getCurrentItemIndex();

 // Введенные данные добавляются
 insBranchProcess(index, name, is_branch);
}


void TreeScreen::insBranchProcess(QModelIndex index, QString name, bool is_branch)
{
 // Получение ссылки на узел, который соответствует выделенной строке
 TreeItem *item=knowTreeModel->getItem(index);

 find_object<MainWindow>("mainwindow")->setDisabled(true);

 // Получение уникального идентификатора
 QString id=get_unical_id();


 // Вставка данных и установка курсора

 // Одноранговая ветка
 if(is_branch)
  {
   // Вставка новых данных в модель дерева записей
   knowTreeModel->addNewSiblingBranch(index, id, name);

   // Установка курсора на только что созданную позицию

   // Чтобы вычислить позицию, надо синхронно получить parent элемента,
   // на уровне которого должен был создасться новый элемент.
   // Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
   // Затем у объекта Item выяснить количество элементов, и установить
   // засветку через метод index() относительно parent в виде QModelIndex
   QModelIndex setto=knowTreeModel->index(item->parent()->childCount()-1,0,index.parent());
   knowTreeView->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
  }
 else
  {
   // Подветка

   // Вставка новых данных в модель дерева записей
   knowTreeModel->addNewChildBranch(index, id, name);

   // Установка курсора на только что созданную позицию
   QModelIndex setto=knowTreeModel->indexChildren(index,item->childCount()-1);
   knowTreeView->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

   // А можно было установить курсор на нужную позицию и так
   // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
   //                                             QItemSelectionModel::ClearAndSelect);
  }

 // Сохранение дерева веток
 find_object<TreeScreen>("TreeScreen")->saveKnowTree();

 find_object<MainWindow>("mainwindow")->setEnabled(true);
}


void TreeScreen::edit_branch(void)
{
 qDebug() << "In edit_branch()";

  // Получение списка индексов QModelIndex выделенных элементов
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();
 
 // Если выбрано более одной ветки
 if(selectitems.size()>1)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Unavailable action"));
   messageBox.setText(tr("You've selected ")+QString::number(selectitems.size())+tr(" items.\nPlease select single item for enabling edit operation."));
   messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
   messageBox.exec();
   return; 
  }
 
 // Получение индекса выделенной строки
 QModelIndex index=getCurrentItemIndex();
 
 // Получение ссылки на узел, который соответствует выделенной строке
 TreeItem *item=knowTreeModel->getItem(index);

 // Если ветка зашифрована и пароль не был введен
 if(item->getField("crypt")=="1" &&
    globalParameters.getCryptKey().length()==0)
  return;
 
 // Получение имени ветки
 QString name=item->getField("name");
  
 // Создается окно ввода данных
 bool ok;
 QString newname = QInputDialog::getText(this, 
                                      tr("Edit item name"),
                                      tr("Item name:"), 
                                      QLineEdit::Normal,
                                      name, 
                                      &ok);

 // Если была нажата отмена
 if (!( ok && !newname.isEmpty() )) return; 

 find_object<MainWindow>("mainwindow")->setDisabled(true);
 
 item->setField("name", newname);
 
 // Сохранение дерева веток
 find_object<TreeScreen>("TreeScreen")->saveKnowTree();
 
 find_object<MainWindow>("mainwindow")->setEnabled(true);
}


// Удаление выбранных веток, вызывается при выборе соотвествущей
// кнопки или пункта меню
void TreeScreen::del_branch(QString mode)
{
 qDebug() << "In del_branch()";

 // На время удаления блокируется главное окно
 find_object<MainWindow>("mainwindow")->setDisabled(true);
 find_object<MainWindow>("mainwindow")->blockSignals(true);


 // Получение списка индексов QModelIndex выделенных элементов
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();

 // Список имен веток, которые нужно удалить
 QStringList branches_name;
 for(int i = 0; i < selectitems.size(); ++i)
  {
   QModelIndex index=selectitems.at(i);
   TreeItem *item=knowTreeModel->getItem(index);
   branches_name << item->getField("name");
  }

 
 // Если системный пароль не установлен, зашифрованные ветки удалять нельзя
 if(globalParameters.getCryptKey().size()==0)
  {
   bool disableFlag=false;

   // Перебираются удаляемые ветки
   for(int i = 0; i < selectitems.size(); ++i)
    {
     QModelIndex index=selectitems.at(i);
     TreeItem *item=knowTreeModel->getItem(index);
     
     // Если у ветки установлен флаг шифрования
     if(item->getField("crypt")=="1")
      {
       disableFlag=true;
       break;
      }

     // Проверяется наличие флага шифрования у всех подветок
     QList<QStringList> cryptFlagsList=item->getAllChildrenPathAsField("crypt");
     foreach(QStringList cryptFlags, cryptFlagsList)
      if(cryptFlags.contains("1"))
       {
        disableFlag=true;
        break;
       }

     if(disableFlag)
      break;

    } // Закрылся цикл перебора всех выделенных для удаления веток


   // Если в какой-то ветке обнаружено шифрование
   if(disableFlag)
    {
     QMessageBox messageBox(this);
     messageBox.setWindowTitle(tr("Unavailable action"));
     messageBox.setText(tr("In your selected data found closed item. Action canceled."));
     messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
     messageBox.exec();

     // Разблокируется главное окно
     find_object<MainWindow>("mainwindow")->setEnabled(true);
     find_object<MainWindow>("mainwindow")->blockSignals(false);

     return;
    }

  } // Закрылось условие что системный пароль не установлен
 

 // Создается окно с вопросом, нужно удалять ветки или нет
 QString title, text, del_button;
 bool enable_question=true;
 if(mode=="delete")
  {
   title=tr("Delete item(s)");
   text=tr("Are you sure you wish to delete item(s) <b>") + branches_name.join(", ") + tr("</b> and all sub items?");
   del_button=tr("Delete");

   enable_question=true;
  }
 else if(mode=="cut")
  {
   title=tr("Cut item");
   text=tr("Are you sure you wish to cut item <b>") + branches_name.join(", ") + tr("</b> and all sub items?");
   del_button=tr("Cut");

   if(mytetraConfig.get_cutbranchconfirm()) enable_question=true;
   else enable_question=false;
  }

 bool enable_del=true;
 if(enable_question)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(title);
   messageBox.setText(text);
   messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
   QAbstractButton *deleteButton =messageBox.addButton(del_button, QMessageBox::AcceptRole);
   messageBox.exec();

   if(messageBox.clickedButton() == deleteButton) enable_del=true;
   else enable_del=false;
  }


 // Если удаление подтверждено
 if(enable_del)
  {
   // Сохраняется текст в окне редактирования
   // Нужно, чтобы нормально удалилась текущая редактируемая запись,
   // если она находится в удаляемой ветке
   find_object<MainWindow>("mainwindow")->saveTextarea();

   // Сортировка списка индексов по вложенности методом пузырька
   // Индексы с длинным путем перемещаются в начало списка
   for(int i = 0; i < selectitems.size(); i++)
    for(int j=selectitems.size()-1; j>i; j--)
     {
      QStringList path_1=(knowTreeModel->getItem(selectitems.at(j-1)))->getPath();
      QStringList path_2=(knowTreeModel->getItem(selectitems.at(j)))->getPath();
      if(path_1.size() < path_2.size())
       selectitems.swap(j-1, j);
     }
 
   qDebug() << "Path for delete";
   for(int i = 0; i < selectitems.size(); ++i)
    qDebug() << (knowTreeModel->getItem(selectitems.at(i)))->getPath();

   // Вызов удаления веток
   for(int i = 0; i < selectitems.size(); ++i)
    del_one_branch(selectitems.at(i));

   qDebug() << "Delete finish";

   // Сохранение дерева веток
   find_object<TreeScreen>("TreeScreen")->saveKnowTree();

   qDebug() << "Save new tree finish";
  }

 // Разблокируется главное окно
 find_object<MainWindow>("mainwindow")->setEnabled(true);
 find_object<MainWindow>("mainwindow")->blockSignals(false);

 treeEmptyControl();
 treeCryptControl();
}


// Удаление одной ветки и её подветок
void TreeScreen::del_one_branch(QModelIndex index)
{

 if(!index.isValid()) return;

 qDebug() << "Label 1";

 // Получение узла, который соответствует обрабатываемому индексу
 TreeItem *item=knowTreeModel->getItem(index);

 qDebug() << "Label 2, branch id:" << item->getField("id") << "name:" << item->getField("name");

 // Получение пути к элементу
 QStringList path=item->getPath();

 qDebug() << "Label 3";

 // Получение путей ко всем подветкам
 QList<QStringList> subbranchespath=item->getAllChildrenPath();

 qDebug() << "Label 4";

 // Сортировка массива веток по длине пути
 qSort(subbranchespath.begin(),subbranchespath.end(),compare_QStringList_len);

 qDebug() << "Label 5";

 // Удаление всех таблиц конечных записей для нужных подветок
 // Удаление всех подчиненных элементов для нужных подветок
 // Вначале пробегаются самые длинные ветки а потом более короткие
 for(int i=subbranchespath.size()-1;i>=0;i--)
 {
  qDebug() << "Delete subbranch, id:" << knowTreeModel->getItem(subbranchespath.at(i))->getField("id") << "name:" << knowTreeModel->getItem(subbranchespath.at(i))->getField("name");
  ( knowTreeModel->getItem(subbranchespath.at(i)) )->recordtableDeleteAllRecords();
  // ( knowTreeModel->getItem(subbranchespath.at(i)) )->removeAllChildren();
 }

 qDebug() << "Label 6";

 // Удаление таблицы конечных записей для самой удаляемой ветки
 // Удаление подчиненных элементов для самой удаляемой ветки
 qDebug() << "Delete rootbranch, id:" << knowTreeModel->getItem(path)->getField("id") << "name:" << knowTreeModel->getItem(path)->getField("name");
 ( knowTreeModel->getItem(path) )->recordtableDeleteAllRecords();
 // ( knowTreeModel->getItem(path) )->removeAllChildren();

 qDebug() << "Label 7";

 // Удаление ветки на экране, при этом удалятся все подветки
 qDebug() << "This branch have row() as" << index.row();
 if(index.isValid()) qDebug() << "Index valid";
 else qDebug() << "Index non valid";
 knowTreeModel->removeRows(index.row(), 1, index.parent());

 /*
 // Удаление всех нужных подветок
 // Пробегаются самые длинные ветки а потом более короткие
 for (int i=subbranchespath.size()-1;i>=0;i--)
  if(knowTreeModel->isItemValid(subbranchespath.at(i)))
   {
    TreeItem *current_item=knowTreeModel->getItem(subbranchespath.at(i));
    delete current_item;
   }
 */

 qDebug() << "Label 8";
}


void TreeScreen::cut_branch(void)
{
 bool copy_result;
 
 copy_result=copy_branch();

 if(copy_result)
  del_branch("cut");
}


bool TreeScreen::copy_branch(void)
{
 qDebug() << "In copy_branch()";

 // Сохраняется текст в окне редактирования
 find_object<MainWindow>("mainwindow")->saveTextarea();

 // Получение списка индексов QModelIndex выделенных элементов
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();

 // Если выбрано более одной ветки
 if(selectitems.size()>1)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Unavailable action"));
   messageBox.setText(tr("Please select a single item for copy."));
   messageBox.addButton(tr("OK"),QMessageBox::AcceptRole);
   messageBox.exec();
   return false;
  }


 // Получение индекса выделенной ветки
 QModelIndex index=getCurrentItemIndex();

 // Получение ссылки на узел, который соответствует выделенной ветке
 TreeItem *item=knowTreeModel->getItem(index);

 // Получение пути к выделенной ветке
 QStringList path=item->getPath();

 // Получение путей ко всем подветкам
 QList<QStringList> subbranchespath=item->getAllChildrenPath();


 // Проверка, содержит ли данная ветка как шифрованные
 // так и незашифрованные данные
 bool nocryptPresence=false;
 bool encryptPresence=false;

 // Флаги на основе состояния текущей ветки
 if(knowTreeModel->getItem(path)->getField("crypt")=="1")
  encryptPresence=true;
 else
  nocryptPresence=true;

 // Флаги на основе состояния подветок
 foreach(QStringList currPath, subbranchespath)
  if(knowTreeModel->getItem(currPath)->getField("crypt")=="1")
   encryptPresence=true;
  else
   nocryptPresence=true;

 // Если ветка содержит как шифрованные так и нешифрованные данные
 // то такую ветку копировать в буфер нельзя
 if(nocryptPresence==true && encryptPresence==true)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Unavailable action"));
   messageBox.setText(tr("This item contains both unencrypted and encrypted data. Copy/paste operation is possible only for item that contain similar type data."));
   messageBox.addButton(tr("OK"),QMessageBox::AcceptRole);
   messageBox.exec();
   return false;
  }


 // -------------------
 // Копирование в буфер
 // -------------------

 qDebug() << "Branch copy to buffer";

 // Создается ссылка на буфер обмена
 QClipboard *cbuf=QApplication::clipboard();

 // Создается объект с данными для заполнения буфера обмена
 // Если в буфере есть какие-то старые данные, они удаляются
 static int fillflag=0;
 if(fillflag==1)
  {
   const ClipboardBranch *branch_clipboard_data_previos;
   branch_clipboard_data_previos=qobject_cast<const ClipboardBranch *>(cbuf->mimeData());
   if(branch_clipboard_data_previos!=NULL)delete branch_clipboard_data_previos;
   fillflag=0;
  }
 ClipboardBranch *branch_clipboard_data=new ClipboardBranch();
 fillflag=1;

 // Добавление корневой ветки
 addBranchToClipboard(branch_clipboard_data, path, true);

 // Добавление прочих веток
 foreach(QStringList curr_path, subbranchespath)
  addBranchToClipboard(branch_clipboard_data, curr_path, false);

 branch_clipboard_data->print();

 // Объект с ветками помещается в буфер обмена
 cbuf->setMimeData(branch_clipboard_data);

 return true;
}


// Вспомогательная функция при копировании ветки в буфер
void TreeScreen::addBranchToClipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root)
{
 TreeItem *curr_item;
 QMap<QString, QString> curr_item_fields;
 QString branch_id;
 RecordTableData *curr_item_record_table;

 // Добавление ветки
 curr_item=knowTreeModel->getItem(path);
 curr_item_fields=curr_item->getAllFields(); // Раньше было getAllFieldsDirect()
 branch_id=curr_item_fields["id"];
 if(is_root)
  branch_clipboard_data->addBranch("-1", curr_item_fields);
 else
  branch_clipboard_data->addBranch(curr_item->getParentId(),
                                    curr_item_fields);

 // Добавление конечных записей
 curr_item_record_table=curr_item->recordtableGetTableData();
 for(int i=0; i<curr_item_record_table->size (); i++)
 {
  // Образ записи, включающий все текстовые поля (и HTML-код записи как "text")
  QMap<QString, QString> exemplar=curr_item_record_table->getRecordExemplar(i);

  // Имя директории, в которой расположена запись и ее файлы
  QString directory=mytetraConfig.get_tetradir()+"/base/"+exemplar["dir"];

  branch_clipboard_data->addRecord(branch_id,
                                    exemplar,
                                    get_files_from_directory(directory, "*.png"));
 }
}


// Вставка ветки из буфера на том же уровне, что и выбранная
void TreeScreen::paste_branch(void)
{
 qDebug() << "In paste_branch";

 pasteBranchSmart(true);
}


// Вставка ветки из буфера в виде подветки выбранной ветки
void TreeScreen::paste_subbranch(void)
{
 qDebug() << "In paste_subbranch";

 pasteBranchSmart(false);
}


void TreeScreen::pasteBranchSmart(bool is_branch)
{
 // Проверяется, содержит ли буфер обмена данные нужного формата
 const QMimeData *mimeData=QApplication::clipboard()->mimeData();
 if(mimeData==NULL)
  return;
 if( ! (mimeData->hasFormat("mytetra/branch")) )
  return;

 // Получение списка индексов QModelIndex выделенных элементов
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();

 // Если выбрано более одной ветки
 if(selectitems.size()>1)
  {
   QMessageBox messageBox(this);
   messageBox.setWindowTitle(tr("Unavailable action"));
   messageBox.setText(tr("You've selected ")+QString::number(selectitems.size())+tr(" items.\nPlease select single item for enabling paste operation."));
   messageBox.addButton(tr("OK"),QMessageBox::AcceptRole);
   messageBox.exec();
   return;
  }

 // Блокируется главное окно, чтобы при продолжительном выполнении
 // небыло возможности сделать другие действия
 find_object<MainWindow>("mainwindow")->setDisabled(true);


 // Создается ссылка на буфер обмена
 QClipboard *cbuf=QApplication::clipboard();

 // Извлечение объекта из буфера обмена
 const ClipboardBranch *branch;
 branch=qobject_cast<const ClipboardBranch *>(cbuf->mimeData());
 branch->print();
 branch->printIdTree();


 // Получение индекса выделенной строки
 QModelIndex index=getCurrentItemIndex();

 // Добавление ветки
 QString pasted_branch_id;
 if(is_branch)
  pasted_branch_id=knowTreeModel->pasteNewSiblingBranch(index, (ClipboardBranch *)branch);
 else
  pasted_branch_id=knowTreeModel->pasteNewChildBranch(index, (ClipboardBranch *)branch);


 // Установка курсора на новую созданную ветку
 TreeItem *pasted_branch_item=knowTreeModel->getItemById(pasted_branch_id);
 QStringList pasted_branch_path=pasted_branch_item->getPath();
 find_object<MainWindow>("mainwindow")->setTreePosition(pasted_branch_path);

 // Сохранение дерева веток
 find_object<TreeScreen>("TreeScreen")->saveKnowTree();

 // Разблокируется главное окно
 find_object<MainWindow>("mainwindow")->setEnabled(true);
}


// Шифрование ветки
void TreeScreen::encrypt_branch(void)
{
 // Если пароль в данной сессии не вводился
 if(globalParameters.getCryptKey().size()==0)
  {
   // Запрашивается пароль
   Password password;
   if(password.retrievePassword()==true) // Если пароль введен правильно
    encryptBranchItem(); // Ветка шифруется
  }
 else
  {
   // Иначе считается, что шифрующий ключ уже был задан и он правильный
 
   encryptBranchItem(); // Ветка шифруется
  }
}



// Расшифровка ветки
void TreeScreen::decrypt_branch(void)
{
 // Если пароль в данной сессии не вводился
 if(globalParameters.getCryptKey().size()==0)
  {
   // Запрашивается пароль
   Password password;
   if(password.retrievePassword()==true) // Если пароль введен правильно
    decryptBranchItem(); // Ветка расшифровывается
  }
 else
  {
   // Иначе пароль в данной сессии вводился и он правильный

   decryptBranchItem(); // Ветка расшифровывается
  }
}


void TreeScreen::encryptBranchItem(void)
{
 // Получаем указатель на текущую выбранную ветку дерева
 TreeItem *item = knowTreeModel->getItem( getCurrentItemIndex() );

 // Шифрация ветки и всех подветок
 item->switchToEncrypt();

 // Сохранение дерева веток
 find_object<TreeScreen>("TreeScreen")->saveKnowTree();

 // Обновляеются на экране ветка и ее подветки
 updateBranchOnScreen( getCurrentItemIndex() );
}


void TreeScreen::decryptBranchItem(void)
{
 // Получаем указатель на текущую выбранную ветку дерева
 TreeItem *item = knowTreeModel->getItem( getCurrentItemIndex() );

 // Расшифровка ветки и всех подветок
 item->switchToDecrypt();

 // Сохранение дерева веток
 find_object<TreeScreen>("TreeScreen")->saveKnowTree();

 // Обновляеются на экране ветка и ее подветки
 updateBranchOnScreen( getCurrentItemIndex() );

 // Проверяется, остались ли в дереве зашифрованные данные
 // если зашифрованных данных нет, будет предложено сбросить пароль
 treeCryptControl();
}


// Обновление на экране ветки и подветок
void TreeScreen::updateBranchOnScreen(const QModelIndex &index)
{
 // Для корневой ветки дается команда чтобы модель сообщила о своем изменении
 knowTreeModel->emitSignalDataChanged(index);

 // По модельному индексу выясняется указатель на ветку
 TreeItem *item=knowTreeModel->getItem(index);

 // Перебираются подветки
 QList<QStringList> updatePathts=item->getAllChildrenPath();
 foreach(QStringList currentPath, updatePathts)
 {
  TreeItem *currentItem=knowTreeModel->getItem(currentPath);

  QModelIndex currentIndex=knowTreeModel->getIndexByItem(currentItem);

  // Для подветки дается команда чтобы модель сообщила о своем изменении
  knowTreeModel->emitSignalDataChanged(currentIndex);
 }
}


// Действия при клике на ветку дерева
void TreeScreen::on_knowtree_clicked(const QModelIndex &index)
{
 // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

 // Сохраняется текст в окне редактирования в соответсвующий файл
 find_object<MainWindow>("mainwindow")->saveTextarea(); 
 
 // Получаем указатель на текущую выбранную ветку дерева
 TreeItem *item = knowTreeModel->getItem(index);

 // Вначале все инструменты работы с веткой включаются
 QMapIterator<QString, QAction *> i(actionList);
 while (i.hasNext())
  {
   i.next();
   i.value()->setEnabled(true);
  }

 // Проверяется, происходит ли клик по зашифрованной ветке
 if(item->getField("crypt")=="1")
  {
   // Если пароль доступа к зашифрованным данным не вводился в этой сессии
   if(globalParameters.getCryptKey().length()==0)
    {
     // Запрашивается пароль
     Password password;
     if(password.retrievePassword()==false)
      {
       // Устанавливаем пустые данные для отображения таблицы конечных записей
       find_object<RecordTableView>("RecordTableView")->setTableData(NULL);

       // Все инструменты работы с веткой отключаются
       QMapIterator<QString, QAction *> i(actionList);
       while (i.hasNext())
       {
        i.next();
        i.value()->setEnabled(false);
       }

       return; // Программа дальше не идет, доделать...
      }
    }
  }


 // Получаем указатель на данные таблицы конечных записей
 RecordTableData *rtdata=item->recordtableGetTableData();

 // Устанавливаем данные таблицы конечных записей
 find_object<RecordTableView>("RecordTableView")->setTableData(rtdata);

 // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
 knowTreeView->resizeColumnToContents(0);

 // Переключаются окна (используется для мобильного интерфейса)
 globalParameters.getWindowSwitcher()->switchFromTreeToRecordtable();
}


/*
void treescreen::on_knowTreeView_repaint(void)
{
 // Попытка расширить нулевую колонку с деревом, если содержимое слишком широкое
 knowTreeView->resizeColumnToContents(0);
}
*/


void TreeScreen::updateSelectedBranch(void)
{
 // Получение списка выделенных Item-элементов
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();

 // Обновление на экране
 for (int i = 0; i < selectitems.size(); ++i) 
  knowTreeView->update(selectitems.at(i));
}


QItemSelectionModel * TreeScreen::getSelectionModel(void)
{
 return knowTreeView->selectionModel();
}


void TreeScreen::setCursorToIndex(QModelIndex index)
{
 knowTreeView->selectionModel()->setCurrentIndex(index,QItemSelectionModel::ClearAndSelect);
}

 
// Получение номера первого выделенного элемента
int TreeScreen::getFirstSelectedItemIndex(void)
{
 // Получение списка выделенных Item-элементов
 QModelIndexList selectitems=knowTreeView->selectionModel()->selectedIndexes();

 if(selectitems.isEmpty())
  return -1; // Если ничего не выделено
 else
  return (selectitems.at(0)).row(); // Индекс первого выделенного элемента
}


// Получение индекса текущего элемента на котором стоит курсор
QModelIndex TreeScreen::getCurrentItemIndex(void)
{
 return knowTreeView->selectionModel()->currentIndex();
}


// Метод, следящий, не обнулилось ли дерево
// Если дерево стало пустым, данный метод добавит одну ветку
void TreeScreen::treeEmptyControl(void)
{
 qDebug() << "treescreen::tree_empty_control() : Tree branch count " << knowTreeModel->rowCount();

 if(knowTreeModel->rowCount()==0)
  {
   qDebug() << "treescreen::tree_empty_control() : Tree empty, create blank branch";

   insBranchProcess(QModelIndex(), tr("Rename me"), false);
  }
}


// Метод, следящий, не стало ли дерево содержать только незашифрованные записи
// Если в дереве нет шифрования, задается вопрос, нужно ли сбросить пароль
void TreeScreen::treeCryptControl(void)
{
 // Если в дереве нет шифрования
 if(knowTreeModel->isContainsCryptBranches()==false)
  {
   // Запускается диалог сброса пароля шифрования
   Password pwd;
   pwd.resetPassword();
  }
}


// Сохранение дерева веток на диск
void TreeScreen::saveKnowTree(void)
{
 knowTreeModel->save();
}


// Перечитывание дерева веток с диска
void TreeScreen::reloadKnowTree(void)
{
 knowTreeModel->reload();
}
