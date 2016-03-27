#include <QString>

#include "main.h"
#include "WalkHistory.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"


WalkHistory::WalkHistory(void)
{
 clear();
}


WalkHistory::~WalkHistory(void)
{
 
}


void WalkHistory::clear(void)
{
 qDebug() << "WalkHistory::clear()";

 historyId.clear();
 data.clear();

 historyPoint=-1;
 leaveMarkPoint=-1;
 dropFlag=false;
}


void WalkHistory::add(QString id,
                      int cursorPosition,
                      int scrollBarPosition,
                      int mode)
{
  qDebug() << "WalkHistory::add() : id " << id;
  qDebug() << "WalkHistory::add() : mode " << mode;
  qDebug() << "WalkHistory::add() start status:";
  print();

  if(id.length()==0)
    return;

  if(dropFlag)
    return;

  data[id].cursorPosition=cursorPosition;
  data[id].scrollBarPosition=scrollBarPosition;

  // Если история слишком большая
  if( historyId.length() > WALK_HISTORY_MAX )
  {
    qDebug() << "WalkHistory::add() : history too big, trim";

    historyId.removeFirst();

    if(historyPoint>0)
      historyPoint=historyPoint-1;

    if(leaveMarkPoint>0)
      leaveMarkPoint=leaveMarkPoint-1;
  }


  // Если происходит просто запоминание нового ID без движения по истории
  if(mode==WALK_HISTORY_GO_NONE)
  {
    qDebug() << "WalkHistory::add() : mode WALK_HISTORY_GO_NONE";

    // Повторяющийся идентификатор не запоминается
    if(historyId.size()>0)
      if(historyId.last()==id)
      {
        qDebug() << "WalkHistory::add() : Repeate ID, dont save";
        print();
        return;
      }

    // Идентификатор добавляется в историю
    historyId << id;
    historyPoint=historyId.length()-1;

    // Без этой команды работало правильно, но она вроде должна быть в этом месте
    leaveMarkPoint=-1;

    print();
    return;
  } // Закончилось условие что происходит запоминание без движения по истории

  ///////////////////////////////////////////////////////////////////////////////
  // В этом месте остались режимы WALK_HISTORY_GO_PREVIOUS и WALK_HISTORY_GO_NEXT
  ///////////////////////////////////////////////////////////////////////////////

  // Если история была пуста
  if(historyPoint==-1)
  {
    // Идентификатор просто добавляется в историю
    historyId << id;
    historyPoint=0;

    qDebug() << "WalkHistory::add() : history is empty, only add ID";
    print();
    return;
  }


  // Если добавляемый идентификатор не равен последнему в истории, его нужно добавить (даже если идет движение назад или вперед по истории)
  if(id!=historyId.last())
  {
    qDebug() << "WalkHistory::add() : new ID not equal new ID, adding";

    // Если указатель находится в конце истории
    if(historyPoint==(historyId.length()-1))
      historyPoint=historyId.length(); // Указатель перемещается на ячейку, которая добавится следующей командой

    // Идентификатор добавляется в историю
    historyId << id;

    qDebug() << "WalkHistory add id: " << id;
    qDebug() << "WalkHistory pointer: " << historyPoint;
    qDebug() << "WalkHistory lenght: " << historyId.length();
  }


  // Если происходит запоминание с движением назад по истории
  if(mode==WALK_HISTORY_GO_PREVIOUS)
  {
    qDebug() << "WalkHistory::add() : mode WALK_HISTORY_GO_PREVIOUS";

    // Если указатель находится в конце истории
    if(historyPoint==(historyId.length()-1))
    {
      qDebug() << "WalkHistory::add() : pointer on end";

      // Запоминается момент, когда началось движение назад по истории
      leaveMarkPoint=historyPoint;

      historyPoint=historyId.length()-1;
    }

    // Происходит перемещение указателя назад
    if(historyPoint>0)
      historyPoint--;
  } // Закончилось условие что происходит запоминание с движением назад по истории


  // Если происходит запоминание с движением вперед по истории
  if(mode==WALK_HISTORY_GO_NEXT)
  {
    qDebug() << "WalkHistory::add() : mode WALK_HISTORY_GO_NEXT";

    // Если указатель находится в конце истории
    if(historyPoint==(historyId.length()-1))
    {
      qDebug() << "WalkHistory::add() : pointer on end, no move to next";

      print();
      return;
    }
    else
    {
      // Иначе указатель находится где-то в середине истории
      qDebug() << "WalkHistory::add() : pointer on middle history";

      // Происходит перемещение указателя вперед
      if( historyPoint < (historyId.length()-1) )
        historyPoint++;

      // Если указатель при движении вперед достиг точки, в которой начинал движение назад
      if(historyPoint>leaveMarkPoint)
      {
        historyPoint=historyId.length()-1; // Прыжок на самую последнюю запись
        leaveMarkPoint=-1; // Точка начала движения назад становится неактивной
      }
    }

  } // Закончилось условие что происходит запоминание с движением вперед по истории


  print();
}


/*
void WalkHistory::switchToPrevious(void)
{
 if(historyPoint>0)
  historyPoint--;

 qDebug() << "WalkHistory::switchToPrevious() :";
 // print();
}


void WalkHistory::switchToNext(void)
{
 if( historyPoint < (historyId.length()-1) )
  historyPoint++;

 qDebug() << "WalkHistory::switchToNext() :";
 // print();
}
*/


QString WalkHistory::getId()
{
 if(historyPoint>=0 && historyPoint<=(historyId.length()-1))
  return historyId[historyPoint];
 else
  return QString();
}


int WalkHistory::getCursorPosition(QString id)
{
 if(data.contains(id))
  return data[id].cursorPosition;
 else
  return 0;
}


int WalkHistory::getScrollBarPosition(QString id)
{
 if(data.contains(id))
  return data[id].scrollBarPosition;
 else
  return 0;
}


void WalkHistory::removeHistoryData(QString id)
{
 if(data.contains(id))
  data.remove(id);
}


void WalkHistory::print(void)
{
  return;

  qDebug() << "WalkHistory length: " << historyId.length();
  qDebug() << "WalkHistory pointer: " << historyPoint;
  qDebug() << "WalkHistory leave mark: " << leaveMarkPoint;

  qDebug() << "WalkHistory table ---v";

  for(int i=0; i<historyId.size(); i++)
    qDebug() << "WalkHistory " << i << ":" << historyId.at(i);

  qDebug() << "WalkHistory table ---^";
}


void WalkHistory::setDrop(bool flag)
{
 dropFlag=flag;
}


// Проверка наличия идентификатора в базе, и его исключение из истории, если
// идентификатор не обнаружен
void WalkHistory::checkId(QString id)
{
 // Выясняется ссылка на модель дерева данных
 KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());

 // Если запись с указанным идентификатором существует
 if(dataModel->getRecordPath(id).length()>0)
  return; // Ничего делать не нужно


 // Здесь запись с указанным идентификатором не существует, и его надо
 // удалить из истории

 // Выясняется, сколько раз встречается несуществущий идентификатор в истории
 // и удаляется из истории
 int idRemoveCount=historyId.removeAll(id);

 // Происходит перемещение указателя назад на столько ячеек, сколько
 // было удалено идентификторов
 for(int i=0; i<idRemoveCount; i++)
  if(historyPoint>0)
    historyPoint--;
}

