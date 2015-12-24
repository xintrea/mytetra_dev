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
    qDebug() << indent << "Entering " << QString(funcName) << "() - (" << QString(fileName) << ":" << QString().setNum(iLineNumber) << ")";
    indent.append("  ");
}

TraceLogger::~TraceLogger()
{
    indent.resize(indent.length() - 2);
    qDebug() << indent << "Leaving  " << QString(funcName) << "() - (" << QString(fileName) << ")";
}


