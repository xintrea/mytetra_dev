#ifndef CSSHELPER_H
#define CSSHELPER_H

#include <QtGlobal>


class CssHelper
{
public:
    CssHelper();

    static qreal getCalculateIconSizePx(void);
    static void setCssStyle();

protected:

    static QString replaceCssMetaIconSize(QString styleText);

};

#endif // CSSHELPER_H
