#ifndef LINKHELPER_H
#define LINKHELPER_H

class QString;
class QUrl;

class LinkHelper
{
public:
    LinkHelper();

    static void gotoReference(QString href);

private:

    static bool openLinkWithDesktopServices(const QString &link);

    static bool isExternal(const QUrl &url);
    static bool isHrefInternal(QString href);
    static QString getIdFromInternalHref(QString href);

};

#endif // LINKHELPER_H
