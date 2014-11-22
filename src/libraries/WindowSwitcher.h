#ifndef WINDOWSWITCHER_H
#define WINDOWSWITCHER_H

#include <QObject>

class WindowSwitcher : public QObject
{
 Q_OBJECT

public:
 explicit WindowSwitcher(QObject *parent = 0);

 void switchFromTreeToRecordtable(void);
 void switchFromTreeToFindInBase(void);

 static void switchFromRecordToRecordtable(void); // Используется как callback из редактора MetaEditor
 void switchFromRecordToFindInBase(void);

 void switchFromRecordtableToRecord(void);
 void switchFromRecordtableToFindInBase(void);
 void switchFromRecordtableToTree(void);

 void closeFindInBase(void);

 void switchDirectTo(QString widgetName);

signals:

public slots:

private:

 bool enableSwitcher;

};

#endif // WINDOWSWITCHER_H
