#include <QtGlobal>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QStyle>
#include <QPalette>
#include <QSettings>
#include <QRegularExpression>

#include "PaletteHelper.h"

#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;
extern FixedParameters fixedParameters;


PaletteHelper::PaletteHelper()
{

}

QMap<QString, QPalette::ColorRole> PaletteHelper::getColorMap()
{
    static QMap<QString, QPalette::ColorRole> colorMap = {
            {"Window",          QPalette::Window},
            {"WindowText",      QPalette::WindowText},
            {"Base",            QPalette::Base},
            {"AlternateBase",   QPalette::AlternateBase},
            {"Text",            QPalette::Text},
            {"Button",          QPalette::Button},
            {"ButtonText",      QPalette::ButtonText},
            {"BrightText",      QPalette::BrightText},
            {"Highlight",       QPalette::Highlight},
            {"HighlightedText", QPalette::HighlightedText},
            {"Link",            QPalette::Link},
            {"LinkVisited",     QPalette::LinkVisited},
            {"ToolTipBase",     QPalette::ToolTipBase},
            {"ToolTipText",     QPalette::ToolTipText}
        };

    return colorMap;
}


QStringList PaletteHelper::getColorNameList()
{
    return getColorMap().keys();
}


QString PaletteHelper::getNameByColorRole(QPalette::ColorRole role)
{
    QMap<QString, QPalette::ColorRole> colorMap = getColorMap();

    for (const QString &name : colorMap.keys())
    {
        if ( role == colorMap[name] )
        {
            return name;
        }
    }

    return "";
}


QPalette::ColorRole PaletteHelper::getColorRoleByName(const QString &name)
{
    QMap<QString, QPalette::ColorRole> colorMap = getColorMap();

    if ( colorMap.contains(name) )
    {
        return colorMap[name];
    }
    else
    {
        return QPalette::NoRole;
    }
}


void PaletteHelper::applyCustomPalette(const QMap<QString, QColor> &colorMap)
{
    QPalette palette;

    for ( const QString &name : colorMap.keys() ) {
        QPalette::ColorRole role = getColorRoleByName(name);

        if (role!=QPalette::NoRole)
        {
            palette.setColor(role, colorMap[name]);
        }
    }

    // Полученная палитра применяется
    qApp->setPalette(palette);
}


QMap<QString, QColor> PaletteHelper::parseCustomPalette(const QString &cssText)
{
    QMap<QString, QColor> result;

    // Начало блока CustomPalette
    int startPos = cssText.indexOf("CustomPalette", 0, Qt::CaseInsensitive);
    if (startPos == -1) {
        return result;
    }

    // Открывающая фигурная скобка
    int openBracePos = cssText.indexOf('{', startPos);
    if (openBracePos == -1) {
        return result;
    }

    // Закрывающая фигурная скобка
    int closeBracePos = cssText.indexOf('}', openBracePos + 1);
    if (closeBracePos == -1) {
        return result;
    }

    // Содержимое между скобками
    QString blockContent = cssText.mid(openBracePos + 1, closeBracePos - openBracePos - 1);

    QStringList lines = blockContent.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();

        // Пропуск пустых строк
        if (trimmedLine.isEmpty() ) {
            continue;
        }

        // Строка разделяется на имя свойства и значение
        int colonPos = trimmedLine.indexOf(':');
        if (colonPos == -1) {
            continue;
        }

        QString propertyName = trimmedLine.left(colonPos).trimmed();
        QString valuePart = trimmedLine.mid(colonPos + 1).trimmed();

        // Убирается точка с запятой в конце если есть
        if (valuePart.endsWith(';')) {
            valuePart.chop(1);
            valuePart = valuePart.trimmed();
        }

        // Создается QColor из значения
        QColor color;

        if (valuePart.startsWith('#')) {
            // HEX форматы: #RGB, #RRGGBB, #AARRGGBB
            color = QColor(valuePart);
        } else if (valuePart.contains("rgb(") || valuePart.contains("rgba(")) {
            // RGB/RGBA форматы
            QRegularExpression rgbRegex(R"(rgba?\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*(?:,\s*([\d.]+)\s*)?\))");
            QRegularExpressionMatch match = rgbRegex.match(valuePart);
            if (match.hasMatch()) {
                int r = match.captured(1).toInt();
                int g = match.captured(2).toInt();
                int b = match.captured(3).toInt();
                if (match.captured(4).isEmpty()) {
                    color = QColor(r, g, b);
                } else {
                    float alpha = match.captured(4).toFloat();
                    color = QColor(r, g, b, qRound(alpha * 255));
                }
            }
        } else {
            // Именованные цвета и другие форматы
            color = QColor(valuePart);
        }

        if (color.isValid()) {
            result.insert(propertyName, color);
        } else {
            qDebug() << "Failed to parse color:" << valuePart << "for property:" << propertyName;
        }
    }

    return result;
}
