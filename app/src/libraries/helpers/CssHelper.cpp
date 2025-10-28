#include <QtGlobal>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QDebug>

#include "CssHelper.h"

#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "models/appConfig/AppConfig.h"
#include "models/appConfig/AppFiles.h"
#include "libraries/helpers/DiskHelper.h"

extern GlobalParameters globalParameters;
extern FixedParameters fixedParameters;
extern AppConfig mytetraConfig;
extern AppFiles mytetraFiles;


CssHelper::CssHelper()
{

}


qreal CssHelper::getCalculateIconSizePx(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
    qreal dpiX=qApp->desktop()->physicalDpiX();
    qreal dpiY=qApp->desktop()->physicalDpiY();
    qreal dpi=(dpiX+dpiY)/2;
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
    qreal dpi=QApplication::screens().at(0)->physicalDotsPerInch();
#endif

    qreal iconSizeMm=6; // Размер иконки в миллиметрах (рекомендованный)
    qreal iconSizeInch=iconSizeMm/25.4; // Размер иконки в дюймах
    qreal iconSizePx=iconSizeInch*dpi;

    return iconSizePx;
}


// Замена в CSS-стиле все вхождения подстроки META_ICON_SIZE на вычисленный размер иконки в пикселях
QString CssHelper::replaceCssMetaIconSize(const QString &styleText)
{
    QMap<QString, qreal> table;
    table["META_ICON_FOUR_SIZE"]       = getCalculateIconSizePx() * 4.0;
    table["META_ICON_TRIPLE_SIZE"]     = getCalculateIconSizePx() * 3.0;
    table["META_ICON_DOUBLE_SIZE"]     = getCalculateIconSizePx() * 2.0;
    table["META_ICON_ONEANDHALF_SIZE"] = getCalculateIconSizePx() * 1.5;

    table["META_ICON_SIZE"] = getCalculateIconSizePx();

    table["META_ICON_SEVFIVEPERC_SIZE"]  = getCalculateIconSizePx() * 0.75;
    table["META_ICON_HALF_SIZE"]         = getCalculateIconSizePx() / 2.0;
    table["META_ICON_THIRD_PART_SIZE"]   = getCalculateIconSizePx() / 3.0;
    table["META_ICON_QUARTER_PART_SIZE"] = getCalculateIconSizePx() / 4.0;

    QString resultText = styleText;

    for (auto name : table.keys())
    {
      qreal value=table.value(name);
      resultText.replace( name, QString::number( (int) value )+"px" );
    }

    return resultText;
}


void CssHelper::extractCssStyles()
{
    QString dirName=globalParameters.getWorkDirectory();

    // Если в рабочей директории нет каталога с темами
    QDir themesDir(dirName+"/themes");
    if ( !themesDir.exists() )
    {
        // Каталог с темами распаковывается из ресурсов
        mytetraFiles.createThemesFiles( dirName+"/themes" );
    }

    /*
    // Если файл стилей есть по старому пути, он переносится в каталог themes
    // поверх файла стандартной темы, а в старом месте удаляется
    // Старый файл темы был самодостаточным и не использовал внешние ресурсы
    QFile fromFile(dirName+"/stylesheet.css");
    if (fromFile.exists())
    {
      // На всякий случай создается каталог с default-темой, если его
      // не было в ресурсах (подумать, а надо ли это делать)
      QDir themeDir(dirName);
      themeDir.mkpath("themes/default");

      // Метод rename переместит файл, причем на старом месте он будет удален
      fromFile.rename(dirName+"/themes/default/stylesheet.css");
    }
    */
}


void CssHelper::removeOldCssStyles()
{
    QString dirName=globalParameters.getWorkDirectory();

    // Устаревший файл стиля, который ранее находился рядом с конфиг-файлами
    // а не в отдельной директории, удаляется
    QFile fromFile(dirName+"/stylesheet.css");
    if ( fromFile.exists() )
    {
        fromFile.remove();
    }

    // Устаревший каталог стиля, который делал пользователь gee12, удаляется
    QDir styleDir(dirName+"/style");
    if ( !styleDir.exists() )
    {
        DiskHelper::removeDirectory( styleDir.absolutePath() );
    }
}


void CssHelper::loadCurrentTheme()
{
    applyTheme( mytetraConfig.getInterfaceTheme() );
}


bool CssHelper::applyTheme(const QString &themeName)
{
    if ( !fixedParameters.themesAvailableList.contains(themeName) )
    {
        qWarning() << "Incorrect interface theme name: " << themeName;
        return false;
    }

    QString dirName = globalParameters.getWorkDirectory();
    QString fileName = dirName+"/themes/"+themeName+"/stylesheet.css";

    QFile cssFile( fileName );

    bool openResult=cssFile.open(QIODevice::ReadOnly | QIODevice::Text);

    // Если файла не существует
    if(!openResult)
    {
        qWarning() << "Can't find CSS theme file: " << fileName;
        return false;
    }

    // Берется содержимое CSS-файла
    QString styleText = QTextStream(&cssFile).readAll();

    // Преобразовывается размер иконок из подстановочных названий
    // в настоящие пиксели
    styleText=CssHelper::replaceCssMetaIconSize(styleText);

    // Стиль применяется
    qApp->setStyleSheet(styleText);

    return true;
}

