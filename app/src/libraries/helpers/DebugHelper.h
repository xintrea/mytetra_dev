#ifndef DEBUGHELPER_H
#define DEBUGHELPER_H

// Функции, используемые при отладке


class QString;
class QDomNode;

[[ noreturn ]] void criticalError(QString message);

QString xmlNodeToString(QDomNode xmlData);

void printObjectTree(void);

void setDebugMessageHandler();

#endif // DEBUGHELPER_H
