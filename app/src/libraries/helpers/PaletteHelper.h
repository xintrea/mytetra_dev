#ifndef PALETTEHELPER_H
#define PALETTEHELPER_H

#include <QtGlobal>
#include <QPalette>


class PaletteHelper
{
public:
    PaletteHelper();
    
    static QMap<QString, QColor> parseCustomPalette(const QString &cssText);
    static void applyCustomPalette(const QMap<QString, QColor> &colorMap);

protected:

    static QString getNameByColorRole(QPalette::ColorRole role);
    static QPalette::ColorRole getColorRoleByName(const QString &name);

    static QMap<QString, QPalette::ColorRole> getColorMap();
    static QStringList getColorNameList();
};

#endif // PALETTEHELPER_H
