#ifndef ACTIONLOGGER_H
#define ACTIONLOGGER_H

#include <QString>
#include <QFile>

class ActionLogger
{
public:

  enum action { createNote,
                editNote,
                moveNoteUp,
                moveNoteDown,
                deleteNote,

                createBranch,
                editBranch,
                moveBranchUp,
                moveBranchDown,
                deleteBranch,

                copyNoteToBuffer,
                cutNoteToBuffer,
                pasteNoteFromBuffer,

                dragAndDropNote };


  ActionLogger();
  virtual ~ActionLogger();

  void addActionCreateRecord(QString recordId, QString recordName, QString branchId, QString branchName);
  void addActionEditRecord(QString recordId, QString recordName);
  void addActionDeleteRecord(QString recordId, QString recordName);
  void addActionMoveUpRecord(QString recordId, QString recordName);
  void addActionMoveDownRecord(QString recordId, QString recordName);
  void addActionCutRecord(QString recordId, QString recordName);
  void addActionPasteRecord(QString recordId, QString recordName);

  QString getFullDescription(int iAction, QMap<QString, QString> iData);

protected:

  QString logFileName;
  QString logPrevFileName;

  QFile logFile;

};

#endif // ACTIONLOGGER_H
