#include "TraceLogger.h"

#include <QDebug>


// Статическая переменная, хранящая отступ (для отрисовки вложения)
QString TraceLogger::indent;


TraceLogger::TraceLogger(const char* iFileName,
                         const char* iFuncName,
                         int iLineNumber)
{
    fileName = iFileName;
    funcName = iFuncName;
    qDebug() << qPrintable(indent) << "> " << qPrintable(QString(funcName)) << "() - (" << qPrintable(QString(fileName)) << ":" << qPrintable(QString().setNum(iLineNumber)) << ")";
    indent.append("  ");
}

TraceLogger::~TraceLogger()
{
    indent.resize(indent.length() - 2);
    qDebug() << qPrintable(indent) << "< " << qPrintable(QString(funcName)) << "() - (" << qPrintable(QString(fileName)) << ")";
}


