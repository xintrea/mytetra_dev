#include <QDebug>
#include <QObject>
#include <QMessageBox>

#include "DebugHelper.h"
#include "libraries/ActionLogger.h"


extern ActionLogger actionLogger;
extern QObject *pMainWindow;


void logPrint(char *lpszText, ...)
{
  va_list argList;
  FILE *pFile;

  // открываем лог-файл для добавления данных
  if((pFile = fopen("mytetralog.txt", "a+")) == nullptr)
  {
    printf("\nLog file not writable\n");
    return;
  }

  // инициализируем список аргументов
  va_start(argList, lpszText);

  // пишем текст в файл
  vfprintf(pFile, lpszText, argList);
  // putc('\n', pFile);

  // пишем текст на экран
  vprintf(lpszText, argList);

  // закрываем файл
  fclose(pFile);
  va_end(argList);

  // успешное завершение
  return;
}


void criticalError(QString message)
{
  QMap<QString, QString> data;
  data["errorMessage"]=message;
  actionLogger.addAction("criticalError", data);

  qDebug() << " ";
  qDebug() << "---------------";
  qDebug() << "Critical error!";
  qDebug() << "---------------";
  qDebug() << message;
  qDebug() << "---------------";
  qDebug() << " ";

  QMessageBox::critical(qobject_cast<QWidget *>(pMainWindow), "Critical error",
                        message+"\n\nProgramm was closed.",
                        QMessageBox::Ok);

  exit(1);
}


// Функция-помощник при отладке генерации XML-кода. Преобразует узел DOM в строку
QString xmlNodeToString(QDomNode xmlData)
{
  // Если узел представляет собой полностью документ
  if(xmlData.isDocument())
  {
    // Значит этот узел можно напрямую преобразовать
    // в документ, а потом в XML строку
    return xmlData.toDocument().toString();
  }
  else
  {
    // Иначе узел не является документом, и его нужно обрабатывать по-другому

    // Строка, где будет размещен XML код
    QString xmlcode;

    // Поток, связаный со строкой, в который будет направляться XML код узла
    QTextStream stream(&xmlcode, QIODevice::WriteOnly);

    // XML документ записывается в поток, после чего автоматом окажется в строке xmlcode
    xmlData.save(stream, 1);

    return xmlcode;
  }
}


// Рекурсивная печать дерева объектов, т.к. dumpObjectInfo() и dumpObjectTree() не работают
void printObjectTreeRecurse(QObject *pobj)
{
  static int indent=0;

  QObjectList olist;

  olist=pobj->children();

  for(int i=0;i<olist.size();++i)
  {
    QObject *currobj;
    currobj=olist.at(i);

    QString indentline=".";
    for(int j=0;j<indent;j++)indentline=indentline+".";

    if((currobj->objectName()).length()==0)
      qDebug("%s%s", indentline.toLocal8Bit().data(),
                     currobj->metaObject()->className() );
    else
      qDebug("%s%s, NAME %s", indentline.toLocal8Bit().data(),
                              currobj->metaObject()->className(),
                              (currobj->objectName()).toLocal8Bit().data() );

    indent++;
    printObjectTreeRecurse(currobj);
    indent--;
  }
}


// Печать дерева объектов, основная функция
void printObjectTree(void)
{
  qDebug() << "Object tree";

  printObjectTreeRecurse(pMainWindow);
}


//DebugHelper::DebugHelper()
//{

//}
