#include <QUrl>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QDesktopServices>
#include <QDebug>

#include "LinkHelper.h"


LinkHelper::LinkHelper()
{

}


bool LinkHelper::openLinkWithDesktopServices(const QString &link)
{
    // qDebug() << "Try open link " << link;

    QUrl url = QUrl(link);

    // Использовать метод QUrl::isLocalFile() нельзя, так как он просто
    // возвращает true если схема "file" и все.
    // Вместо этого написана специальная функция определения, внешняя это
    // или внутренняя ссылка
    if ( LinkHelper::isExternal( url ) )
    {
        // Для внешних ссылок используется QDesktopServices
        return QDesktopServices::openUrl(url);
    }
    else
    {
        // Определено, что передана локальная ссылка

        QString filePath = link; // url.toLocalFile();

        // qDebug() << "Try open file " << filePath;

        // Используем QProcess для вызова системной команды
#ifdef Q_OS_WIN
        QStringList args;
        args << "/C" << "start" << "" << filePath.replace("/", "\\");
        QProcess::startDetached("cmd", args);
#elif defined(Q_OS_MAC)
        QProcess::startDetached("open", QStringList() << filePath);
#elif defined(Q_OS_LINUX)
        QProcess::startDetached("xdg-open", QStringList() << filePath);
#else
        return false; // Не поддерживается на других платформах
#endif
        return true;
    }
}


bool LinkHelper::isExternal(const QUrl &url)
{
    // Проверка схемы URL
    QString scheme = url.scheme().toLower();

    QStringList external = QStringList() << "http"
                                         << "https"
                                         << "ftp"
                                         << "sftp";

    return external.contains( scheme );
}

