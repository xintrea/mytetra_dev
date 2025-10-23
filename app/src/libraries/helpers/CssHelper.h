#ifndef CSSHELPER_H
#define CSSHELPER_H

#include <QtGlobal>


class CssHelper
{
public:
    CssHelper();

    static qreal getCalculateIconSizePx(void);

    // Раскладывание CSS-стилей на диск в подкаталог рабочей директории
    static void extractCssStyles();

    static void removeOldCssStyles();

    static bool applyTheme(const QString &themeName);

protected:

    static QString replaceCssMetaIconSize(const QString &styleText);

};

#endif // CSSHELPER_H
