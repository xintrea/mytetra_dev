#ifndef OBJECTHELPER_H
#define OBJECTHELPER_H

#include <QObject>

#include "libraries/helpers/DebugHelper.h"

// Функции для работы с объектами QObject


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
            return nullptr;
        }
        else
            return qobject_cast<X *>(pMainWindow);
    }

    // Запрошен обычный объект, надо его найти
    // findObj=qFindChild<X *>(pMainWindow, objectName);
    findObj=pMainWindow->findChild<X *>(objectName);

    if(!findObj)
    {
        // Если объекта с указанным именем не найдено
        // print_object_tree();
        printf("find_object(): Can't find object with name %s\n",qPrintable(objectName));

        printObjectTree();

        /*
        QList<QWidget *> widgets = pMainWindow->findChildren<QWidget *>();
        foreach (QWidget* b, widgets)
            printf("Obj: %s\n", qPrintable( b->objectName() ) );
        */

        exit(1);
    }
    else
    {
        // Объект был найден, и нужно преобразовать указатель на него
        // к указателю c заданным в шаблоне типом
        X *obj=qobject_cast<X *>(findObj);

        if(obj==0)
        {
            // Если найденный объект не может быть преобразован к заданному в шаблоне типу
            printf("find_object(): Object %s find, but can't convert type. Check <type> in function call\n",qPrintable(objectName));
            exit(1);
        }
        else
            return obj; // Объект найден нормально
    }

    return nullptr;
}


//class ObjectHelper
//{
//public:
//    ObjectHelper();
//};

#endif // OBJECTHELPER_H
