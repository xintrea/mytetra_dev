#ifndef DISKHELPER_H
#define DISKHELPER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QByteArray>

// Класс, помогающий работать с дисковой подсистемой

class DiskHelper {
public:
    DiskHelper();

    static void removeDirectoryToTrash(QString nameDirFrom);
    static void removeFileToTrash(QString fileNameFrom);
    static QString copyFileToTrash(QString fileNameFrom);
    static QString createTempDirectory(void);
    static bool removeDirectory(const QString &dirName);
    static bool copyDirectory(const QString &fromName, const QString &toName);
    static QMap<QString, QByteArray> getFilesFromDirectory(QString dirName, QString fileMask);
    static bool saveFilesToDirectory(QString dirName, QMap<QString, QByteArray> fileList);
};

#endif // DISKHELPER_H
