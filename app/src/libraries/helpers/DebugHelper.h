#ifndef DEBUGHELPER_H
#define DEBUGHELPER_H

// Функции, используемые при отладке


class QString;
class QDomNode;

void logPrint(char *lpszText, ...);

[[ noreturn ]] void criticalError(QString message);

QString xmlNodeToString(QDomNode xmlData);

void printObjectTree(void);

void setDebugMessageHandler();

//class DebugHelper
//{
//public:
//    DebugHelper();
//};

#endif // DEBUGHELPER_H
