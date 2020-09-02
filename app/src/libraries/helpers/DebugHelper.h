#ifndef DEBUGHELPER_H
#define DEBUGHELPER_H

#include <QString>
#include <QDomNode>

// Функции, используемые при отладке

void logPrint(char *lpszText, ...);

[[ noreturn ]] void criticalError(QString message);

QString xmlNodeToString(QDomNode xmlData);

void printObjectTree(void);


//class DebugHelper
//{
//public:
//    DebugHelper();
//};

#endif // DEBUGHELPER_H
