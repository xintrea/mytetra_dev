#ifndef TRACELOGGER_H
#define TRACELOGGER_H

#include <QString>

class TraceLogger
{
public:
  TraceLogger(const char* iFileName,
              const char* iFuncName,
              int iLineNumber);
  ~TraceLogger();

private:
    const char* fileName;
    const char* funcName;

    static QString indent;
};

#endif // TRACELOGGER_H
