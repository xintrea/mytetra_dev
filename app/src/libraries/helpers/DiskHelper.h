#ifndef DISKHELPER_H
#define DISKHELPER_H

#include <QFile>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QByteArray>

// Класс, помогающий работать с дисковой подсистемой

class DiskHelper
{
public:
  DiskHelper();

  static void removeDirectoryToTrash(QString nameDirFrom);
  static bool removeFileToTrash(QString fileNameFrom, bool stopIfError=true);
  static QString copyFileToTrash(QString fileNameFrom, bool stopIfError=true);
  static bool isTrashDirectoryExists();

  static QString createTempDirectory(void);
  static bool removeDirectory(const QString &dirName);
  static bool copyDirectory(const QString &fromName, const QString &toName);
  static bool copyDirectoryRecursively(const QString &fromName, const QString &toName, QFile::Permissions permissionSpec);
  static QMap<QString, QByteArray> getFilesFromDirectory(QString dirName, QString fileMask);
  static bool saveFilesToDirectory(QString dirName, QMap<QString, QByteArray> fileList);

  static bool isDirectoryEmpty(QString dirName);

  static bool copyQrcToDirectory(const QString &resourcePath, const QString &targetDirPath);

private:

  static bool copyQrcToDirectoryRecurse(const QString &resourcePath,
                                        const QString &targetDirPath,
                                        bool includeRootDir);

};

#endif // DISKHELPER_H
