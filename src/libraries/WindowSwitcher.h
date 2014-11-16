#ifndef WINDOWSWITCHER_H
#define WINDOWSWITCHER_H

#include <QObject>

class WindowSwitcher : public QObject
{
 Q_OBJECT

public:
 explicit WindowSwitcher(QObject *parent = 0);

 void switchFromTreeToRecordtable(void);
 void switchFromRecordtableToRecord(void);
 static void switchFromRecordToRecordtable(void); // Используется как callback из редактора MetaEditor
 void switchFromTreeToFindInBase(void);
 void switchFromRecordToFindInBase(void);
 void switchFromRecordtableToFindInBase(void);
 void closeFindInBase(void);

signals:

public slots:

private:

 bool enableSwitcher;

};

#endif // WINDOWSWITCHER_H
