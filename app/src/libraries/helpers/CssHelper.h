#ifndef CSSHELPER_H
#define CSSHELPER_H

#include <QtGlobal>


class CssHelper
{
public:
    CssHelper();

    // Получение стандартного единичного размера иконок, используется для
    // высчитывания относительного размера других элементов интерфейса
    static qreal getCalculateIconSizePx(void);

    // Раскладывание CSS-стилей на диск в подкаталог рабочей директории
    static void extractCssStyles();

    // Загрузка текущей выставленной в конфигурации темы
    static void loadCurrentTheme();

protected:

    static void removeVeryOldCssStyles();
    static void removePreviousThemes();

    static QString removeCssComments(const QString& input);
    static QString replaceCssMetaIconSize(const QString &styleText);

    static QString getCssFileContentForTheme(const QString &themeName, bool fromResources=false);
    static int getMyTetraStyleVersion(const QString& text);

    static void loadPalette(const QString &cssText);

    static bool applyTheme(const QString &themeName);

    static void updateInterface();

};

#endif // CSSHELPER_H
