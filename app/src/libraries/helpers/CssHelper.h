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

    static void removeVeryOldCssStyles();

    static void loadCurrentTheme();
    static bool applyTheme(const QString &themeName);

protected:

    static QString replaceCssMetaIconSize(const QString &styleText);
    static void fineTuneHrefColor(const QString &themeName);

};

#endif // CSSHELPER_H
