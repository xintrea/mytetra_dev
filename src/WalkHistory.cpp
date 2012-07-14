#include <QString>

#include "main.h"
#include "WalkHistory.h"
#include "KnowTreeModel.h"


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

 data[id].cursorPosition=cursorPosition;
 data[id].scrollBarPosition=scrollBarPosition;

 if(dropFlag)
  {
   qDebug() << "WalkHistory::add() : Dropping adding.";
   return;
  }

 // Если происходит просто запоминание без движения по истории
 if(mode==WALK_HISTORY_GO_NONE)
  {
   // Если указатель истории не указывает на конец истории (на вершину стека)
   if( historyPoint!=(historyId.length()-1) && historyPoint!=(-1) )
    {
     // Значит указатель находится где-то в середине истории

     // Нужно удалить все старшие записи от указателя истории
     int deleteStart=historyPoint+1;
     int deleteEnd=historyId.length();
     int deleteCount=deleteEnd-deleteStart;

     for(int i=0; i<deleteCount; i++)
      historyId.removeLast();
    }

   // Повторяющийся идентификатор не запоминается
   if(historyId.size()>0)
    if(historyId.last()==id)
     {
      print();
      return;
     }

   // Идентификатор добавляется в историю
   historyId << id;
   historyPoint=historyId.length()-1;

   // Если история слишком большая
   if( historyId.length() > WALK_HISTORY_MAX )
    {
     historyId.removeFirst();
     historyPoint=historyId.length()-1;
    }
  } // Закончилось условие что происходит запоминание без движения по истории


 // Если происходит запоминание с движением назад по истории
 if(mode==WALK_HISTORY_GO_PREVIOUS)
  {
   // Если история была пуста
   if(historyPoint==-1)
    {
     // Идентификатор просто добавляется в историю
     historyId << id;
     historyPoint=0;

     print();
     return;
    }

   // Если указатель находится в конце истории
   if(historyPoint==(historyId.length()-1))
    {
     // Если добавляемый идентификатор не равен последнему в истории
     // Его нужно добавить
     if(id!=historyId.last())
      {
       // Идентификатор добавляется в историю
       historyId << id;
       historyPoint=historyId.length()-1;

       // Если история слишком большая
       if( historyId.length() > WALK_HISTORY_MAX )
        {
         historyId.removeFirst();
         historyPoint=historyId.length()-1;
        }
      }

     // Происходит перемещение указателя назад
     if(historyPoint>0)
      historyPoint--;
    } // Закрылось условие что указатель находился в конце истории
   else
    {
     // Иначе указатель находится где-то в середине истории

     if(id!=historyId.at(historyPoint))
      {
       // Невозможная ситуация. При правильной работе механизма выбора и кликов
       // по записям, она не может возникнуть. Но так как возможны баги в разных
       // версиях Qt, жесткой реации на получение такой ситуации делать нельзя
       QString message="WARNING! Bad walk history state if move previous. History max index "+QString::number(historyId.length()-1)+", History point "+QString::number(historyPoint)+", History id "+historyId.at(historyPoint)+", Current id "+id;
       qDebug() << message;

       checkId(id);
       checkId(historyId.at(historyPoint));
        
       // critical_error(message);
       // return;
      }

     // Происходит перемещение указателя назад
     if(historyPoint>0)
      historyPoint--;
    }
  } // Закончилось условие что происходит запоминание с движением назад по истории


 // Если происходит запоминание с движением вперед по истории
 if(mode==WALK_HISTORY_GO_NEXT)
  {
   // Если история была пуста
   if(historyPoint==-1)
    {
     // Идентификатор просто добавляется в историю
     historyId << id;
     historyPoint=0;

     print();
     return;
    }


   // Если указатель находится в конце истории
   if(historyPoint==(historyId.length()-1))
    {

     // Если добавляемый идентификатор не равен последнему в истории
     // Его нужно добавить
     if(id!=historyId.last())
      {
       // Идентификатор добавляется в историю
       historyId << id;
       historyPoint=historyId.length()-1;

       // Если история слишком большая
       if( historyId.length() > WALK_HISTORY_MAX )
        {
         historyId.removeFirst();
         historyPoint=historyId.length()-1;
        }
      }

     print();
     return;
    }
   else
    {
     // Иначе указатель находится где-то в середине истории

     if(id!=historyId.at(historyPoint))
      {
       // Невозможная ситуация. При правильной работе механизма выбора и кликов
       // по записям, она не может возникнуть. Но так как возможны баги в разных
       // версиях Qt, жесткой реации на получение такой ситуации делать нельзя
       QString message="WARNING! Bad walk history state if move next. History max index "+QString::number(historyId.length()-1)+", History point "+QString::number(historyPoint)+", History id "+historyId.at(historyPoint)+", Current id "+id;
       qDebug() << message;

       checkId(id);
       checkId(historyId.at(historyPoint));

       // critical_error(message);
       // return;
      }

     // Происходит перемещение указателя вперед
     if( historyPoint < (historyId.length()-1) )
      historyPoint++;
    }

  } // Закончилось условие что происходит запоминание с движением вперед по истории


 print();
}


void WalkHistory::switchToPrevious(void)
{
 if(historyPoint>0)
  historyPoint--;

 qDebug() << "WalkHistory::switchToPrevious() :";
 print();
}


void WalkHistory::switchToNext(void)
{
 if( historyPoint < (historyId.length()-1) )
  historyPoint++;

 qDebug() << "WalkHistory::switchToNext() :";
 print();
}


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
 qDebug() << "WalkHistory table ---v";
 qDebug() << "WalkHistory pointer: " << historyPoint;

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
 KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<QTreeView>("knowtree")->model());

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

