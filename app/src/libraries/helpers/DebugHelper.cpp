#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <QString>
#include <QDomNode>
#include <QTime>
#include <QTextCodec>

#include <iostream>

#include "DebugHelper.h"
#include "libraries/ActionLogger.h"
#include "libraries/GlobalParameters.h"
#include "models/appConfig/AppConfig.h"


extern ActionLogger actionLogger;
extern QObject *pMainWindow;
extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;

using namespace std;


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


void smartPrintDebugMessage(QString msg)
{
    if(globalParameters.getTargetOs()=="any" ||
       globalParameters.getTargetOs()=="meego")
    {
        QTime currTime = QTime::currentTime();
        QString timeText=currTime.toString("hh:mm:ss");
        msg=timeText+" "+msg;

        // unsigned int messageLen=msg.toLocal8Bit().size();
        // fwrite(msg.toLocal8Bit().data(), sizeof(char), messageLen, stderr);

        // Установка кодека для нормальной работы консоли
        if(globalParameters.getConsoleCodepage()!=globalParameters.getSystemCodepage() &&
           globalParameters.getConsoleCodepage().size()>0) {
            QTextCodec::setCodecForLocale(QTextCodec::codecForName( globalParameters.getConsoleCodepage().toLatin1() ));
        }

        cout << msg.toLocal8Bit().data();

        // Установка кодека для дальнейшей нормальной работы файловых инструкций
        if(globalParameters.getConsoleCodepage()!=globalParameters.getSystemCodepage() &&
           globalParameters.getSystemCodepage().size()>0) {
            QTextCodec::setCodecForLocale(QTextCodec::codecForName( globalParameters.getSystemCodepage().toLatin1() ));
        }

    }

    // В Android пока неясно, как смотреть поток ошибок, для андроида qDebug() не переопределяется
}


// Обработчик (хендлер) вызовов qDebug()
// Внутри этого обработчика нельзя использовать вызовы qDebug(), т. к. получится рекурсия
#if QT_VERSION < 0x050000
void myMessageOutput(QtMsgType type, const char *msg)
#else
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msgText)
#endif
{
 #if QT_VERSION >= 0x050000
 Q_UNUSED(context)
 #endif

 #if QT_VERSION < 0x050000
  QString msgText( QString::fromUtf8(msg) );
 #endif

 // Пока идет инициализация конфигурации программы
 if(!mytetraConfig.is_init())
 {
  smartPrintDebugMessage("[INI] "+msgText+"\n");
  return;
 }

 // Если в конфигурации запрещен вывод отладочных сообщений
 if(!mytetraConfig.get_printdebugmessages())
  return;

 switch (type) {
   case QtDebugMsg:
       smartPrintDebugMessage("[DBG] "+msgText+"\n");
       break;
   case QtWarningMsg:
       smartPrintDebugMessage("[WRN] "+msgText+"\n");
       break;
   case QtCriticalMsg:
       smartPrintDebugMessage("[CRERR] "+msgText+"\n");
       break;
   case QtFatalMsg:
       smartPrintDebugMessage("[FTERR] "+msgText+"\n");
       abort();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
   case QtInfoMsg:
       smartPrintDebugMessage("[INF] "+msgText+"\n");
       break;
#endif
 }

 // #endif
}


void setDebugMessageHandler()
{
 qDebug() << "Debug message before set message handler for target OS: " << globalParameters.getTargetOs();

 // Для десктопных операционок можно переустановить обработчик qDebug()
 // Для Андроида переустановка qDebug() приводит к невозможности получения отладочных сообщений в удаленном отладчике
 if(globalParameters.getTargetOs()=="any" ||
    globalParameters.getTargetOs()=="meego")
  {
   qDebug() << "Set alternative handler myMessageOutput() for debug message";

   #if QT_VERSION < 0x050000
    qInstallMsgHandler(myMessageOutput);
   #else
    qInstallMessageHandler(myMessageOutput);
   #endif
  }

 qDebug() << "Debug message after set message handler";
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
