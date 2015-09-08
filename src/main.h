#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>

#include <QApplication>
#include <QClipboard>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QtGlobal>

#include <QDomNode>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QToolBar>
#include <QAbstractItemView>
#include <QThread>

using namespace std;

// ----------------------------------------------------------
// MyTetra - программа для накопления осмысленной информации
// Волгодонск, 2010 - 2015 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензиями:
// - GPL v.3
// - Modified BSD
// © Степанов С. М. и товарищи 2010 - 2015
// ----------------------------------------------------------

// Версия программы
#define APPLICATION_RELEASE_VERSION         1
#define APPLICATION_RELEASE_SUBVERSION     32
#define APPLICATION_RELEASE_MICROVERSION  143

// Поддерживаемая версия формата базы (хранилища)
#define CURRENT_FORMAT_VERSION    1
#define CURRENT_FORMAT_SUBVERSION 1


#define ADD_NEW_RECORD_TO_END         0
#define ADD_NEW_RECORD_BEFORE         1
#define ADD_NEW_RECORD_AFTER          2


// Прототипы функций, которые могут использоваться в других файлах
void logprint(char *lpszText, ...);
void critical_error(QString message);
void info_window(QString i);
QString xmlnode_to_string(QDomNode xmlData);

void remove_directory_to_trash(QString nameDirFrom);
void remove_file_to_trash(QString fileNameFrom);
QString create_temp_directory(void);
bool remove_directory(const QString &dirName);
bool copy_directory(const QString &fromName, const QString &toName);
QMap<QString, QByteArray> get_files_from_directory(QString dirName, QString fileMask="*");
bool save_files_to_directory(QString dirName, QMap<QString, QByteArray> fileList);

void print_object_tree(void);
bool compare_QStringList_len(const QStringList &list1, const QStringList &list2);
void insertActionAsButton(QToolBar *tools_line, QAction *action);
int imax(int x1, int x2);
int imin(int x1, int x2);
void myMessageOutput(QtMsgType type, const char *msg);
QStringList text_delimiter_decompose(QString text);
QString get_unical_id(void);
int get_milli_count(void);
void init_random(void);

void convertByteArrayToVector(const QByteArray &qba, vector<unsigned char> &vec);
void convertVectorToByteArray(const vector<unsigned char> &vec, QByteArray &qba);
QString encryptString(QByteArray key, QString line);
QString decryptString(QByteArray key, QString line);
QByteArray encryptStringToByteArray(QByteArray key, QString line);
QString decryptStringFromByteArray(QByteArray key, QByteArray data);
void encryptFile(QByteArray key, QString fileName);
void decryptFile(QByteArray key, QString fileName);
void encDecFileSmart(QByteArray key, QString fileName, int mode);

void setKineticScrollArea(QAbstractItemView *object);

int getScreenSizeX(void);
int getScreenSizeY(void);
qreal getCalculateIconSizePx(void);


void showMessageBox(QString message); // Выдача на экран простого окна с сообщением


// template <class X> inline X *find_object(QString n);


// Поиск объекта от корня по имени
template <class X> inline X *find_object(QString objectName)
{
 QObject *findObj;

 extern QObject *pMainWindow;

 // Если запрошен сам корень
 if(objectName=="mainwindow")
  {
   QObject *mvp=qobject_cast<X *>(pMainWindow);

   if(mvp->metaObject()->className()!=pMainWindow->metaObject()->className())
    {
     // Если запрошенный класс объекта не является классом главного окна
     printf("find_object(): Can't find mainwindow object. Check <type> in function call\n");
     exit(1);
     return NULL;
    }
   else
    return qobject_cast<X *>(pMainWindow);
  }

 // Запрошен обычный объект, надо его найти
 // findObj=qFindChild<X *>(pMainWindow, objectName);
 findObj=pMainWindow->findChild<X *>(objectName);

 if(findObj==NULL)
  {
   // Если объекта с указанным именем не найдено
   // print_object_tree();
   printf("find_object(): Can't find object with name %s\n",qPrintable(objectName));

   print_object_tree();

   /*
   QList<QWidget *> widgets = pMainWindow->findChildren<QWidget *>();
   foreach (QWidget* b, widgets)
     printf("Obj: %s\n", qPrintable( b->objectName() ) );
   */

   exit(1);
   return NULL;
  }
 else
  {
   // ОБъект был найден, и нужно преобразовать указатель на него
   // к указателю c заданным в шаблоне типом
   X *obj=qobject_cast<X *>(findObj);

   if(obj==0)
    {
     // Если найденный объект не может быть преобразован к заданному в шаблоне типу
     printf("find_object(): Object %s find, but can't convert type. Check <type> in function call\n",qPrintable(objectName));
     exit(1);
     return NULL;
    }
   else
    return obj; // Объект найден нормально
  }
}


class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

#endif // __MAIN_H__
