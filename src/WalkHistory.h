#ifndef _WALKHISTORY_H_
#define _WALKHISTORY_H_

#include <QtGlobal>
#include <QObject>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMap>

#define WALK_HISTORY_MAX 100

#define WALK_HISTORY_GO_NONE     0
#define WALK_HISTORY_GO_PREVIOUS 1
#define WALK_HISTORY_GO_NEXT     2

struct WALK_HISTORY_DATA
{
 int cursorPosition;
 int scrollBarPosition;
};


class WalkHistory : public QObject
{
 Q_OBJECT
  
public:
 WalkHistory(void);
 ~WalkHistory(void);

 void clear(void);

 void add(QString id, int cursorPosition, int scrollBarPosition, int mode=WALK_HISTORY_GO_NONE);

 QString getId();
 int getCursorPosition(QString id);
 int getScrollBarPosition(QString id);
 void removeHistoryData(QString id);

 void setDrop(bool flag);

protected:

  bool dropFlag;

  void print(void);

  // Эти методы видимо ненужны, подумать
  void switchToPrevious(void);
  void switchToNext(void);

  void checkId(QString id);

  // Указатель в списке истории посещения записей
  // Обычно указывает на последнюю в списке запись
  int historyPoint;

  // Список идентификаторов посещаемых записей
  QStringList historyId;

  // Для каждого идентификатора запоминается позиция курсора и прокрутки
  QMap<QString, WALK_HISTORY_DATA> data;
};

#endif // _WALKHISTORY_H_
