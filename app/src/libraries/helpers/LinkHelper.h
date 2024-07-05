#ifndef LINKHELPER_H
#define LINKHELPER_H

class QString;
class QUrl;

class LinkHelper
{
public:
    LinkHelper();

    static bool openLinkWithDesktopServices(const QString &link);
    static bool isExternal(const QUrl &url);

};

#endif // LINKHELPER_H
